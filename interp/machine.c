#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"

char Memory[MEMORY_SIZE];
Machine_State Regs;

//***************************************
int Get_Word(int address)
{
    if (address < 0 || (address+WORD_SIZE) >= MEMORY_SIZE)
    {
        fprintf(stderr, "Machine check: address 0x%08X out of bounds\n", address);
        exit(-1);
    } 
    else if (address & 0x0003)
    {
        fprintf(stderr, "Machine check: misalligned address 0x%08X out of bounds\n", address);
        exit(-1);
    }

    return *(int *)&Memory[address];
}
//***************************************
void Set_Word(int address, int value)
{
    if (address < 0 || (address+WORD_SIZE) >= MEMORY_SIZE)
    {
        fprintf(stderr, "Machine check: address 0x%08X out of bounds\n", address);
        exit(-1);
    } 
    else if (address & 0x0003)
    {
        fprintf(stderr, "Machine check: misalligned address 0x%08X out of bounds\n", address);
        exit(-1);
    }

    *(int *)&Memory[address] = value;
}
//***************************************
int Get_Byte(int address)
{
    if (address < 0 || address >= MEMORY_SIZE)
    {
        fprintf(stderr, "Machine check: address 0x%08X out of bounds\n", address);
        exit(-1);
    } 

    return Memory[address];
}
//***************************************
void Set_Byte(int address, int value)
{
    if (address < 0 || address >= MEMORY_SIZE)
    {
        fprintf(stderr, "Machine check: address 0x%08X out of bounds\n", address);
        exit(-1);
    } 

    Memory[address] = value;
}
//***************************************
void *Get_Addr(int address)
{
    if (address < 0 || address >= MEMORY_SIZE)
    {
        fprintf(stderr, "Machine check: address 0x%08X out of bounds\n", address);
        exit(-1);
    } 

    return &Memory[address];
}
