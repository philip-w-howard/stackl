#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "opcodes.h"
#include "loader.h"
#include "io_handler.h"
#include "dma_term.h"
#include "pio_term_int.h"
#include "disk.h"
#include "timer.h"
#include "../version.h"

// Use PATH_MAX from limits.h?
//
static char Input_File[256] = "";
static int Memory_Size = 0;
static int Use_Disk = 0;        // 1
static int Boot_Disk = 0;       // 1
static int Use_PIO_Term = 0;    // 1
static int Use_DMA_Term = 0;
static int Use_Inp_Instr = 0;

static const char *HELP_STR =
 "stackl [-boot] [-dma_term] [-inp] [-[no]pio_term] [-nodisk]\n"
 "       [-opcodes] [-help] [-version] [-loader]\n"
 "       [-M<mem size>] [-N<num instr>]\n";

void Process_Args(int argc, char **argv)
{
    int ii;
    for (ii=1; ii<argc; ii++)
    {
        if (argv[ii][0] == '-')
        {
            char *arg = &argv[ii][1];
            if (strcmp(arg, "boot") == 0)
            {
                Use_Disk = 1;
                Boot_Disk = 1;
            }
            else if (strcmp(arg, "dma_term") == 0)
            {
                Use_PIO_Term = 0;
                Use_DMA_Term = 1;
            }
            else if (strcmp(arg, "help") == 0)
            {
                printf(HELP_STR);
                exit(0);
            }
            else if (strcmp(arg, "inp") == 0)
                Use_Inp_Instr = 1;
            else if (strcmp(arg, "loader") == 0)
                Loader_Debug();
            else if (argv[ii][1] == 'M')
                Memory_Size = atoi(&argv[ii][2]);
            else if (strcmp(arg, "nodisk") == 0)
                Use_Disk = 0;
            else if (strcmp(arg, "nopio_term") == 0)
                Use_PIO_Term = 0;
            else if (argv[ii][1] == 'N')
                Set_Max_Instr(atoi(&argv[ii][2]));
            else if (strcmp(arg, "opcodes") == 0)
                Opcodes_Debug();
            else if (strcmp(arg, "pio_term") == 0)
            {
                Use_PIO_Term = 1;
                Use_DMA_Term = 0;
            }
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

            // If we're given a file to run, we won't boot from the disk
            Boot_Disk = 0;
        }
    }
}
int main(int argc, char **argv)
{

    int result;

    Process_Args(argc, argv);

    Init_Machine(Memory_Size);
    Init_IO(Use_Inp_Instr);
    Timer_Init();

    if (Use_PIO_Term) 
    {
        PIO_T_Init();
    }

    if (Use_DMA_Term) 
    {
        DMA_T_Init();
    }

    if (Use_Disk) 
    {
        Disk_Init();
    }

    if (Boot_Disk && Use_Disk)
    {
        Boot_From_Disk();
    }
    else if (strlen(Input_File) == 0)
    {
        printf("Need to specify an executable file\n");
        return 1;
    } 
    else
    {
        result = Boot(Input_File);
        if (result != 0) 
        {
            printf("Unable to execute %s\n", Input_File);
            return 3;
        }
    }

    Machine_Execute();

    return 0;
}
