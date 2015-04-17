#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"
#include "opcodefunc.h"
#include "machine.h"
#include "loader.h"

void MemCpy(int addr, char *sptr)
{
    while (*sptr)
    {
        Set_Byte(addr, *sptr);
        addr++;
        sptr++;
    }
}

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
//int Load(Machine_State *cpu, const char *filename, int base, int top)
int Load(const char *filename)
{
    Machine_State cpu;
    int base;
    int top;
    int loc;
    int value;
    char record_type[20];
    char string[256];
    char *sptr;
    int  slen;

    Get_Machine_State(&cpu);
    base = cpu.BP;
    top  = cpu.LP;

    int byte = base;
    int max_byte = byte;
    FILE *input = fopen(filename, "r");
    if (input == NULL) 
    {
        strcpy(string, filename);
        strcat(string, ".sl");
        input = fopen(string, "r");
        if (input == NULL) return 1;
    }

    fscanf(input, "%s", record_type);

    while (!feof(input) && record_type[0] != 'X')
    {
        switch (record_type[0])
        {
            case 'D':
                fscanf(input, "%d %d", &byte, &value);
                Set_Word(byte, value);
                byte += WORD_SIZE;
                max_byte = max_byte>byte ? max_byte : byte;
                if (max_byte > top)
                {
                    fprintf(stderr, "Memory overflow while loading\n");
                }
                break;
            case 'F':
                fscanf(input, "%d %d", &loc, &value);
                if (loc >= max_byte)
                {
                    fprintf(stderr, "File format error: fixup record precedes data: %d %d\n", loc, max_byte);
                }
                Set_Word(loc, value);
                break;
            case 'S':
                fscanf(input, "%d", &byte);
                fgets(string, sizeof(string), input);
                slen = strlen(string);
                if (string[slen - 1] != '\n')
                {
                    fprintf(stderr, "File format error: string longer than MAX string size\n");
                    exit(-1);
                }

                sptr = format_string(string);
                slen = strlen(sptr);

                MemCpy(byte, sptr);
                byte += ((slen + WORD_SIZE)/WORD_SIZE)*WORD_SIZE;
                max_byte = max_byte>byte ? max_byte : byte;
                if (max_byte > top)
                {
                    fprintf(stderr, "Memory overflow while loading\n");
                }

                break;
            case 'C':
                fgets(string, sizeof(string), input);
                break;

            default:
                fprintf(stderr, "File format error: Unrecognized record type\n");
                break;
        }
        fscanf(input, "%s", record_type);
    }

    cpu.IP = base + 2*WORD_SIZE;
    cpu.FP = max_byte;
    cpu.SP = max_byte;
    Set_Machine_State(&cpu);

    return 0;
}
