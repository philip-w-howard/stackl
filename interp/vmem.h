#pragma once
#include "machine.h"

void Init_Memory(int mem_size);
void VM_Enable_Log(int enabled);
int Mem_Size();

int Get_Word(Machine_State *regs, int address);
void Set_Word(Machine_State *regs, int address, int value);
int Get_Byte(Machine_State *regs, int address);
void Set_Byte(Machine_State *regs, int address, int value);
void *Get_Addr(Machine_State *regs, int addess);

int Abs_Get_Word(int address);
void Abs_Set_Word(int address, int value);
int Abs_Get_Byte(int address);
void Abs_Set_Byte(int address, int value);
void *Abs_Get_Addr(int address);
