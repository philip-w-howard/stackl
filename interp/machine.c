#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"

char Memory[MEMORY_SIZE];
Machine_State Regs;

//**************************************
void Machine_Check(const char *fmt, ...)
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
int Is_User_Mode()
{
    return Regs.FLAG & FL_USER_MODE;
}
//***************************************
int Set_User_Mode(int value)
{
    int user_mode = Regs.FLAG & FL_USER_MODE;

    if (value)
        Regs.FLAG |= FL_USER_MODE;
    else
        Regs.FLAG &= ~FL_USER_MODE;

    return user_mode;
}
//***************************************
int Get_Word(int address)
{
    int value;

    if (address & 0x0003)
    {
        Machine_Check("misaligned address %d\n", address);
        exit(-1);
    }
    if (Regs.FLAG & FL_USER_MODE)
    {
        if (address < 0 || (address+WORD_SIZE) > Regs.LP - Regs.BP)
        {
            Machine_Check("address %d out of bounds\n", address);
            exit(-1);
        } 

        value = *(int *)&Memory[Regs.BP+address];
    } else {
        value = *(int *)&Memory[address];
    }

    return value;
}
//***************************************
void Set_Word(int address, int value)
{
    if (address & 0x0003)
    {
        Machine_Check("misaligned address %d\n", address);
        exit(-1);
    }

    if (Regs.FLAG & FL_USER_MODE)
    {
        if (address < 0 || (address+WORD_SIZE) > Regs.LP - Regs.BP)
        {
            Machine_Check("address %d out of bounds\n", address);
            exit(-1);
        } 

        *(int *)&Memory[Regs.BP+address] = value;
    } else {
        *(int *)&Memory[address] = value;
    }
}
//***************************************
int Get_Byte(int address)
{
    int value;

    if (Regs.FLAG & FL_USER_MODE)
    {
        if (address < 0 || (address+WORD_SIZE) > Regs.LP - Regs.BP)
        {
            Machine_Check("address %d out of bounds\n", address);
            exit(-1);
        } 

        value = Memory[Regs.BP+address];
    } else {
        value = Memory[address];
    }

    return value;
}
//***************************************
void Set_Byte(int address, int value)
{
    if (Regs.FLAG & FL_USER_MODE)
    {
        if (address < 0 || (address+WORD_SIZE) > Regs.LP - Regs.BP)
        {
            Machine_Check("address %d out of bounds\n", address);
            exit(-1);
        } 

        Memory[Regs.BP+address] = value;
    } else {
        Memory[address] = value;
    }
}
//***************************************
void *Get_Addr(int address)
{
    if (address < 0 || address >= Regs.LP - Regs.BP)
    {
        Machine_Check("address %d out of bounds\n", address);
        exit(-1);
    } 

    if (Regs.FLAG & FL_USER_MODE)
    {
        return &Memory[Regs.BP+address];
    } else {
        return &Memory[address];
    }
}
//***************************************
void Init_Machine()
{
    Regs.BP = 0;
    Regs.LP = MEMORY_SIZE;
    Regs.IP = 0;
    Regs.FP = 0;
    Regs.SP = 0;
    Regs.FLAG = 0;
}
//***************************************
void Get_Machine_State(Machine_State *cpu)
{
    if (Regs.FLAG & FL_USER_MODE) 
    {
        Machine_Check("Protected instruction exception");
    }

    memcpy(cpu, &Regs, sizeof(Regs));
}
//***************************************
void Set_Machine_State(Machine_State *cpu)
{
    if (Regs.FLAG & FL_USER_MODE) 
    {
        Machine_Check("Protected instruction exception");
    }

    memcpy(&Regs, cpu, sizeof(Regs));
}
//***************************************
void Machine_Execute()
{
    while (!Regs.FLAG & FL_HALTED)
    {
        Execute(&Regs);
    }
}
