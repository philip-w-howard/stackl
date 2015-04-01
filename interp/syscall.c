#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "machine.h"
#include "syscodes.h"
#include "syscall.h"
#include "loader.h"
#include "sched.h"
#include "io_handler.h"

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
int syscall(int *args)
{
    int temp = 0;
    switch (args[1])
    {
        case EXIT_CALL:
            DEBUG("EXIT: %d\n", args[2]);
            Sched_Exit(args[2]);
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
            Sched_WaitIO(GETS_CALL, Get_Addr(args[2]));
            //scanf("%s", (char *)Get_Addr(args[2]));
            break;
        case GETL_CALL:
            DEBUG("GETL:\n");
            Sched_WaitIO(GETL_CALL, Get_Addr(args[2]));
            //gets(Get_Addr(args[2]));
            break;
        case GETI_CALL:
            DEBUG("GETI:\n");
            Sched_WaitIO(GETI_CALL, Get_Addr(args[2]));
            //scanf("%d", (int *)Get_Addr(args[2]));
            break;
        case EXEC_CALL:
            DEBUG("EXEC: %s\n", Get_Addr(args[2]));
            temp = Sched_Load(Get_Addr(args[2]));
            break;
        case FORK_CALL:
            DEBUG("FORK:\n");
            temp = Sched_Fork();
            break;
        case WAIT_CALL:
            DEBUG("WAIT: %d\n", args[2]);
            Sched_WaitProc(args[2]);
            break;
        default:
            printf("Unknown system call: %d %d\n", args[0], args[1]);
            printf("Process terminating\n");
            exit(-1);
            break;
    }

    // do NOT schedule on fork. 
    // The is necessary to get the two processes to return the right values
    if (args[1] != FORK_CALL) Schedule();
    DEBUG("Syscall returning: %d\n", temp);
    return temp;
}
