#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "loader.h"
#include "opcodes.h"

int main(int argc, char **argv)
{
    int result;
    Machine_State cpu;

    Init_Machine();

    //printf("stackl Beta %s %s\n", __DATE__, __TIME__);

    if (argc < 2) 
    {
        printf("Need to specify an executable file\n");
        return 1;
    } else {
        if (argc > 2 && strcmp(argv[2], "debug") == 0) Enable_Debug();
        if (argc > 3) Set_Max_Instr(atoi(argv[3]));


        Get_Machine_State(&cpu);

        result = Load(&cpu, argv[1]);
        if (result != 0) 
        {
            printf("Unable to execute %s\n", argv[1]);
            return 2;
        }

        Set_Machine_State(&cpu);

        Machine_Execute();
    }

    return 0;
}
