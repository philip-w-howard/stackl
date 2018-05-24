#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_def.h"
#include "io_handler.h"
#include "vmem.h"
#include "pagetable.h"
#include "paged_vmem.h"

//*********************************************
int32_t VM_Get_Word(Machine_State *cpu, int32_t address)
{
    int32_t value;

    address = VM_Check_Addr(cpu, address, 0);
    if (address < 0) return 0;

    if (address < Mem_Size())
        value = Abs_Get_Word(address);
    else
        value = IO_Get_Word(address);

    return value;
}
//***************************************
void VM_Set_Word(Machine_State *cpu, int32_t address, int32_t value)
{
    address = VM_Check_Addr(cpu, address, 0);
    if (address < 0) return;

    if (address < Mem_Size()) 
        Abs_Set_Word(address, value);
    else
        IO_Set_Word(address, value);
}
//***************************************
int32_t VM_Get_Byte(Machine_State *cpu, int32_t address)
{
    int32_t value;

    address = VM_Check_Addr(cpu, address, 1);
    if (address < 0) return 0;

    if (address < Mem_Size())
        value = Abs_Get_Byte(address);
    else
        value = IO_Get_Byte(address);

    return value;
}
//***************************************
void VM_Set_Byte(Machine_State *cpu, int32_t address, int32_t value)
{
    address = VM_Check_Addr(cpu, address, 1);
    if (address < 0) return;

    if (address < Mem_Size())
        Abs_Set_Byte(address, value);
    else
        IO_Set_Byte(address, value);
}
//***************************************
void *VM_Get_Addr(Machine_State *cpu, int32_t address)
{
    //int value = Get_Word(address);
    address = VM_Check_Addr(cpu, address, 1);
    if (address < 0 || address >= Mem_Size()) 
    {
        Machine_Check(MC_ILLEGAL_ADDR, "address %d out of bounds\n", address);
        return NULL;
    }

    return Abs_Get_Addr(address);
}
//***************************************
int VM_Check_Addr(Machine_State *regs, int32_t address, int is_byte)
{
    if ((regs->FLAG & (FL_USER_MODE|FL_VMEM) ) == (FL_USER_MODE|FL_VMEM))
    {
        if (regs->VMEM <= 0) 
        {
            Machine_Check(MC_ILLEGAL_ADDR, "VMEM reg invalid while in VM mode");
            return 0;
        }
    }

    return 1;
}

