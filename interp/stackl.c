#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sched.h"
#include "opcodefunc.h"
#include "syscall.h"
#include "io_handler.h"

char Input_File[200] = "";

void Process_Args(int argc, char **argv)
{
    int ii;
    for (ii=1; ii<argc; ii++)
    {
        if (argv[ii][0] == '-')
        {
            char *arg = &argv[ii][1];
            if (strcmp(arg, "opcodes") == 0)
                Opcodes_Debug();
            else if (strcmp(arg, "sched") == 0)
                Sched_Debug();
            else if (strcmp(arg, "syscall") == 0)
                Syscall_Debug();
            else if (argv[ii][1] == 'N')
                Set_Max_Instr(atoi(&argv[ii][2]));
            else if (argv[ii][1] == 'T')
                Set_Timer_Interval(atoi(&argv[ii][2]));
            else if (strcmp(arg, "version") == 0)
            {
                printf("stackl Beta %s %s\n", __DATE__, __TIME__);
                exit(0);
            }
            else if (strcmp(arg, "help") == 0)
            {
                printf("stackl -version -help -opcodes -sched -syscall <file>\n");
                exit(0);
            }
            else
                printf("Unrecognized option %s\n", argv[ii]);
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

    Init_IO();
    Init_Machine();
    Sched_Init();

    result = Sched_Fork();
    if (result == -1)
    {
        printf("Unable to fork initial process\n");
        return 2;
    }

    result = Sched_Load(Input_File);
    if (result != 0) 
    {
        printf("Unable to execute %s\n", Input_File);
        return 3;
    }

    Schedule();

    Machine_Execute();

    Finish_IO();

    return 0;
}
