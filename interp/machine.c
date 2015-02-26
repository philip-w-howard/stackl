#include <stdio.h>
#include "opcodes.h"

#define MEMORY_SIZE  10000
int Memory[MEMORY_SIZE];
Machine_State Regs;

int Load_And_Go(const char *filename)
{
    int byte = 0;
    int value;
    FILE *input = fopen(filename, "r");

    if (input == NULL) return 1;

    while (!feof(input))
    {
        fscanf(input, "%d", &value);
        Memory[byte++] = value;
    }

    Regs.mem = Memory;
    Regs.IP = 0;
    Regs.FP = byte;
    Regs.SP = byte;

    Execute(&Regs);

    return 0;
}
