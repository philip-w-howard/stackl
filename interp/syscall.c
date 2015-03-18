#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "machine.h"
#include "syscodes.h"
#include "syscall.h"
#include "loader.h"
#include "sched.h"

static int Syscall_Do_Debug = 0;

void Syscall_Debug()
{
    Syscall_Do_Debug = 1;
}

//***************************************
// set DEBUG to "//" to turn off DEBUG
#define DEBUG(fmt, ...) debug_print(fmt,## __VA_ARGS__);
static void debug_print(const char *fmt, ...)
{
    if (!Syscall_Do_Debug) return;

    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    va_end(args);
}
//***************************************
int syscall(Machine_State *cpu, int *args)
{
    int temp = 0;
    switch (args[1])
    {
        case EXIT_CALL:
            DEBUG("EXIT: %d\n", args[2]);
            exit(args[2]);
            break;
        case PRINTI_CALL:
            DEBUG("PRINTI: %d\n", args[2]);
            printf("%d\n", args[2]);
            fflush(stdout);
            break;
        case PRINTS_CALL:
            DEBUG("PRINTS: %s\n", (char *)Get_Addr(args[2]));
            printf("%s", (char *)Get_Addr(args[2]));
            fflush(stdout);
            break;
        case PRINTC_CALL:
            DEBUG("PRINTC: %c\n", args[2]);
            printf("%c", args[2]);
            fflush(stdout);
            break;
        case GETS_CALL:
            DEBUG("GETS:\n");
            scanf("%s", (char *)Get_Addr(args[2]));
            break;
        case GETL_CALL:
            DEBUG("GETL:\n");
            gets(Get_Addr(args[2]));
            break;
        case GETI_CALL:
            DEBUG("GETI:\n");
            scanf("%d", (int *)Get_Addr(args[2]));
            break;
        case EXEC_CALL:
            DEBUG("EXEC: %s\n", Get_Addr(args[2]));
            temp = Sched_Load(Get_Addr(args[2]));
            break;
        case FORK_CALL:
            DEBUG("FORK:\n");
            temp = Fork();
            break;
        case WAIT_CALL:
            DEBUG("WAIT: %d\n", args[2]);
            Wait(args[2]);
            break;
        default:
            printf("Unknown system call: %d %d\n", args[0], args[1]);
            printf("Process terminating\n");
            exit(-1);
            break;
    }
    Schedule();
    DEBUG("Syscall returning: %d\n", temp);
    return temp;
}
