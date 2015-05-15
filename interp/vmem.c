#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_def.h"
#include "io_handler.h"
#include "vmem.h"

#define DEFAULT_MEMORY_SIZE  20000

static char *Memory;
static int  Memory_Size = DEFAULT_MEMORY_SIZE;

//***************************************
int Mem_Size()
{
    return Memory_Size;
}
//***************************************
static int validate_address(Machine_State *regs, int address, int is_char)
{
    if (address < 0)
    {
        Machine_Check("Invalid address at %d\n", address);
    }

    if ( (address & 0x0003) && !is_char)
    {
        Machine_Check("misaligned address %d\n", address);
        exit(-1);
    }

    if (regs->FLAG & FL_USER_MODE)
    {
        if (address < 0 || (address+WORD_SIZE) > regs->LP - regs->BP)
        {
            Machine_Check("address %d out of bounds\n", address);
            exit(-1);
        } 

        address = regs->BP + address;
    }

    return address;
}
//***************************************
int Get_Word(Machine_State *cpu, int address)
{
    int value;

    address = validate_address(cpu, address, 0);
    if (address < Memory_Size)
        value = *(int *)&Memory[address];
    else
        value = IO_Get_Word(address);

    //fprintf(Mem_Log, "%5d %8d %08X Get_Word\n", address, value, value);
    return value;
}
//***************************************
void Set_Word(Machine_State *cpu, int address, int value)
{
    address = validate_address(cpu, address, 0);
    //fprintf(Mem_Log, "%5d %8d %08X Set_Word\n", address, value, value);
    if (address < Memory_Size)
        *(int *)&Memory[address] = value;
    else
        IO_Set_Word(address, value);
}
//***************************************
int Get_Byte(Machine_State *cpu, int address)
{
    int value;

    address = validate_address(cpu, address, 1);
    if (address < Memory_Size)
        value = Memory[address];
    else
        value = IO_Get_Byte(address);

    //fprintf(Mem_Log, "%5d %8d %08X Get_Byte\n", address, value, value);
    return value;
}
//***************************************
void Set_Byte(Machine_State *cpu, int address, int value)
{
    address = validate_address(cpu, address, 1);
    //fprintf(Mem_Log, "%5d %8d %08X Set_Byte\n", address, value, value);
    if (address < Memory_Size)
        Memory[address] = value;
    else
        IO_Set_Byte(address, value);
}
//***************************************
void *Get_Addr(Machine_State *cpu, int address)
{
    //int value = Get_Word(address);
    address = validate_address(cpu, address, 1);
    //fprintf(Mem_Log, "%5d %8d %08X Get_Addr\n", address, value, value);
    return &Memory[address];
}
//***************************************
void Init_Memory(int mem_size)
{
    if (mem_size <= 0) mem_size = DEFAULT_MEMORY_SIZE;
    Memory_Size = mem_size;
    Memory = (char *)malloc(Memory_Size);
    memset(Memory, 0x79, Memory_Size);
}
//***************************************
int xGet_Word(int address)
{
    Machine_State cpu;
    Get_Machine_State(&cpu);
    return Get_Word(&cpu, address);
}
//***************************************
void xSet_Word(int address, int value)
{
    Machine_State cpu;
    Get_Machine_State(&cpu);
    return Set_Word(&cpu, address, value);
}
//***************************************
int xGet_Byte(int address)
{
    Machine_State cpu;
    Get_Machine_State(&cpu);
    return Get_Byte(&cpu, address);
}
//***************************************
void xSet_Byte(int address, int value)
{
    Machine_State cpu;
    Get_Machine_State(&cpu);
    return Set_Byte(&cpu, address, value);
}
//***************************************
void *xGet_Addr(int address)
{
    Machine_State cpu;
    Get_Machine_State(&cpu);
    return Get_Addr(&cpu, address);
}
