#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_def.h"
#include "opcodes.h"
#include "machine.h"
#include "loader.h"

static int Do_Debug = 0;

void Loader_Debug()
{
    Do_Debug = 1;
}

// set DEBUG to "//" to turn off DEBUG
#define DEBUG(...) debug_print(__VA_ARGS__);
static void debug_print(int location, const char *fmt, ...)
{
    if (!Do_Debug) return;

    va_list args;
    va_start(args, fmt);

    char format[200];
    sprintf(format, "Loading at %d: %s\n", location, fmt);
    vfprintf(stderr, format, args);
    va_end(args);
}

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
            // can't user strcat because dest overlaps with src
            //strcat(str, sptr);
            memmove(str+strlen(str), sptr, strlen(sptr)+1);
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
        if (input == NULL) return 0;
    }

    fscanf(input, "%s", record_type);

    while (!feof(input) && record_type[0] != 'X')
    {
        switch (record_type[0])
        {
            case 'D':
                fscanf(input, "%d %d", &byte, &value);
                byte += base;
                DEBUG(byte, "D: %d", value);
                Set_Word(byte, value);
                byte += WORD_SIZE;
                max_byte = max_byte>byte ? max_byte : byte;
                if (max_byte > top)
                {
                    fprintf(stderr, "Memory overflow while loading\n");
                    exit(-1);
                }
                break;
            case 'F':
                fscanf(input, "%d %d", &loc, &value);
                loc += base;
                DEBUG(loc, "F: %d", value);
                if (loc >= max_byte)
                {
                    fprintf(stderr, "File format error: fixup record precedes data: %d %d\n", loc, max_byte);
                    exit(-1);
                }
                Set_Word(loc, value);
                break;
            case 'S':
                fscanf(input, "%d", &byte);
                byte += base;
                fgets(string, sizeof(string), input);
                DEBUG(byte, "S: %s", input);
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
                    exit(-1);
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

    return max_byte;
}

int Boot(const char *filename)
{
    int top;
    Machine_State cpu;

    Get_Machine_State(&cpu);
    cpu.SP = cpu.LP - 8;
    cpu.FP = cpu.SP;
    Set_Machine_State(&cpu);

    top = Load(filename);
    if (top == 0) return 1;

    cpu.SP = top + WORD_SIZE;
    cpu.FP = cpu.SP;
    cpu.IP = cpu.BP + 2*WORD_SIZE;
    Set_Machine_State(&cpu);

    return 0;
}
