#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <stdint.h>

#include "machine_def.h"
#include "opcode_defs.h"
#include "opcodes.h"
#include "machine.h"
#include "vmem.h"
#include "io_handler.h"
#include "timer.h"

#define WORD_SIZE 4
#define GET_INTVAL(reg, off) (Get_Word(cpu, cpu-> reg + OFFSET(off)))
#define SET_INTVAL(reg, off, val) (Set_Word(cpu, cpu-> reg + OFFSET(off), val))
#define INC(reg, amount)    cpu-> reg += amount*WORD_SIZE;
#define OFFSET(v)           ((v)*WORD_SIZE)

#define LINES_PER_HEADER 30     // for opcode lists

int Do_Debug = 0;
int Max_Instructions = INT_MAX;

static int debug_break()
{
    return 0;
}

void Opcodes_Debug()
{
    Do_Debug = 1;
}

void Set_Max_Instr(int max)
{
    Max_Instructions = max;
}

// set DEBUG to "//" to turn off DEBUG
#define DEBUG(...) debug_print(cpu,__VA_ARGS__);
static void debug_print(Machine_State *cpu, const char *fmt, ...)
{
    static int num_lines = 0;
    va_list args;
    va_start(args, fmt);
    char format[200];

    if (!Do_Debug) return;

    if (num_lines % LINES_PER_HEADER == 0)
    {
        fprintf(stderr, "\n%8s %6s %6s %6s %6s %6s\n", 
                "Flag", "BP", "LP", "IP", "SP", "FP");
    }
    num_lines++;

    sprintf(format, "%08x %6d %6d %6d %6d %6d %s\n", 
            cpu->FLAG, cpu->BP, cpu->LP, cpu->IP, cpu->SP, cpu->FP, fmt);
    vfprintf(stderr, format, args);
    va_end(args);
}

//***************************************
static void push(Machine_State *cpu, int32_t value)
{
    Set_Word(cpu, cpu->SP, value);
    cpu->SP += WORD_SIZE;
}
//***************************************
static int32_t pop(Machine_State *cpu)
{
    cpu->SP -= WORD_SIZE;
    return Get_Word(cpu, cpu->SP);
}
//***********************************
static int check_priv(Machine_State *cpu, const char *inst_name)
{
    if (cpu->FLAG & FL_USER_MODE)
    {
        Machine_Check(MC_PROT_INST, 
                "%s instruction while not in system mode", inst_name);
        return 0;
    }
    return 1;
}
//***********************************
// determine if the list of addresses will generate a page fault
// FIX THIS: should be static, but haven't inserted calls yet
int check_mem(Machine_State *cpu, int num_addr, ...)
{
    va_list args;
    int addr;
    int ii;

    if (cpu->FLAG & FL_I_PF) return 0;

    va_start(args, num_addr);

    for (ii=0; ii<num_addr; ii++)
    {
        addr = (int)va_arg(args, int);

        // we're only interested in PF, not alignment, 
        // so pretend all addresses are byte addresses
        VM_Check_Addr(cpu, addr, 1);
        if (cpu->FLAG & FL_I_PF) 
        {
            va_end(args);
            return 0;
        }
    }
    va_end(args);

    return 1;
}
//***************************************
static void do_rti(Machine_State *cpu)
{
    int32_t flag;
    int32_t new_flag;
    flag = cpu->FLAG;

    if (check_priv(cpu, "RTI"))
    {
        cpu->FP = pop(cpu);
        cpu->IP = pop(cpu);
        cpu->LP = pop(cpu);
        cpu->BP = pop(cpu);
        new_flag = pop(cpu);    // cant store flag until after restoring SP
        cpu->SP = pop(cpu);

        cpu->FLAG = new_flag;

        // re-enable any interrupts that occurred during this ISR
        cpu->FLAG |= flag & FL_I_ALL;

        //if (cpu->FLAG & FL_USER_MODE) 
        //    cpu->SP -= cpu->BP;
    }
}
//***********************************************
static void interrupt(Machine_State *cpu, int is_trap)
{
    int32_t was_user;
    int32_t vector;

    was_user = cpu->FLAG & FL_USER_MODE;

    if (is_trap) 
    {
        vector = TRAP_VECTOR;
    }
    else
    {
        // Find highest priority pending interrupt
        for (vector=0; vector<16; vector++)
        {
            if (cpu->FLAG & (FL_I_FIRST_INT << vector)) break;
        }

        if (vector == 16) return;   // no pending interrupts

        // turn off pending bit for HW interrupts
        cpu->FLAG &= ~(FL_I_FIRST_INT << vector);

        // if IVEC not set, use ISR at offset zero
        if (cpu->IVEC == 0)
        {
            vector = HW_INTERRUPT_VECTOR;
        }
    }

    push(cpu, cpu->SP);
    push(cpu, cpu->FLAG);
    push(cpu, cpu->BP);
    push(cpu, cpu->LP);
    push(cpu, cpu->IP);
    push(cpu, cpu->FP);

    // Don't update the FP for trap instructions, so
    // trap handler can see args from caller
    if (!is_trap) cpu->FP = cpu->SP;

    // go to system mode and interrupt mode
    cpu->FLAG &= ~FL_USER_MODE;
    cpu->FLAG |= FL_INT_MODE;

    if (was_user)
    {
        // Switch FP and SP to absolute addresses
        cpu->FP += cpu->BP;
        cpu->SP += cpu->BP;
    }

    // ISR is at vector
    cpu->IP = Get_Word(cpu, cpu->IVEC + vector*WORD_SIZE);
}

int rotate_left(int value, int count)
{
    int shiftc = count % 32;
    int mask = ~(0xffffffff << shiftc);
    return (value << shiftc) | (mask & (value >> (32 - shiftc)));
}

int rotate_right(int value, int count)
{
    return rotate_right(value, -count);
}

//***************************************
void Execute(Machine_State *cpu)
{
    int32_t temp;
    static int num_instructions = 0;

    if (Max_Instructions != INT_MAX && ++num_instructions >= Max_Instructions) 
    {
        cpu->FLAG |= FL_HALTED;
        return;
    }

    Timer_Heartbeat();

    if ( (cpu->FLAG & FL_I_ALL) && 
        !(cpu->FLAG & FL_INT_MODE)    &&
        !(cpu->FLAG & FL_INT_DIS))
    {
        DEBUG("INTERRUPT");
        interrupt(cpu, 0);
        return;
    }

    int32_t val = GET_INTVAL(IP, 0);
    
    switch (val)
    {
        case NOP_OP:
            DEBUG("NOP");
            INC(IP, 1);
            break;
        case PLUS_OP:
            DEBUG("PLUS");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) + GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case MINUS_OP:
            DEBUG("MINUS");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) - GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case TIMES_OP:
            DEBUG("TIMES");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) * GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case DIVIDE_OP:
            DEBUG("DIVIDE");
            if (GET_INTVAL(SP, -1) != 0)
            {
                SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) / GET_INTVAL(SP, -1)));
            }
            else
            {
                Machine_Check(MC_DIVIDE_ZERO,
                        "Divide by zero at %d\n", cpu->IP);
            }
            INC(SP, -1);
            INC(IP, 1);
            break;
        case MOD_OP:
            DEBUG("MOD");
            if (GET_INTVAL(SP, -1) != 0)
            {
                SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) % GET_INTVAL(SP, -1)));
            }
            else
            {
                Machine_Check(MC_DIVIDE_ZERO,
                        "Divide by zero at %d\n", cpu->IP);
            }
            INC(SP, -1);
            INC(IP, 1);
            break;
        case NOT_OP:
            DEBUG("NOT");
            SET_INTVAL(SP, -1, !GET_INTVAL(SP, -1));
            INC(IP, 1);
            break;
        case COMP_OP:
            DEBUG("COMP");
            SET_INTVAL(SP, -1, ~GET_INTVAL(SP, -1));
            INC(IP, 1);
            break;
        case NEG_OP:
            DEBUG("NEG");
            SET_INTVAL(SP, -1, -GET_INTVAL(SP, -1));
            INC(IP, 1);
            break;
        case EQ_OP:
            DEBUG("EQ");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) == GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case NE_OP:
            DEBUG("NE");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) != GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case GT_OP:
            DEBUG("GT");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) > GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case LT_OP:
            DEBUG("LT");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) < GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case GE_OP:
            DEBUG("GE");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) >= GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case LE_OP:
            DEBUG("LE");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) <= GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case AND_OP:
            DEBUG("AND");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) && GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case OR_OP:
            DEBUG("OR");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) || GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case BAND_OP:
            DEBUG("BAND");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) & GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case BOR_OP:
            DEBUG("BOR");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) | GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case BXOR_OP:
            DEBUG("BXOR");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) ^ GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case SHIFTL_OP:
            DEBUG("SHIFTL");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) << GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case SHIFTR_OP:
            DEBUG("SHIFTR");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) >> GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case HALT_OP:
            DEBUG("HALT");
            if (check_priv(cpu, "HALT")) cpu->FLAG |= FL_HALTED;
            break;
        case POP_OP:
            DEBUG("POP");
            INC(SP, -1);
            INC(IP, 1);
            break;
        case CALL_OP:
            DEBUG("CALL %d", GET_INTVAL(IP, 1));
            temp = GET_INTVAL(IP, 1);
            SET_INTVAL(SP, 0, (cpu->IP + OFFSET(2)));
            SET_INTVAL(SP, 1, (cpu->FP));
            INC(SP, 2);
            cpu->FP = cpu->SP;
            cpu->IP = temp;
            break;
        case CALLI_OP:
            DEBUG("CALLI %d", GET_INTVAL(SP, -1));
            INC(SP, -1);
            temp = GET_INTVAL(SP, 0);
            SET_INTVAL(SP, 0, (cpu->IP + OFFSET(1)));
            SET_INTVAL(SP, 1, (cpu->FP));
            INC(SP, 2);
            cpu->FP = cpu->SP;
            cpu->IP = temp;
            break;
        case RETURN_OP:
            DEBUG("RETURN");
            cpu->SP = cpu->FP - OFFSET(2);
            cpu->IP = GET_INTVAL(FP, -2);
            cpu->FP = GET_INTVAL(FP, -1);
            break;
        case RETURNV_OP:
            DEBUG("RETURNV");
            temp = GET_INTVAL(SP, -1);
            cpu->SP = cpu->FP - OFFSET(1);
            cpu->IP = GET_INTVAL(FP, -2);
            cpu->FP = GET_INTVAL(FP, -1);
            SET_INTVAL(SP, -1, temp);
            break;
        case PUSH_OP:
            DEBUG("PUSH %d", GET_INTVAL(IP, 1));
            INC(IP, 1);
            SET_INTVAL(SP, 0, GET_INTVAL(IP, 0));
            INC(SP, 1);
            INC(IP, 1);
            break;
        case PUSHFP_OP:
            DEBUG("PUSHFP %d", cpu->FP);
            SET_INTVAL(SP, 0, cpu->FP);
            INC(SP, 1);
            INC(IP, 1);
            break;
        case CLID_OP:
            DEBUG("CLID");
            if (check_priv(cpu, "CLID"))
            {
                SET_INTVAL(SP, 0, cpu->FLAG & FL_INT_DIS); 
                cpu->FLAG &= ~FL_INT_DIS;
                INC(SP, 1);
                INC(IP, 1);
            }
            break;
        case SEID_OP:
            DEBUG("SEID");
            if (check_priv(cpu, "SEID"))
            {
                SET_INTVAL(SP, 0, cpu->FLAG & FL_INT_DIS); 
                cpu->FLAG |= FL_INT_DIS;
                INC(SP, 1);
                INC(IP, 1);
            }
            break;
        case OUTS_OP:
            DEBUG("OUTS %d", GET_INTVAL(SP,-1));
            if (check_priv(cpu, "OUTS"))
            {
                temp = pop(cpu);
                printf("%s", (char *)Get_Addr(cpu, temp));
                fflush(stdout);
                INC(IP, 1);
            }
            break;
        case INP_OP:
            temp = pop(cpu);
            DEBUG("INP %d %d", Get_Word(cpu, temp), Get_Word(cpu, temp+4));
            if (check_priv(cpu, "INP"))
            {
                Schedule_IO(cpu, temp);
                INC(IP, 1);
            }
            break;
        /*
        case TRAPTOC_OP:
            DEBUG("TRAPTOC_OP %d %d", GET_INTVAL(FP,-3), GET_INTVAL(FP,-4));
            {
                int32_t args[20];
                int32_t ii;
                args[0] = GET_INTVAL(FP, -3);
                for (ii=1; ii<args[0]; ii++)
                {
                    args[ii] = GET_INTVAL(FP, -ii-3);
                }
                //INC(SP, -args[0]);
                INC(IP, 1);
                temp = syscall(args);
                SET_INTVAL(SP, 0, temp);
                INC(SP, 1);
            }
            break;
        */
        case TRAP_OP:
            DEBUG("TRAP %d %d", GET_INTVAL(FP,-3), GET_INTVAL(FP,-4));
            INC(IP, 1);
            interrupt(cpu, 1);
            break;
        case RTI_OP:
            DEBUG("RTI");
            do_rti(cpu);
            break;
        case JUMP_OP:
            DEBUG("JUMP %d", GET_INTVAL(IP, 1));
            cpu->IP = GET_INTVAL(IP, 1);
            break;
        case JUMPE_OP:
            DEBUG("JUMPE %d %d", GET_INTVAL(SP, -1), GET_INTVAL(IP, 1));
            INC(IP, 1);
            INC(SP, -1);
            if (!GET_INTVAL(SP, 0))
                cpu->IP = GET_INTVAL(IP,0);
            else
                INC(IP, 1);
            break;
        case PUSHCVARIND_OP:
            temp = Get_Byte(cpu, GET_INTVAL(SP, -1));
            DEBUG("PUSHCVARIND %d %d", GET_INTVAL(SP, -1), temp);
            SET_INTVAL(SP,-1, temp);
            INC(IP, 1);
            break;
        case POPCVARIND_OP:
            DEBUG("POPCVARIND %d %d", GET_INTVAL(SP, -2), GET_INTVAL(SP, -1));
            temp = GET_INTVAL(SP, -1);
            Set_Byte(cpu, temp, GET_INTVAL(SP, -2));
            INC(SP, -2);
            INC(IP, 1);
            break;
        case PUSHVARIND_OP:
            temp = Get_Word(cpu, GET_INTVAL(SP, -1));
            DEBUG("PUSHVARIND %d %d", GET_INTVAL(SP, -1), temp);
            SET_INTVAL(SP,-1, temp);
            INC(IP, 1);
            break;
        case POPVARIND_OP:
            DEBUG("POPVARIND %d %d", GET_INTVAL(SP, -2), GET_INTVAL(SP, -1));
            temp = GET_INTVAL(SP, -1);
            Set_Word(cpu, temp, GET_INTVAL(SP, -2));
            INC(SP, -2);
            INC(IP, 1);
            break;
        case PUSHVAR_OP:
            temp = cpu->FP + GET_INTVAL(IP, 1);
            DEBUG("PUSHVAR %d %d", GET_INTVAL(IP, 1), Get_Word(cpu, temp));
            INC(IP, 1);
            SET_INTVAL(SP,0, Get_Word(cpu, temp));
            INC(SP, 1);
            INC(IP, 1);
            break;
        case POPVAR_OP:
            DEBUG("POPVAR %d %d", GET_INTVAL(IP,1), GET_INTVAL(SP,-1));
            INC(IP, 1);
            temp = cpu->FP + GET_INTVAL(IP, 0);
            INC(SP, -1);
            Set_Word(cpu, temp, GET_INTVAL(SP,0));
            INC(IP,1);
            break;
        case PUSHCVAR_OP:
            temp = cpu->FP + GET_INTVAL(IP, 1);
            DEBUG("PUSHCVAR %d %d", GET_INTVAL(IP, 1), Get_Byte(cpu, temp));
            Set_Word(cpu, cpu->SP, Get_Byte(cpu, temp));
            INC(SP, 1);
            INC(IP, 2);
            break;
        case POPCVAR_OP:
            DEBUG("POPCVAR %d %d", GET_INTVAL(IP,1), GET_INTVAL(SP,-1));
            temp = cpu->FP + GET_INTVAL(IP, 1);
            INC(SP, -1);
            Set_Byte(cpu, temp, Get_Byte(cpu, cpu->SP));
            INC(IP,2);
            break;
        case JMPUSER_OP:
            DEBUG("JMPUSER_%d", GET_INTVAL(IP,1));
            if (check_priv(cpu, "JMPUSER"))
            {
                temp = GET_INTVAL(IP,1);

                // need to update regs to reflect user mode
                cpu->FLAG |= FL_USER_MODE;
                //cpu->SP -= cpu->BP;
                //cpu->FP -= cpu->BP;
                cpu->IP = temp;
            }

            break;
        case PUSHREG_OP:
            DEBUG("PUSHREG %d", GET_INTVAL(IP,1));
            temp = GET_INTVAL(IP,1);
            INC(IP,2);
            switch (temp)
            {
                case BP_REG:
                    push(cpu, cpu->BP);
                    break;
                case LP_REG:
                    push(cpu, cpu->LP);
                    break;
                case IP_REG:
                    push(cpu, cpu->IP);
                    break;
                case SP_REG:
                    push(cpu, cpu->SP);
                    break;
                case FP_REG:
                    push(cpu, cpu->FP);
                    break;
                case FLAG_REG:
                    push(cpu, cpu->FLAG);
                    break;
                case IVEC_REG:
                    push(cpu, cpu->IVEC);
                    break;
            }
            break;
        case POPREG_OP:
            DEBUG("POPREG %d", GET_INTVAL(IP,1));
            debug_break();
            if (check_priv(cpu, "POPREG"))
            {
                temp = GET_INTVAL(IP,1);
                INC(IP,2);
                switch (temp)
                {
                    case BP_REG:
                        cpu->BP = pop(cpu);
                        break;
                    case LP_REG:
                        cpu->LP = pop(cpu);
                        break;
                    case IP_REG:
                        cpu->IP = pop(cpu);
                        break;
                    case SP_REG:
                        cpu->SP = pop(cpu);
                        break;
                    case FP_REG:
                        cpu->FP = pop(cpu);
                        break;
                    case FLAG_REG:
                        cpu->FLAG = pop(cpu);
                        break;
                    case IVEC_REG:
                        cpu->IVEC = pop(cpu);
                        break;
                }
            }
            break;
        case ADJSP_OP:
            DEBUG("ADJSP %d", GET_INTVAL(IP,1));
            INC(IP,1);
            cpu->SP += GET_INTVAL(IP,0);
            INC(IP,1);
            break;
        case POPARGS_OP:
            DEBUG("POPARGS %d", GET_INTVAL(IP,1));
            INC(IP,1);
            temp = GET_INTVAL(SP, -1);
            cpu->SP -= GET_INTVAL(IP,0);
            SET_INTVAL( SP, -1, temp);
            INC(IP,1);
            break;
        case SWAP_OP:
            DEBUG("SWAP %d %d", GET_INTVAL(SP, -1), GET_INTVAL(SP, -2));
            temp = GET_INTVAL(SP, -1);
            SET_INTVAL( SP, -1, GET_INTVAL(SP, -2));
            SET_INTVAL(SP, -2, temp);
            INC(IP,1);
            break;
        case DUP_OP:
            DEBUG("DUP");
            SET_INTVAL(SP, 0, GET_INTVAL(SP, -1));
            INC(SP, 1);
            INC(IP,1);
            break;
        case TRACE_ON_OP:
            Do_Debug = 1;
            INC(IP,1);
            break;
        case TRACE_OFF_OP:
            Do_Debug = 0;
            INC(IP,1);
            break;
        case ROL_OP:
            DEBUG("SHIFTL");
            SET_INTVAL(SP, -2, rotate_left(GET_INTVAL(SP, -2), GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case ROR_OP:
            DEBUG("SHIFTR");
            SET_INTVAL(SP, -2, rotate_right(GET_INTVAL(SP, -2), GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        default:
            Machine_Check(MC_ILLEGAL_INST,
                    "Illegal instruction '%i' at %d\n", val, cpu->IP);
            break;
    }
}
