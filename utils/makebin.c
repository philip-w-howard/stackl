#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../version.h"

#define WORD_SIZE 4

static char Input_File[200] = "";
static int Memory_Size = 65536;
static char *Memory;

static const char *HELP_STR =
 "stackl [-help] [-version] [-loader] [-M<mem size>]\n";

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
int Load_Text(const char *filename)
{
    int loc;
    int value;
    char record_type[20];
    char string[256];
    char *sptr;
    int  slen;

    int addr = 0;
    int max_addr = addr;
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
                fscanf(input, "%d %d", &addr, &value);
                DEBUG(addr, "D: %d", value);
                *(int *)&Memory[addr] = value;
                addr += WORD_SIZE;
                max_addr = max_addr>addr ? max_addr : addr;
                if (max_addr > Memory_Size)
                {
                    fprintf(stderr, "Memory overflow while loading\n");
                    exit(-1);
                }
                break;
            case 'F':
                fscanf(input, "%d %d", &loc, &value);
                DEBUG(loc, "F: %d", value);
                if (loc >= max_addr)
                {
                    fprintf(stderr, "File format error: fixup record precedes data: %d %d\n", loc, max_addr);
                    exit(-1);
                }
                *(int *)&Memory[loc] = value;
                break;
            case 'G':
                fscanf(input, "%d %s %d", &addr, string, &slen);
                DEBUG(addr, "G: %s %d %d", string, addr, slen);
                while (slen > 0)
                {
                    *(int *)&Memory[addr] = value;
                    addr += WORD_SIZE;
                    slen -= WORD_SIZE;
                }

                max_addr = max_addr>addr ? max_addr : addr;
                if (max_addr > Memory_Size)
                {
                    fprintf(stderr, "Memory overflow while loading\n");
                    exit(-1);
                }
                break;
            case 'S':
                fscanf(input, "%d", &addr);
                fgets(string, sizeof(string), input);
                DEBUG(addr, "S: %s", input);
                slen = strlen(string);
                if (string[slen - 1] != '\n')
                {
                    fprintf(stderr, "File format error: string longer than MAX string size\n");
                    exit(-1);
                }

                sptr = format_string(string);
                slen = strlen(sptr);

                strcpy((char *)&Memory[addr], sptr);
                addr += ((slen + WORD_SIZE)/WORD_SIZE)*WORD_SIZE;
                max_addr = max_addr>addr ? max_addr : addr;
                if (max_addr > Memory_Size)
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

    return max_addr;
}

// Process command line args
void Process_Args(int argc, char **argv)
{
    int ii;
    for (ii=1; ii<argc; ii++)
    {
        if (argv[ii][0] == '-')
        {
            char *arg = &argv[ii][1];
            if (strcmp(arg, "help") == 0)
            {
                printf(HELP_STR);
                exit(0);
            }
            else if (strcmp(arg, "loader") == 0)
                Loader_Debug();
            else if (argv[ii][1] == 'M')
                Memory_Size = atoi(&argv[ii][2]);
            else if (strcmp(arg, "version") == 0)
            {
                printf("makebin %s %s %s\n", VERSION, __DATE__, __TIME__);
                exit(0);
            }
            else
            {
                printf("Unrecognized option %s\n", argv[ii]);
                exit(-1);
            }
        }
        else
        {
            // assume input file name
            strcpy(Input_File, argv[ii]);
        }
    }
}
int main(int argc, char **argv)
{
    int top;
    int status;

    Process_Args(argc, argv);

    if (strlen(Input_File) == 0)
    {
        printf("Need to specify an executable file\n");
        return 1;
    } 

    Memory = (char *)malloc(Memory_Size);
    if (Memory == NULL)
    {
        printf("Unable to allocate memory\n");
        exit(1);
    }

    top = Load_Text(Input_File);
    if (top == 0) 
    {
        printf("Unable to convert %s\n", Input_File);
        return 3;
    }

    char *extension = strstr(Input_File, ".sl");
    if (extension != NULL)
        strcpy(extension, ".slb");
    else
        strcat(Input_File, ".slb");

    void *start_addr = &Memory[0];

    FILE *bin_file = fopen(Input_File, "wb");

    if (bin_file == NULL)
    {
        printf("Unable to open output file\n");
        return 4;
    }

    status = fwrite(start_addr, top, 1, bin_file);
    if (status != 1)
    {
        printf("Unable to create output file\n");
        return 4;
    }

    fclose(bin_file);

    return 0;
}
