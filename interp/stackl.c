#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "opcodes.h"
#include "loader.h"
#include "io_handler.h"
#include "dma_term.h"
#include "pio_term.h"
#include "disk.h"
#include "../version.h"

static char Input_File[200] = "";
static int Memory_Size = 0;

static const char *HELP_STR =
 "stackl [-opcodes] [-help] [-version] [-loader]\n"
 "       [-M<mem size>] [-N<num instr>] [-T<timer interval>]\n";

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
            else if (argv[ii][1] == 'N')
                Set_Max_Instr(atoi(&argv[ii][2]));
            else if (strcmp(arg, "opcodes") == 0)
                Opcodes_Debug();
            else if (argv[ii][1] == 'T')
                Set_Timer_Interval(atoi(&argv[ii][2]));
            else if (strcmp(arg, "version") == 0)
            {
                printf("stackl %s %s %s\n", VERSION, __DATE__, __TIME__);
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
    int result;

    Process_Args(argc, argv);

    if (strlen(Input_File) == 0)
    {
        printf("Need to specify an executable file\n");
        return 1;
    } 

    Init_Machine(Memory_Size);
    Init_IO();
    PIO_T_Init();
    DMA_T_Init();
    Disk_Init();
    //Sched_Init();

    /*
    result = Sched_Fork();
    if (result == -1)
    {
        printf("Unable to fork initial process\n");
        return 2;
    }
    */

    result = Boot(Input_File);
    if (result != 0) 
    {
        printf("Unable to execute %s\n", Input_File);
        return 3;
    }

    //Schedule();

    Machine_Execute();

    Disk_Finish();
    DMA_T_Finish();
    PIO_T_Finish();
    Finish_IO();

    return 0;
}
