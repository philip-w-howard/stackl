#pragma once
#include "machine.h"

void Init_Memory(int mem_size);
int Mem_Size();

int Get_Word(Machine_State *regs, int address);
void Set_Word(Machine_State *regs, int address, int value);
int Get_Byte(Machine_State *regs, int address);
void Set_Byte(Machine_State *regs, int address, int value);
void *Get_Addr(Machine_State *regs, int addess);

int xGet_Word(int address);
void xSet_Word(int address, int value);
int xGet_Byte(int address);
void xSet_Byte(int address, int value);
void *xGet_Addr(int address);
