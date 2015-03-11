#pragma once
#include "machine.h"

#define EXIT_CALL       1
#define PRINT_CALL      2
#define PRINTSA_CALL    3
#define PRINTC_CALL     4
#define GETS_CALL       5
#define GETL_CALL       6
#define GETI_CALL       7
#define EXEC_CALL       8

void syscall(Machine_State *cpu, int *args);
