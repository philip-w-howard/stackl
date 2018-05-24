#pragma once
#include <stdint.h>
#include "machine_def.h"

typedef void* debug_info;

typedef struct
{
    int32_t BP;         // Base Pointer: base addr for mem
    int32_t LP;         // Limit Pointer: high water mark for mem
    int32_t IP;         // Instructino Pionter
    int32_t SP;         // Stack Pointer
    int32_t FP;         // Frame Pointer
    int32_t FLAG;       // FLAG register (bits defined in machine_def.h)
    int32_t IVEC;       // Address of the Interrupt Vector Table
    int32_t VMEM;       // Address of the Page Table
    debug_info debugger;
} Machine_State;

void Init_Machine(int mem_size);
void Get_Machine_State(Machine_State *cpu);
void Set_Machine_State(Machine_State *cpu);
void Set_Debug_Info(debug_info debugger);
void Machine_Execute();
void Machine_Check(int code, const char *fmt, ...);
void Machine_Signal_Interrupt(int from_hw, int32_t vector);

int Machine_Check_Happened();

