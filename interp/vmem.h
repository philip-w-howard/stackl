#pragma once
#include <stdint.h>
#include "machine.h"

void Init_Memory(int mem_size);
void VM_Enable_Log(int enabled);
int Mem_Size();

int32_t Get_Word(Machine_State *regs, int32_t address);
void Set_Word(Machine_State *regs, int32_t address, int32_t value);
int32_t Get_Byte(Machine_State *regs, int32_t address);
void Set_Byte(Machine_State *regs, int32_t address, int value);
void *Get_Addr(Machine_State *regs, int32_t addess);

int32_t Abs_Get_Word(int32_t address);
void Abs_Set_Word(int32_t address, int32_t value);
int32_t Abs_Get_Byte(int32_t address);
void Abs_Set_Byte(int32_t address, int32_t value);
void *Abs_Get_Addr(int32_t address);
