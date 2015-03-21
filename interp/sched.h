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
void Exit();
int  Fork();
int  Sched_Load(char *filename);
void WaitProc(int process);
void WaitIO(int io_op, void *addr);
void Reschedule(Process_State_t *proc);
