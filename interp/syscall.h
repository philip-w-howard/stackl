#pragma once
#include "machine.h"
#include "system.h"

void syscall(Machine_State *cpu, int *args);
