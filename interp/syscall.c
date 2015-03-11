#include <stdio.h>
#include <stdlib.h>
#include "machine.h"
#include "syscall.h"

void syscall(Machine_State *cpu, int *args)
{
    switch (args[1])
    {
        case EXIT_CALL:
            printf("Exiting with status %d\n", args[2]);
            exit(args[2]);
            break;
        case PRINT_CALL:
            printf("%d\n", args[2]);
            break;
        case PRINTSA_CALL:
            printf("%s", &cpu->mem[args[2]]);
            break;
        case PRINTC_CALL:
            printf("%c", args[2]);
            break;
        case GETS_CALL:
            scanf("%s", &cpu->mem[args[2]]);
            break;
        case GETL_CALL:
            gets(&cpu->mem[args[2]]);
            break;
        case GETI_CALL:
            scanf("%d", (int *)&cpu->mem[args[2]]);
            break;
        case EXEC_CALL:
            Load(cpu, &cpu->mem[args[2]]);
            break;
        default:
            printf("Unknown system call: %d %d\n", args[0], args[1]);
            printf("Process terminating\n");
            exit(-1);
            break;
    }
}
