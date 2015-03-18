#pragma once
#include "machine.h"

void Syscall_Debug();
int syscall(Machine_State *cpu, int *args);
