#pragma once
#include <stdint.h>

#include "machine.h"

#define IO_QUEUED   0x10000000
#define IO_PENDING  0x20000000
#define IO_ERROR    0x40000000
#define IO_COMPLETE 0x80000000

#define IO_FLAGS    (IO_QUEUED | IO_PENDING | IO_COMPLETE)

typedef struct
{
    int32_t op;
    int32_t param1;
    int32_t param2;
} io_blk;

void Init_IO(int allow_INP);
void Schedule_IO(Machine_State *cpu, int32_t io_blk_addr);
void IO_Enable_Inp();

int32_t IO_Get_Word(int32_t address);
void IO_Set_Word(int32_t address, int32_t value);
int32_t IO_Get_Byte(int32_t address);
void IO_Set_Byte(int32_t address, int32_t value);

int IO_Register_Handler(int32_t id, int32_t addr, int32_t size, 
        int32_t (*get_word)(int32_t id, int32_t addr),
        int32_t (*get_byte)(int32_t id, int32_t addr),
        void (*set_word)(int32_t id, int32_t addr, int32_t value),
        void (*set_byte)(int32_t id, int32_t addr, int32_t value)
        );

