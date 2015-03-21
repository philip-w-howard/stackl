#pragma once

#define MEMORY_SIZE  20000
#define WORD_SIZE 4

typedef struct
{
    int BP;         // Base Pointer: base addr for mem
    int LP;         // Limit Pointer: high water mark for mem
    int IP;         // Instructino Pionter
    int SP;         // Stack Pointer
    int FP;         // Frame Pointer
    int halted;     // CPU halted
} Machine_State;

int Get_Word(int address);
void Set_Word(int address, int value);
int Get_Byte(int address);
void Set_Byte(int address, int value);
void *Get_Addr(int addess);

void Init_Machine();
void Get_Machine_State(Machine_State *cpu);
void Set_Machine_State(Machine_State *cpu);
void Machine_Execute();

