#pragma once
#include "machine.h"

void Sched_Init();
void Schedule();
void Exit();
int  Fork();
int  Sched_Load(char *filename);
void Wait(int process);
