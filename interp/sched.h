#pragma once
#include "machine.h"

typedef struct
{
    int process_id;
    int base;
    int top;
    Machine_State cpu;
    int state;
    int waiting_for;
} Process_State_t;

void Sched_Debug();
void Sched_Init();
void Schedule();
void Sched_Exit();
int  Sched_Fork();
int  Sched_Load(char *filename);
void Sched_WaitProc(int process);
void Sched_WaitIO(int io_op, void *addr);
void Sched_Reschedule(Process_State_t *proc);
