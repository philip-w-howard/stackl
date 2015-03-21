#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "machine.h"
#include "loader.h"
#include "sched.h"
#include "io_handler.h"

#define NUM_PROCESSES 2

#define EMPTY           0
#define RUNNING         1
#define RUNABLE         2
#define WAITING_PROC    3
#define WAITING_IO      4


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
    int index;
    int from_size;
    int to_size;
    int value;

    from_size =Process_State[from_proc].cpu.LP-Process_State[from_proc].cpu.BP;
    to_size = Process_State[to_proc].cpu.LP - Process_State[to_proc].cpu.BP;
    index = 0;

    while (index < from_size && index < to_size)
    {
        Set_Machine_State(&Process_State[from_proc].cpu);
        value = Get_Word(index);
        Set_Machine_State(&Process_State[to_proc].cpu);
        Set_Word(index, value);
        index += WORD_SIZE;
    }
    Set_Machine_State(&Process_State[Current_Process].cpu);
}
//*************************************
void Sched_Init()
{
    int ii;

    Current_Process = 0;

    for (ii=0; ii<NUM_PROCESSES; ii++)
    {
        Get_Machine_State(&Process_State[ii].cpu);
        Process_State[ii].process_id = ii+1;
        Process_State[ii].cpu.BP = ii * MEMORY_SIZE/NUM_PROCESSES;
        Process_State[ii].cpu.LP = (ii+1)*MEMORY_SIZE/NUM_PROCESSES;
        Process_State[ii].state = EMPTY;
    }
    Set_Machine_State(&Process_State[Current_Process].cpu);
}
//*************************************
void Schedule()
{
    int ii;
    int found_io = 0;

    DEBUG("Schedule");
    Get_Machine_State(&Process_State[Current_Process].cpu);
    if (Process_State[Current_Process].state == RUNNING)
    {
        Process_State[Current_Process].state = RUNABLE;
    }

    while (1)
    {
        // repeat until we find a process to schedule
        Current_Process++;
        for (ii=0; ii<NUM_PROCESSES; ii++)
        {
            if (Current_Process >= NUM_PROCESSES) Current_Process = 0;
            if (Process_State[Current_Process].state == WAITING_IO) found_io++;
            if (Process_State[Current_Process].state == RUNABLE)
            {
                Process_State[Current_Process].state = RUNNING;
                Set_Machine_State( &Process_State[Current_Process].cpu );
                DEBUG("switch to %d", Current_Process);
                return;
            }
            Current_Process++;
        }

        if (!found_io)
        {
            fprintf(stderr, "Machine halting: no runable processes\n");
            exit(-1);
        }
        else
        {
            // wait for IO to complete
            usleep(10000);
        }
    }
}
//*************************************
void Sched_Exit(int status)
{
    int ii;

    DEBUG("stopping %d", Current_Process);
    Get_Machine_State(&Process_State[Current_Process].cpu);
    Process_State[Current_Process].state = EMPTY;

    // if somebody was waiting for us, set them to running and run them
    for (ii=0; ii<NUM_PROCESSES; ii++)
    {
        if (Process_State[ii].state == WAITING_PROC &&
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
    exit(status);
    return;
}
//*************************************
int  Sched_Fork()
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
              duplicate_memory(ii, Current_Process);
              Process_State[ii].cpu.IP = Process_State[Current_Process].cpu.IP;
              Process_State[ii].cpu.SP = Process_State[Current_Process].cpu.SP;
              Process_State[ii].cpu.FP = Process_State[Current_Process].cpu.FP;
              
              // In the child, we need to simulate the pushing of a zero
              // because the syscall code that handles the return value 
              // will not execute in the child
              Set_Word(Process_State[ii].cpu.SP - WORD_SIZE, 0);
              //Process_State[ii].cpu.SP += WORD_SIZE;
            }
            else
            {
              // no point duplicating memory. 
              // Set regs to point at start of memory
              Process_State[ii].cpu.IP = 0;
              Process_State[ii].cpu.SP = 0;
              Process_State[ii].cpu.FP = 0;
            }

            Process_State[ii].state = RUNABLE;
            
            
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
            0,
            Process_State[Current_Process].cpu.LP -  
                Process_State[Current_Process].cpu.BP);

    if (status != 0)
    {
        // If the load fails, we keep the state RUNABLE because
        // the original code should still run to return non-zero from exec()
        //
        // Process_State[Current_Process].state = EMPTY;
        // Schedule();
        return status;
    }

    Process_State[Current_Process].state = RUNABLE;
    Set_Machine_State(&Process_State[Current_Process].cpu);

    return 0;
}

//*************************************
void  Sched_WaitProc(int process)
{
    DEBUG("wait: %d", process);
    Get_Machine_State(&Process_State[Current_Process].cpu);
    Process_State[Current_Process].state = WAITING_PROC;
    Process_State[Current_Process].waiting_for = process;
    Schedule();
}

//*************************************
void  Sched_WaitIO(int io_op, void *addr)
{
    DEBUG("wait IO: %d", io_op);
    Get_Machine_State(&Process_State[Current_Process].cpu);
    Process_State[Current_Process].state = WAITING_IO;
    Schedule_IO(io_op, addr, &Process_State[Current_Process]);
    Schedule();
}
//*************************************
void Sched_Reschedule(Process_State_t *proc)
{
    DEBUG("Reschedule %d", proc->process_id);
    proc->state = RUNABLE;
}
