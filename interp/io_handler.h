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

void Init_IO();
void Finish_IO();
void Schedule_IO(int io_blk_addr);
