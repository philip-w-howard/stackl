#pragma once
#include <stdint.h>

#define MACHINE_CHECK_VECTOR    0
#define TRAP_VECTOR             1

typedef void* debug_info;

typedef struct
{
    int32_t BP;         // Base Pointer: base addr for mem
    int32_t LP;         // Limit Pointer: high water mark for mem
    int32_t IP;         // Instructino Pionter
    int32_t SP;         // Stack Pointer
    int32_t FP;         // Frame Pointer
    int32_t FLAG;       // FLAG register (bits defined above)
    int32_t IVEC;       // Address if the Interrupt Vector Table
    debug_info debugger;
} Machine_State;

void Init_Machine(int mem_size);
void Get_Machine_State(Machine_State *cpu);
void Set_Machine_State(Machine_State *cpu);
void Machine_Execute();
void Machine_Check(const char *fmt, ...);
void Machine_Signal_Interrupt(int from_hw, int32_t vector);

