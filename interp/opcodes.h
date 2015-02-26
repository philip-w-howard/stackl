#pragma once
#include "machine.h"

#define NOP         0
#define PLUS        1
#define MINUS       2
#define TIMES       3
#define DIVIDE      4
#define MOD         5
#define EQUAL       6
#define NOT_EQUAL   7
#define GREATER     8
#define LESS        9
#define GREATER_EQ  10
#define LESS_EQ     11
#define LOG_AND     12
#define LOG_OR      13
#define PRINT       14
#define PRINTS      15
#define DUP         16
#define HALT        17
#define POP         18
#define PUSH        -1
#define JUMP        -2
#define JUMPE       -3

void Execute(Machine_State *machine);

