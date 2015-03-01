#include <stdio.h>
#include "opcodes.h"

#define MEMORY_SIZE  10000
int Memory[MEMORY_SIZE];
Machine_State Regs;

int Load_And_Go(const char *filename)
{
    int byte = 0;
    int loc;
    int value;
    char record_type[20];
    FILE *input = fopen(filename, "r");

    if (input == NULL) return 1;

    fscanf(input, "%s", record_type);

    while (!feof(input) && record_type[0] != 'X')
    {
        switch (record_type[0])
        {
            case 'D':
                fscanf(input, "%d", &value);
                Memory[byte++] = value;
                break;
            case 'F':
                fscanf(input, "%d %d", &loc, &value);
                if (loc >= byte)
                {
                    fprintf(stderr, "File format error: fixup record precedes data\n");
                }
                Memory[loc] = value;
                break;
            default:
                fprintf(stderr, "File format error: Unrecognized record type\n");
                break;
        }
        fscanf(input, "%s", record_type);
    }

    Regs.mem = Memory;
    Regs.IP = 0;
    Regs.FP = byte;
    Regs.SP = byte;

    Execute(&Regs);

    return 0;
}
