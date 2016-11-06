#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine_def.h"
#include "opcodes.h"
#include "io_handler.h"
#include "vmem.h"

#define DEFAULT_MEMORY_SIZE  40000

static Machine_State Regs;
static pthread_mutex_t Machine_Lock = PTHREAD_MUTEX_INITIALIZER;

//static FILE *Mem_Log;

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
void Init_Machine(int mem_size)
{
    //Mem_Log = fopen("mem.log", "w");

    pthread_mutex_lock(&Machine_Lock);

    Init_Memory(mem_size);

    Regs.BP = 0;
    Regs.LP = Mem_Size();
    Regs.IP = 0;
    Regs.FP = 0;
    Regs.SP = 0;
    Regs.FLAG = 0;
    Regs.IVEC = 0;

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
void Machine_Execute_Debug(stackl_debugger& dbg)
{
    dbg.query_user(Regs.IP, Regs.FP);
    while ( !(Regs.FLAG & FL_HALTED) )
    {
        pthread_mutex_lock(&Machine_Lock);
        dbg.debug_check(Regs.IP, Regs.FP);
        Execute(&Regs);
        pthread_mutex_unlock(&Machine_Lock);
    }
}
//***************************************
void Machine_Signal_Interrupt(int from_hw, int32_t vector)
{
    if (from_hw) pthread_mutex_lock(&Machine_Lock);
    Regs.FLAG |= FL_I_FIRST_INT << vector;
    if (from_hw) pthread_mutex_unlock(&Machine_Lock);
}

