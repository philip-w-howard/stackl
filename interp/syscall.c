#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "machine.h"
#include "syscodes.h"
#include "syscall.h"
#include "loader.h"
#include "sched.h"
#include "io_handler.h"

#error "No longer used"

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
    int user_mode;
    int temp = 0;

    Machine_Check("obsolete interpreter");
    user_mode = Is_User_Mode();

    switch (args[1])
    {
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

    // What about forked process?
    Set_User_Mode(user_mode);

    return temp;
}
