#include <stdio.h>

#include "opcodes.h"
#include "machine.h"

void Execute(Machine_State *cpu)
{
    while (cpu->mem[cpu->IP] != HALT)
    {
        switch (cpu->mem[cpu->IP])
        {
            case NOP:
                cpu->IP++;
                break;
            case PLUS:
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] + cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case MINUS:
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] - cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case TIMES:
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] * cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case DIVIDE:
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] / cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case MOD:
                cpu->mem[cpu->SP-2] = cpu->mem[cpu->SP-2] % cpu->mem[cpu->SP-1];
                cpu->SP--;
                cpu->IP++;
                break;
            case EQUAL:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] == cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case NOT_EQUAL:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] != cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case GREATER:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] > cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case LESS:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] < cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case GREATER_EQ:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] >= cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case LESS_EQ:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] <= cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case LOG_AND:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] && cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case LOG_OR:
                cpu->mem[cpu->SP-2] = (cpu->mem[cpu->SP-2] || cpu->mem[cpu->SP-1]);
                cpu->SP--;
                cpu->IP++;
                break;
            case PRINT:
                printf("%d\n", cpu->mem[cpu->SP-1]);
                cpu->IP++;
                break;
            case PRINTS:
                //printf("%s", (char *)cpu->mem[cpu->SP-1]);
                cpu->IP++;
                break;
            case DUP:
                cpu->mem[cpu->SP] = cpu->mem[cpu->SP-1];
                cpu->SP++;
                cpu->IP++;
                break;
            case HALT:
                cpu->IP++;
                break;
            case POP:
                cpu->SP--;
                break;
            case PUSH:
                cpu->IP++;
                cpu->mem[cpu->SP] = cpu->mem[cpu->IP];
                cpu->SP++;
                cpu->IP++;
                break;
            case JUMP:
                cpu->IP++;
                break;
            case JUMPE:
                cpu->IP++;
                break;
        }
    }
}

