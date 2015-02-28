#pragma once
#include "machine.h"

#define NOP         0
#define PLUS_OP     1
#define MINUS_OP    2
#define TIMES_OP    3
#define DIVIDE_OP   4
#define MOD_OP      5
#define EQ_OP       6
#define NE_OP       7
#define GT_OP       8
#define LT_OP       9
#define GE_OP       10
#define LE_OP       11
#define AND_OP      12
#define OR_OP       13
#define PRINT_OP    14
#define PRINTS_OP   15
#define DUP_OP      16
#define HALT_OP     17
#define POP_OP      18
#define CALL_OP     19
#define RETURN_OP   20
#define ILLEGAL_OP  127
#define PUSH_OP     -1
#define JUMP_OP     -2
#define JUMPE_OP    -3
#define PUSHVAR_OP  -4
#define POPVAR_OP   -5
#define ADJSP_OP    -6

void Execute(Machine_State *machine);

