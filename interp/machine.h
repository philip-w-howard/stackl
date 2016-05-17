#pragma once
#include <stdint.h>

#define INTERRUPT_VECTOR    0
#define TRAP_VECTOR         1

typedef struct
{
    int32_t BP;         // Base Pointer: base addr for mem
    int32_t LP;         // Limit Pointer: high water mark for mem
    int32_t IP;         // Instructino Pionter
    int32_t SP;         // Stack Pointer
    int32_t FP;         // Frame Pointer
    int32_t FLAG;       // FLAG register (bits defined above)
} Machine_State;

int Is_User_Mode();
int Set_User_Mode(int value);

void Init_Machine();
void Get_Machine_State(Machine_State *cpu);
void Set_Machine_State(Machine_State *cpu);
void Machine_Execute();
void Machine_Check(const char *fmt, ...);
void Machine_Signal_Interrupt(int from_hw);

