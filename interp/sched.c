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
            return;
        }
    }

    // No runable processes, so HALT the system
    Process_State[ii].cpu.halted = 1;
    return;
}
//*************************************
int  Create(char *filename)
{
    int ii;
    int status;

    Get_Machine_State(&Process_State[Current_Process].cpu);

    // Find some empty slot to use
    for (ii=0; ii<NUM_PROCESSES; ii++)
    {
        if (Process_State[ii].state == EMPTY)
        {
            Current_Process = ii;
            status = Sched_Load(filename);

            if (status != 0)
            {
                Process_State[ii].state = EMPTY;
                return status;
            }

            return 0;
        }
    }

    return 1;
}
//*************************************
int  Sched_Load(char *filename)
{
    int status;
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
    Get_Machine_State(&Process_State[Current_Process].cpu);
    Process_State[Current_Process].state = WAITING;
    Process_State[Current_Process].waiting_for = process;
    Schedule();
}

