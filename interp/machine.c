#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_def.h"
#include "opcodes.h"
#include "io_handler.h"

#define DEFAULT_MEMORY_SIZE  20000

static char *Memory;
static int  Memory_Size = DEFAULT_MEMORY_SIZE;
static Machine_State Regs;
static pthread_mutex_t Machine_Lock = PTHREAD_MUTEX_INITIALIZER;

static FILE *Mem_Log;

void machine_debug()
{
    //printf("***************************** machine debug\n");
}
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
static int validate_address(int address, int is_char)
{
    if ( (address & 0x0003) && !is_char)
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

        address = Regs.BP + address;
    }

    return address;
}
//***************************************
int Get_Word(int address)
{
    int value;

    address = validate_address(address, 0);
    if (address < Memory_Size)
        value = *(int *)&Memory[address];
    else
        value = IO_Get_Word(address);

    if (value == 0x79797979) machine_debug();
    //fprintf(Mem_Log, "%5d %8d %08X Get_Word\n", address, value, value);
    return value;
}
//***************************************
void Set_Word(int address, int value)
{
    address = validate_address(address, 0);
    //fprintf(Mem_Log, "%5d %8d %08X Set_Word\n", address, value, value);
    if (address < Memory_Size)
        *(int *)&Memory[address] = value;
    else
        IO_Set_Word(address, value);
}
//***************************************
int Get_Byte(int address)
{
    int value;

    address = validate_address(address, 1);
    if (address < Memory_Size)
        value = Memory[address];
    else
        value = IO_Get_Byte(address);

    if (value == 0x79) machine_debug();
    //fprintf(Mem_Log, "%5d %8d %08X Get_Byte\n", address, value, value);
    return value;
}
//***************************************
void Set_Byte(int address, int value)
{
    address = validate_address(address, 1);
    //fprintf(Mem_Log, "%5d %8d %08X Set_Byte\n", address, value, value);
    if (address < Memory_Size)
        Memory[address] = value;
    else
        IO_Set_Byte(address, value);
}
//***************************************
void *Get_Addr(int address)
{
    //int value = Get_Word(address);
    address = validate_address(address, 1);
    //fprintf(Mem_Log, "%5d %8d %08X Get_Addr\n", address, value, value);
    return &Memory[address];
}
//***************************************
void Init_Machine(int mem_size)
{
    Mem_Log = fopen("mem.log", "w");

    pthread_mutex_lock(&Machine_Lock);
    if (mem_size <= 0) mem_size = DEFAULT_MEMORY_SIZE;
    Memory_Size = mem_size;
    Memory = (char *)malloc(Memory_Size);
    memset(Memory, 0x79, Memory_Size);
    Regs.BP = 0;
    Regs.LP = Memory_Size;
    Regs.IP = 0;
    Regs.FP = 0;
    Regs.SP = 0;
    Regs.FLAG = 0;
    pthread_mutex_unlock(&Machine_Lock);
}
//***************************************
void Get_Machine_State(Machine_State *cpu)
{
    pthread_mutex_lock(&Machine_Lock);
    if (Regs.FLAG & FL_USER_MODE) 
    {
        Machine_Check("Protected instruction exception");
    }

    memcpy(cpu, &Regs, sizeof(Regs));
    pthread_mutex_unlock(&Machine_Lock);
}
//***************************************
void Set_Machine_State(Machine_State *cpu)
{
    pthread_mutex_lock(&Machine_Lock);
    if (Regs.FLAG & FL_USER_MODE) 
    {
        Machine_Check("Protected instruction exception");
    }

    memcpy(&Regs, cpu, sizeof(Regs));
    pthread_mutex_unlock(&Machine_Lock);
}
//***************************************
void Machine_Execute()
{
    while ( !(Regs.FLAG & FL_HALTED) )
    {
        pthread_mutex_lock(&Machine_Lock);
        Execute(&Regs);
        pthread_mutex_unlock(&Machine_Lock);
    }
}
//***************************************
void Machine_Signal_Interrupt()
{
    pthread_mutex_lock(&Machine_Lock);
    Regs.FLAG |= FL_INT_PENDING;
    pthread_mutex_unlock(&Machine_Lock);
}

