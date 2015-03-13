#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "syscall.h"
#include "opcodes.h"
#include "machine.h"

#define WORD_SIZE 4
#define GET_INTVAL(reg, off) (Get_Word(cpu-> reg + OFFSET(off)))
#define SET_INTVAL(reg, off, val) (Set_Word(cpu-> reg + OFFSET(off), val))
#define INC(reg, amount)    cpu-> reg += amount*WORD_SIZE;
#define OFFSET(v)           ((v)*WORD_SIZE)

int Do_Debug = 0;
int Max_Instructions = INT_MAX;

void Enable_Debug()
{
    Do_Debug = 1;
}

void Set_Max_Instr(int max)
{
    Max_Instructions = max;
}

// set DEBUG to "//" to turn off DEBUG
#define DEBUG(...) DebugPrint(cpu,__VA_ARGS__);
void DebugPrint(Machine_State *cpu, const char *fmt, ...)
{
    if (!Do_Debug) return;

    va_list args;
    va_start(args, fmt);

    char format[200];
    sprintf(format, "%d %d %d %s\n", cpu->IP, cpu->SP, cpu->FP, fmt);
    vfprintf(stderr, format, args);
    va_end(args);
}

void Execute(Machine_State *cpu)
{
    int temp;
    static int num_instructions = 0;

    if (++num_instructions >= Max_Instructions) 
    {
        cpu->halted = 1;
        return;
    }

    switch (GET_INTVAL(IP, 0))
    {
        case NOP:
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
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) / GET_INTVAL(SP, -1)));
            INC(SP, -1);
            INC(IP, 1);
            break;
        case MOD_OP:
            DEBUG("MOD");
            SET_INTVAL(SP, -2, (GET_INTVAL(SP, -2) % GET_INTVAL(SP, -1)));
            INC(SP, -1);
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
        case HALT_OP:
            DEBUG("HALT");
            cpu->halted = 1;
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
        case TRAP_OP:
            DEBUG("TRAP %d %d", GET_INTVAL(FP,0), GET_INTVAL(FP,1));
            {
                int args[20];
                int ii;
                args[0] = GET_INTVAL(FP, -3);
                for (ii=1; ii<args[0]; ii++)
                {
                    args[ii] = GET_INTVAL(FP, -ii-3);
                }
                INC(SP, -args[0]);
                INC(IP, 1);
                syscall(cpu, args);
            }
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
            temp = GET_INTVAL(SP, -1);
            DEBUG("PUSHCVARIND %d %d", GET_INTVAL(SP, -1), Get_Byte(temp));
            SET_INTVAL(SP,-1, Get_Byte(temp));
            INC(IP, 1);
            break;
        case POPCVARIND_OP:
            DEBUG("POPCVARIND %d %d", GET_INTVAL(SP, -2), GET_INTVAL(SP, -1));
            temp = GET_INTVAL(SP, -1);
            Set_Byte(temp, GET_INTVAL(SP, -2));
            INC(SP, -2);
            INC(IP, 1);
            break;
        case PUSHVAR_OP:
            temp = cpu->FP + GET_INTVAL(IP, 1);
            DEBUG("PUSHVAR %d %d", GET_INTVAL(IP, 1), Get_Word(temp));
            INC(IP, 1);
            SET_INTVAL(SP,0, Get_Word(temp));
            INC(SP, 1);
            INC(IP, 1);
            break;
        case POPVAR_OP:
            DEBUG("POPVAR %d", GET_INTVAL(IP,1));
            INC(IP, 1);
            temp = cpu->FP + GET_INTVAL(IP, 0);
            INC(SP, -1);
            Set_Word(temp, GET_INTVAL(SP,0));
            INC(IP,1);
            break;
        case PUSHCVAR_OP:
            temp = cpu->FP + GET_INTVAL(IP, 1);
            DEBUG("PUSHCVAR %d %d", GET_INTVAL(IP, 1), Get_Byte(temp));
            Set_Byte(cpu->SP, Get_Byte(temp));
            INC(SP, 1);
            INC(IP, 2);
            break;
        case POPCVAR_OP:
            DEBUG("POPVVAR %d", GET_INTVAL(IP,1));
            temp = cpu->FP + GET_INTVAL(IP, 1);
            INC(SP, -1);
            Set_Byte(temp, Get_Byte(cpu->SP));
            INC(IP,2);
            break;
        case ADJSP_OP:
            DEBUG("ADJSP %d", GET_INTVAL(IP,1));
            INC(IP,1);
            cpu->SP += GET_INTVAL(IP,0);
            INC(IP,1);
            break;
        default:
            fprintf(stderr, "Illegal instruction at %d\n", cpu->IP);
            exit(-1);
            break;
    }
}

