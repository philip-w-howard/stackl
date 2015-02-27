#include <stdio.h>

#include "opcodes.h"
#include "machine.h"

void Execute(Machine_State *cpu)
{
    int temp;
    while (cpu->mem[cpu->IP] != HALT_OP)
    {
        switch (cpu->mem[cpu->IP])
        {
            case NOP:
                cpu->IP++;
                break;
            case PLUS_OP:
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] + cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case MINUS_OP:
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] - cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case TIMES_OP:
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] * cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case DIVIDE_OP:
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
                printf("%d\n", cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case PRINTS_OP:
                //printf("%s", (char *)cpu->mem[cpu->SP-1]);
                cpu->IP++;
                break;
            case DUP_OP:
                cpu->mem[cpu->SP] = cpu->mem[cpu->SP-1];
                cpu->SP++;
                cpu->IP++;
                break;
            case HALT_OP:
                break;
            case POP_OP:
                cpu->SP--;
                break;
            case CALL_OP:
                temp = cpu->mem[cpu->SP - 1];
                cpu->mem[cpu->SP - 1] = cpu->IP+1;
                cpu->mem[cpu->SP] = cpu->FP;
                cpu->SP++;
                cpu->FP = cpu->SP;
                cpu->IP = temp;
                break;
            case RETURN_OP:
                cpu->SP = cpu->FP - 2;
                cpu->IP = cpu->mem[cpu->FP -2];
                cpu->FP = cpu->mem[cpu->FP - 1];
                break;
            case PUSH_OP:
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
        }
    }
}

