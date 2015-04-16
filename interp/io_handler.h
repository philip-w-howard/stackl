#pragma once
#include "sched.h"

#define IO_QUEUED   1
#define IO_PENDING  2
#define IO_COMPLETE 4

void Init_IO();
void Finish_IO();
void Schedule_IO(int io_blk_addr);
