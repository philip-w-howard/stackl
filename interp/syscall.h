#pragma once
#include "machine.h"
#include "system.h"

int syscall(Machine_State *cpu, int *args);
