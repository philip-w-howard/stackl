#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"

#define MEMORY_SIZE  10000
#define WORD_SIZE 4
char Memory[MEMORY_SIZE];
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
//***************************************
int Load(Machine_State *cpu, const char *filename)
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
                *(int *)(&Memory[byte]) = value;
                byte += WORD_SIZE;
                break;
            case 'F':
                fscanf(input, "%d %d", &loc, &value);
                if (loc >= byte)
                {
                    fprintf(stderr, "File format error: fixup record precedes data\n");
                }
                *(int *)(&Memory[loc]) = value;
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
                heap_top -= ((slen+4)/4)*WORD_SIZE;

                strcpy((char *)&Memory[heap_top], sptr);
                *(int *)(&Memory[byte]) = heap_top;
                byte += WORD_SIZE;
                break;
            default:
                fprintf(stderr, "File format error: Unrecognized record type\n");
                break;
        }
        fscanf(input, "%s", record_type);
    }

    cpu->mem = Memory;
    cpu->IP = 0;
    cpu->FP = byte;
    cpu->SP = byte;

    return 0;
}
int Load_And_Go(const char *filename)
{
    Load(&Regs, filename);
    Execute(&Regs);

    return 0;
}
