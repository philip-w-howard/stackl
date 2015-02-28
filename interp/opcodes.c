#include <stdio.h>
#include <stdarg.h>

#include "opcodes.h"
#include "machine.h"

// set DEBUG to "//" to turn off DEBUG
#define DEBUG(...) DebugPrint(cpu,__VA_ARGS__);
void DebugPrint(Machine_State *cpu, const char *fmt, ...)
{
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
    while (cpu->mem[cpu->IP] != HALT_OP)
    {
        switch (cpu->mem[cpu->IP])
        {
            case NOP:
                DEBUG("NOP");
                cpu->IP++;
                break;
            case PLUS_OP:
                DEBUG("PLUS");
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] + cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case MINUS_OP:
                DEBUG("MINUS");
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] - cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case TIMES_OP:
                DEBUG("TIMES");
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] * cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case DIVIDE_OP:
                DEBUG("DIVIDE");
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] / cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case MOD_OP:
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] % cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case EQ_OP:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] == cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case NE_OP:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] != cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case GT_OP:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] > cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case LT_OP:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] < cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case GE_OP:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] >= cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case LE_OP:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] <= cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case AND_OP:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] && cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case OR_OP:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] || cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case PRINT_OP:
                DEBUG("PRINT %d", cpu->mem[cpu->SP-1]);
                printf("%d\n", cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case PRINTS_OP:
                //printf("%s", (char *)cpu->mem[cpu->SP-1]);
                cpu->IP++;
                break;
            case DUP_OP:
                DEBUG("DUP");
                cpu->mem[cpu->SP] = cpu->mem[cpu->SP-1];
                cpu->SP++;
                cpu->IP++;
                break;
            case HALT_OP:
                DEBUG("HALT");
                break;
            case POP_OP:
                DEBUG("POP");
                cpu->SP--;
                break;
            case CALL_OP:
                DEBUG("CALL %d", cpu->mem[cpu->SP - 1]);
                temp = cpu->mem[cpu->SP - 1];
                cpu->mem[cpu->SP - 1] = cpu->IP+1;
                cpu->mem[cpu->SP] = cpu->FP;
                cpu->SP++;
                cpu->FP = cpu->SP;
                cpu->IP = temp;
                break;
            case RETURN_OP:
                DEBUG("RETURN");
                cpu->SP = cpu->FP - 2;
                cpu->IP = cpu->mem[cpu->FP -2];
                cpu->FP = cpu->mem[cpu->FP - 1];
                break;
            case PUSH_OP:
                DEBUG("PUSH %d", cpu->mem[cpu->IP+1]);
                cpu->IP++;
                cpu->mem[cpu->SP] = cpu->mem[cpu->IP];
                cpu->SP++;
                cpu->IP++;
                break;
            case JUMP_OP:
                cpu->IP++;
                break;
            case JUMPE_OP:
                cpu->IP++;
                break;
            case PUSHVAR_OP:
                temp = cpu->FP + cpu->mem[cpu->IP+1];
                DEBUG("PUSHVAR %d %d", cpu->mem[cpu->IP+1], cpu->mem[temp]);
                cpu->IP++;
                cpu->mem[cpu->SP] = cpu->mem[temp];
                cpu->SP++;
                cpu->IP++;
                break;
            case POPVAR_OP:
                DEBUG("POPVAR %d", cpu->mem[cpu->IP+1]);
                cpu->IP++;
                temp = cpu->FP + cpu->mem[cpu->IP];
                cpu->SP--;
                cpu->mem[temp] = cpu->mem[cpu->SP];
                cpu->IP++;
                break;
            case ADJSP_OP:
                DEBUG("ADJSP %d", cpu->mem[cpu->IP+1]);
                cpu->IP++;
                cpu->SP += cpu->mem[cpu->IP];
                cpu->IP++;
                break;
#define ADJSP_OP    -6
        }
    }
}

