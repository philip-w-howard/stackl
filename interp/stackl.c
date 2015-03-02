#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "machine.h"
#include "opcodes.h"

int main(int argc, char **argv)
{
    int result;

    if (argc < 2) 
    {
        printf("Need to specify an executable file\n");
        return 1;
    } else {
        if (argc > 2 && strcmp(argv[2], "debug") == 0) Enable_Debug();
        if (argc > 3) Set_Max_Instr(atoi(argv[3]));

        result = Load_And_Go(argv[1]);
        if (result != 0) 
        {
            printf("Unable to execute %s\n", argv[1]);
            return 2;
        }
    }

    return 0;
}
