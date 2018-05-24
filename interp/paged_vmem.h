#pragma once
#include <stdint.h>
#include "machine.h"

int32_t VM_Get_Word(Machine_State *regs, int32_t address);
void VM_Set_Word(Machine_State *regs, int32_t address, int32_t value);
int32_t VM_Get_Byte(Machine_State *regs, int32_t address);
void VM_Set_Byte(Machine_State *regs, int32_t address, int value);
void *VM_Get_Addr(Machine_State *regs, int32_t addess);

int VM_Check_Addr(Machine_State *regs, int32_t address, int is_byte);

