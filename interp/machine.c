#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"

#define MEMORY_SIZE  10000
int Memory[MEMORY_SIZE];
Machine_State Regs;

char *format_string(char *str)
{
    char *sptr;

    sptr = strrchr(str, '"');
    if (sptr == NULL)
    {
        fprintf(stderr, "File format error: invalid string\n");
        return str;
    }

    // eliminate trailing "
    *sptr = 0;

    sptr = strchr(str, '"');
    if (sptr == NULL)
    {
        fprintf(stderr, "File format error: invalid string\n");
        return str;
    }

    // eliminate leading "
    sptr++;

    // Handle escape sequences
    str = sptr;
    sptr = strchr(str, '\\');
    while (sptr != NULL)
    {
        if (sptr[1] == 'n')
        {
            *sptr = '\n';
            sptr++;
            *sptr = 0;
            sptr++;
            strcat(str, sptr);
        } else {
            fprintf(stderr, "Unrecognized escape sequence in string\n");
            sptr += 2;
        }
        sptr = strchr(sptr, '\\');
    }

    return str;
}
int Load_And_Go(const char *filename)
{
    int byte = 0;
    int heap_top = MEMORY_SIZE;
    int loc;
    int value;
    char record_type[20];
    char string[256];
    char *sptr;
    int  slen;
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
            case 'S':
                fgets(string, sizeof(string), input);
                slen = strlen(string);
                if (string[slen - 1] != '\n')
                {
                    fprintf(stderr, "File format error: string longer than MAX string size\n");
                    exit(-1);
                }

                sptr = format_string(string);
                slen = strlen(sptr);

                // +4 instead of +3 to accomodate the null byte
                heap_top -= ((slen+4)/4 + 1);

                strcpy((char *)&Memory[heap_top], sptr);
                Memory[byte] = heap_top;
                byte++;
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
