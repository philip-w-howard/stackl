#pragma once
#include "machine.h"
#include "opcodes.h"

void Execute(Machine_State *machine);
void Opcodes_Debug();
void Set_Max_Instr(int max);
void Set_Timer_Interval(int instr);
