#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "machine.h"
#include "loader.h"
#include "sched.h"

#define NUM_PROCESSES 2

#define EMPTY       0
#define RUNNING     1
#define RUNABLE     2
#define WAITING     3

typedef struct
{
    int process_id;
    int base;
    int top;
    Machine_State cpu;
    int state;
    int waiting_for;
} Process_State_t;

static Process_State_t Process_State[NUM_PROCESSES];
static int Current_Process;

static int Sched_Do_Debug = 0;

//***************************************
void Sched_Debug()
{
    Sched_Do_Debug = 1;
}

//***************************************
// set DEBUG to "//" to turn off DEBUG
#define DEBUG(fmt, ...) debug_print(fmt,## __VA_ARGS__);
static void debug_print(const char *fmt, ...)
{
    if (!Sched_Do_Debug) return;

    va_list args;
    va_start(args, fmt);

    char format[200];

    sprintf(format, "Sched: %d: %s\n", Current_Process, fmt);

    vfprintf(stderr, format, args);
    va_end(args);
}
//*************************************
static void duplicate_memory(int to_proc, int from_proc)
{
    int from;
    int to;
    from = Process_State[from_proc].base;
    to = Process_State[to_proc].base;

    while (from < Process_State[from_proc].top &&
           to   < Process_State[to_proc].top)
    {
        Set_Word(to, Get_Word(from));
        to += WORD_SIZE;
        from += WORD_SIZE;
    }
}
//*************************************
void Sched_Init()
{
    int ii;

    Current_Process = 0;

    for (ii=0; ii<NUM_PROCESSES; ii++)
    {
        Process_State[ii].process_id = ii+1;
        Process_State[ii].base = ii * MEMORY_SIZE/NUM_PROCESSES;
        Process_State[ii].top = (ii+1)*MEMORY_SIZE/NUM_PROCESSES - WORD_SIZE;
        Get_Machine_State(&Process_State[ii].cpu);
        Process_State[ii].state = EMPTY;
    }
}
//*************************************
void Schedule()
{
    int ii;

    DEBUG("Schedule");
    Get_Machine_State(&Process_State[Current_Process].cpu);
    if (Process_State[Current_Process].state == RUNNING)
    {
        Process_State[Current_Process].state = RUNABLE;
    }

    Current_Process++;
    for (ii=0; ii<NUM_PROCESSES; ii++)
    {
        if (Current_Process >= NUM_PROCESSES) Current_Process = 0;
        if (Process_State[Current_Process].state == RUNABLE)
        {
            Process_State[Current_Process].state = RUNNING;
            Set_Machine_State( &Process_State[Current_Process].cpu );
            DEBUG("switch to %d", Current_Process);
            return;
        }
        Current_Process++;
    }

    fprintf(stderr, "Machine halting: there are no runable processes\n");
    exit(-1);
}
//*************************************
void Exit(int status)
{
    int ii;

    DEBUG("stopping %d", Current_Process);
    Get_Machine_State(&Process_State[Current_Process].cpu);
    Process_State[Current_Process].state = EMPTY;

    // if somebody was waiting for us, set them to running and run them
    for (ii=0; ii<NUM_PROCESSES; ii++)
    {
        if (Process_State[ii].state == WAITING &&
            Process_State[ii].waiting_for == Current_Process)
        {
            Process_State[ii].state = RUNNING;
            Current_Process = ii;
            Set_Machine_State( &Process_State[ii].cpu );
            DEBUG("resuming %d", Current_Process);
            return;
        }
    }

    // Find some runable process and run it
    for (ii=0; ii<NUM_PROCESSES; ii++)
    {
        if (Process_State[ii].state == RUNABLE)
        {
            Process_State[ii].state = RUNNING;
            Current_Process = ii;
            Set_Machine_State( &Process_State[ii].cpu );
            DEBUG("restarting %d", Current_Process);
            return;
        }
    }

    // No runable processes, so HALT the system
    Process_State[ii].cpu.halted = 1;
    return;
}
//*************************************
int  Fork()
{
    int ii;
    int status;

    Get_Machine_State(&Process_State[Current_Process].cpu);
    status = Process_State[Current_Process].state;

    // Find some empty slot to use
    for (ii=0; ii<NUM_PROCESSES; ii++)
    {
        if (Process_State[ii].state == EMPTY)
        {
            DEBUG("FORK: %d", ii);
            if (status != EMPTY)
            {
                int mem_diff = Process_State[ii].base - 
                    Process_State[Current_Process].base;

                duplicate_memory(ii, Current_Process);
                Get_Machine_State(&Process_State[ii].cpu);
                Process_State[ii].cpu.IP += mem_diff;
                Process_State[ii].cpu.SP += mem_diff;
                Process_State[ii].cpu.FP += mem_diff;
            }
            else
            {
                // no point duplicating memory. 
                // Set regs to point at start of memory
                Process_State[ii].cpu.IP = Process_State[ii].base;
                Process_State[ii].cpu.SP = Process_State[ii].base;
                Process_State[ii].cpu.FP = Process_State[ii].base;
            }

            Process_State[ii].state = RUNABLE;
            
            // In the child, we need to simulate the pushing of a zero
            // because the syscall code that handles the return value 
            // will not execute in the child
            Set_Word(Process_State[ii].cpu.SP, 0);
            Process_State[ii].cpu.SP += WORD_SIZE;
            
            // Current_Process = ii;

            // Set_Machine_State( &Process_State[ii].cpu );
            // DEBUG("starting %d", Current_Process);

            return Current_Process+1;;
        }
    }

    return -1;
}
//*************************************
int  Sched_Load(char *filename)
{
    int status;

    DEBUG("load: %s", filename);

    Get_Machine_State(&Process_State[Current_Process].cpu);
    status = Load(&Process_State[Current_Process].cpu, filename, 
        Process_State[Current_Process].base, 
        Process_State[Current_Process].top);

    if (status != 0)
    {
        Process_State[Current_Process].state = EMPTY;
        return status;
    }

    Process_State[Current_Process].state = RUNABLE;
    Set_Machine_State(&Process_State[Current_Process].cpu);

    return 0;
}

//*************************************
void  Wait(int process)
{
    DEBUG("wait: %d", process);
    Get_Machine_State(&Process_State[Current_Process].cpu);
    Process_State[Current_Process].state = WAITING;
    Process_State[Current_Process].waiting_for = process - 1;
    Schedule();
}

