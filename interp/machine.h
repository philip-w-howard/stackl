#pragma once
#include <stdint.h>

#define MACHINE_CHECK_VECTOR    0
#define HW_INTERRUPT_VECTOR     0
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
void Set_Debug_Info(debug_info debugger);
void Machine_Execute();
void Machine_Check(int code, const char *fmt, ...);
void Machine_Signal_Interrupt(int from_hw, int32_t vector);

// Machine check codes
#define MC_ILLEGAL_INST 0x00000001
#define MC_ILLEGAL_ADDR 0x00000002
#define MC_HW_FAILURE   0x00000004
#define MC_HW_WARNING   0x00000008
#define MC_PROT_INST    0x00000010
