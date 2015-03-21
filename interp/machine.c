#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"

char Memory[MEMORY_SIZE];
Machine_State Regs;

//**************************************
static void machine_check(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char format[200];
    sprintf(format, "Machine check: %s\n", fmt);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(-1);
}
//***************************************
int Get_Word(int address)
{
    if (address < 0 || (address+WORD_SIZE) > Regs.LP - Regs.BP)
    {
        machine_check("address %d out of bounds\n", address);
        exit(-1);
    } 
    else if (address & 0x0003)
    {
        machine_check("misaligned address %d\n", address);
        exit(-1);
    }

    return *(int *)&Memory[Regs.BP+address];
}
//***************************************
void Set_Word(int address, int value)
{
    if (address < 0 || (address+WORD_SIZE) > Regs.LP - Regs.BP)
    {
        machine_check("address %d out of bounds\n", address);
        exit(-1);
    } 
    else if (address & 0x0003)
    {
        machine_check("misaligned address %d\n", address);
        exit(-1);
    }

    *(int *)&Memory[Regs.BP+address] = value;
}
//***************************************
int Get_Byte(int address)
{
    if (address < 0 || address >= Regs.LP - Regs.BP)
    {
        machine_check("address %d out of bounds\n", address);
        exit(-1);
    } 

    return Memory[Regs.BP+address];
}
//***************************************
void Set_Byte(int address, int value)
{
    if (address < 0 || address >= Regs.LP - Regs.BP)
    {
        machine_check("address %d out of bounds\n", address);
        exit(-1);
    } 

    Memory[Regs.BP+address] = value;
}
//***************************************
void *Get_Addr(int address)
{
    if (address < 0 || address >= Regs.LP - Regs.BP)
    {
        machine_check("address %d out of bounds\n", address);
        exit(-1);
    } 

    return &Memory[Regs.BP+address];
}
//***************************************
void Init_Machine()
{
    Regs.BP = 0;
    Regs.LP = MEMORY_SIZE;
    Regs.IP = 0;
    Regs.FP = 0;
    Regs.SP = 0;
    Regs.halted = 0;
}
//***************************************
void Get_Machine_State(Machine_State *cpu)
{
    memcpy(cpu, &Regs, sizeof(Regs));
}
//***************************************
void Set_Machine_State(Machine_State *cpu)
{
    memcpy(&Regs, cpu, sizeof(Regs));
}
//***************************************
void Machine_Execute()
{
    while (!Regs.halted)
    {
        Execute(&Regs);
    }
}
