//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "loader.h"
#include "vmem.h"
#include "../version.h"

static char Input_File[200] = "";
static int Memory_Size = 0;

static const char *HELP_STR =
 "stackl [-help] [-version] [-loader]\n"
 "       [-M<mem size>]\n";

// STUBS for non-existent parts of interp
int IO_Get_Word(int address) { return 0; }
void IO_Set_Word(int address, int value) {}
int IO_Get_Byte(int address) { return 0; }
void IO_Set_Byte(int address, int value) {}
void Execute(Machine_State *machine) {}

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

    Init_Machine(Memory_Size);

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

    void *start_addr = Abs_Get_Addr(0);

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
