#include <stdio.h>
#include <stdlib.h>
#include "machine.h"
#include "syscall.h"
#include "loader.h"
#include "sched.h"

void syscall(Machine_State *cpu, int *args)
{
    switch (args[1])
    {
        case EXIT_CALL:
            printf("Exiting with status %d\n", args[2]);
            exit(args[2]);
            break;
        case PRINTI_CALL:
            printf("%d\n", args[2]);
            break;
        case PRINTS_CALL:
            printf("%s", (char *)Get_Addr(args[2]));
            break;
        case PRINTC_CALL:
            printf("%c", args[2]);
            break;
        case GETS_CALL:
            scanf("%s", (char *)Get_Addr(args[2]));
            break;
        case GETL_CALL:
            gets(Get_Addr(args[2]));
            break;
        case GETI_CALL:
            scanf("%d", (int *)Get_Addr(args[2]));
            break;
        case EXEC_CALL:
            // Need to load in current memory
            Load(cpu, Get_Addr(args[2]), 0, MEMORY_SIZE);
            break;
        default:
            printf("Unknown system call: %d %d\n", args[0], args[1]);
            printf("Process terminating\n");
            exit(-1);
            break;
    }
    Schedule();
}
