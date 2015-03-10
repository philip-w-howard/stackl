#pragma once
#include "machine.h"

#define EXIT_CALL       1
#define PRINT_CALL      2
#define PRINTSA_CALL    3
#define GETS_CALL       4
#define GETL_CALL       5
#define GETI_CALL       6

void syscall(Machine_State *cpu, int *args);
