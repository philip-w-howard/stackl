#pragma once

#define MEMORY_SIZE  20000
#define WORD_SIZE 4

typedef struct
{
    int IP;
    int SP;
    int FP;
    int halted;
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

