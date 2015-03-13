#pragma once

#define MEMORY_SIZE  10000
#define WORD_SIZE 4

typedef struct
{
    int IP;
    int SP;
    int FP;
} Machine_State;

int Get_Word(int address);
void Set_Word(int address, int value);
int Get_Byte(int address);
void Set_Byte(int address, int value);
void *Get_Addr(int addess);

