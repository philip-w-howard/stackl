#pragma once
#include "sched.h"

#define IO_QUEUED   0x10000000
#define IO_PENDING  0x20000000
#define IO_ERROR    0x40000000
#define IO_COMPLETE 0x80000000

#define IO_FLAGS    (IO_QUEUED | IO_PENDING | IO_COMPLETE)

typedef struct
{
    int op;
    int param1;
    int param2;
} io_blk;

void Init_IO(int allow_INP);
void Schedule_IO(Machine_State *cpu, int io_blk_addr);
void IO_Enable_Inp();

int IO_Get_Word(int address);
void IO_Set_Word(int address, int value);
int IO_Get_Byte(int address);
void IO_Set_Byte(int address, int value);

int IO_Register_Handler(int id, int addr, int size, 
        int (*get_word)(int id, int addr),
        int (*get_byte)(int id, int addr),
        void (*set_word)(int id, int addr, int value),
        void (*set_byte)(int id, int addr, int value)
        );

