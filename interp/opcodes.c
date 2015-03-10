#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "system.h"
#include "opcodes.h"
#include "machine.h"

#define WORD_SIZE 4
#define INTVAL(reg, offset) (*(int *)&(cpu->mem[cpu-> reg + OFFSET(offset)]))
#define INTABS(loc)         (*(int *)&(cpu->mem[(loc)]))
#define INC(reg, amount)    cpu-> reg += amount*WORD_SIZE;
#define OFFSET(v)           ((v)*WORD_SIZE)

/*
#define INTVAL(reg, offset) (cpu->mem[cpu-> reg + (offset)])
#define INTABS(loc)         (cpu->mem[(loc)])
#define INC(reg, amount)    cpu-> reg += amount
#define OFFSET(v)           (v)
*/

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
    int num_instructions = 0;
    while (INTVAL(IP,0) != HALT_OP)
    {
        if (++num_instructions >= Max_Instructions) break;

        switch (INTVAL(IP, 0))
        {
            case NOP:
                DEBUG("NOP");
                INC(IP, 1);
                break;
            case PLUS_OP:
                DEBUG("PLUS");
                INTVAL(SP, -2) = INTVAL(SP, -2) + INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case MINUS_OP:
                DEBUG("MINUS");

                INTVAL(SP, -2) = INTVAL(SP, -2) - INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case TIMES_OP:
                DEBUG("TIMES");
                INTVAL(SP, -2) = INTVAL(SP, -2) * INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case DIVIDE_OP:
                DEBUG("DIVIDE");
                INTVAL(SP, -2) = INTVAL(SP, -2) / INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case MOD_OP:
                DEBUG("MOD");
                INTVAL(SP, -2) = INTVAL(SP, -2) % INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case NEG_OP:
                DEBUG("NEG");
                INTVAL(SP, -1) = - INTVAL(SP, -1);
                INC(IP, 1);
                break;
            case EQ_OP:
                DEBUG("EQ");
                INTVAL(SP, -2) = (INTVAL(SP, -2) == INTVAL(SP, -1));
                INC(SP, -1);
                INC(IP, 1);
                break;
            case NE_OP:
                DEBUG("NE");
                INTVAL(SP, -2) = INTVAL(SP, -2) != INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case GT_OP:
                DEBUG("GT");
                INTVAL(SP, -2) = INTVAL(SP, -2) > INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case LT_OP:
                DEBUG("LT");
                INTVAL(SP, -2) = INTVAL(SP, -2) < INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case GE_OP:
                DEBUG("GE");
                INTVAL(SP, -2) = INTVAL(SP, -2) >= INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case LE_OP:
                DEBUG("LE");
                INTVAL(SP, -2) = INTVAL(SP, -2) <= INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case AND_OP:
                DEBUG("AND");
                INTVAL(SP, -2) = INTVAL(SP, -2) && INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case OR_OP:
                DEBUG("OR");
                INTVAL(SP, -2) = INTVAL(SP, -2) || INTVAL(SP, -1);
                INC(SP, -1);
                INC(IP, 1);
                break;
            case PRINT_OP:
                DEBUG("PRINT %d", INTVAL(SP, -1));
                printf("%d\n", INTVAL(SP, -1));
                INC(SP, -1);
                INC(IP, 1);
                break;
    /*
            case PRINTS_OP:
                temp = cpu->FP + INTVAL(SP, -1);
                DEBUG("PRINTS %s", (char *)(&cpu->mem[temp]));
                printf("%s", (char *)(&cpu->mem[temp]));
                INC(SP, -1);
                INC(IP, 1);
                break;
    */
            case PRINTSA_OP:
                temp = INTVAL(SP, -1);
                DEBUG("PRINTSA %s", (char *)(&cpu->mem[temp]));
                printf("%s", (char *)(&cpu->mem[temp]));
                INC(SP, -1);
                INC(IP, 1);
                break;
            case PRINTC_OP:
                DEBUG("PRINTC %c", INTVAL(SP, -1));
                printf("%c", INTVAL(SP, -1));
                INC(SP, -1);
                INC(IP, 1);
                break;
            case DUP_OP:
                DEBUG("DUP");
                INTVAL(SP, 0) = INTVAL(SP, -1);
                INC(SP, 1);
                INC(IP, 1);
                break;
            case HALT_OP:
                DEBUG("HALT");
                break;
            case POP_OP:
                DEBUG("POP");
                INC(SP, -1);
                INC(IP, 1);
                break;
            case CALL_OP:
                DEBUG("CALL %d", INTVAL(IP, 1));
                temp = INTVAL(IP, 1);
                INTVAL(SP, 0) = cpu->IP + OFFSET(2);
                INTVAL(SP, 1) = cpu->FP;
                INC(SP, 2);
                cpu->FP = cpu->SP;
                cpu->IP = temp;
                break;
            case RETURN_OP:
                DEBUG("RETURN");
                cpu->SP = cpu->FP - OFFSET(2);
                cpu->IP = INTVAL(FP, -2);
                cpu->FP = INTVAL(FP, -1);
                break;
            case RETURNV_OP:
                DEBUG("RETURNV");
                temp = INTVAL(SP, -1);
                cpu->SP = cpu->FP - OFFSET(1);
                cpu->IP = INTVAL(FP, -2);
                cpu->FP = INTVAL(FP, -1);
                INTVAL(SP, -1) = temp;
                break;
            case PUSH_OP:
                DEBUG("PUSH %d", INTVAL(IP, 1));
                INC(IP, 1);
                INTVAL(SP, 0) = INTVAL(IP, 0);
                INC(SP, 1);
                INC(IP, 1);
                break;
            case PUSHFP_OP:
                DEBUG("PUSHFP %d", cpu->FP);
                INTVAL(SP, 0) = cpu->FP;
                INC(SP, 1);
                INC(IP, 1);
                break;
            case TRAP_OP:
                DEBUG("TRAP %d %d", INTVAL(FP,0), INTVAL(FP,1));
                {
                    int args[20];
                    int ii;
                    args[0] = INTVAL(FP, -3);
                    for (ii=1; ii<args[0]; ii++)
                    {
                        args[ii] = INTVAL(FP, -ii-3);
                    }
                    INC(SP, -args[0]);
                    INC(IP, 1);
                    syscall(cpu, args);
                }
                break;
            case JUMP_OP:
                DEBUG("JUMP %d", INTVAL(IP, 1));
                cpu->IP = INTVAL(IP, 1);
                break;
            case JUMPE_OP:
                DEBUG("JUMPE %d %d", INTVAL(SP, -1), INTVAL(IP, 1));
                INC(IP, 1);
                INC(SP, -1);
                if (!INTVAL(SP, 0))
                    cpu->IP = INTVAL(IP,0);
                else
                    INC(IP, 1);
                break;
            case PUSHCVARIND_OP:
                temp = INTVAL(SP, -1);
                DEBUG("PUSHCVARIND %d %d", INTVAL(SP, -1), cpu->mem[temp]);
                INTVAL(SP,-1) = cpu->mem[temp];
                //cpu->mem[cpu->SP-OFFSET(1)] = cpu->mem[temp];
                INC(IP, 1);
                break;
            case POPCVARIND_OP:
                DEBUG("POPCVARIND %d %d", INTVAL(SP, -2), INTVAL(SP, -1));
                temp = INTVAL(SP, -1);
                cpu->mem[temp] = INTVAL(SP, -2);
                INC(SP, -2);
                INC(IP, 1);
                break;
            case PUSHVAR_OP:
                temp = cpu->FP + INTVAL(IP, 1);
                DEBUG("PUSHVAR %d %d", INTVAL(IP, 1), INTABS(temp));
                INC(IP, 1);
                INTVAL(SP,0) = INTABS(temp);
                INC(SP, 1);
                INC(IP, 1);
                break;
            case POPVAR_OP:
                DEBUG("POPVAR %d", INTVAL(IP,1));
                INC(IP, 1);
                temp = cpu->FP + INTVAL(IP, 0);
                INC(SP, -1);
                INTABS(temp) = INTVAL(SP,0);
                INC(IP,1);
                break;
            case PUSHCVAR_OP:
                temp = cpu->FP + INTVAL(IP, 1);
                DEBUG("PUSHCVAR %d %d", INTVAL(IP, 1), cpu->mem[temp]);
                cpu->mem[cpu->SP] = cpu->mem[temp];
                INC(SP, 1);
                INC(IP, 2);
                break;
            case POPCVAR_OP:
                DEBUG("POPVVAR %d", INTVAL(IP,1));
                temp = cpu->FP + INTVAL(IP, 1);
                INC(SP, -1);
                cpu->mem[temp] = cpu->mem[cpu->SP];
                INC(IP,2);
                break;
            case ADJSP_OP:
                DEBUG("ADJSP %d", INTVAL(IP,1));
                INC(IP,1);
                cpu->SP += INTVAL(IP,0);
                INC(IP,1);
                break;
            case GETS_OP:
                DEBUG("GETS %d", INTVAL(SP,-1));
                scanf("%s", &cpu->mem[INTVAL(SP,-1)]);
                INC(SP, -1);
                INC(IP,1);
                break;
            case GETL_OP:
                DEBUG("GETL %d", INTVAL(SP,-1));
                gets(&cpu->mem[INTVAL(SP,-1)]);
                INC(SP, -1);
                INC(IP,1);
                break;
            case GETI_OP:
                DEBUG("GETI %d", INTVAL(SP,-1));
                scanf("%d", (int *)&cpu->mem[INTVAL(SP,-1)]);
                INC(SP, -1);
                INC(IP,1);
                break;
            default:
                printf("Illegal instruction at %d\n", cpu->IP);
                exit(-1);
                break;
        }
    }
}

