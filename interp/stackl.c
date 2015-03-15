#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sched.h"
#include "opcodes.h"

char Input_File[200] = "";

void Process_Args(int argc, char **argv)
{
    int ii;
    for (ii=1; ii<argc; ii++)
    {
        if (argv[ii][0] == '-')
        {
            char *arg = &argv[ii][1];
            if (strcmp(arg, "debug") == 0)
                Enable_Debug();
            else if (argv[ii][1] == 'N')
                Set_Max_Instr(atoi(&argv[ii][2]));
            else if (strcmp(arg, "version") == 0)
                printf("stackl Beta %s %s\n", __DATE__, __TIME__);
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

    Init_Machine();
    Sched_Init();

    result = Create(argv[1]);
    if (result != 0) 
    {
        printf("Unable to execute %s\n", argv[1]);
        return 2;
    }

    Schedule();

    Machine_Execute();

    return 0;
}
