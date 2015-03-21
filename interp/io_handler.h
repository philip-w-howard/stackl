#pragma once
#include "sched.h"

void Init_IO();
void Finish_IO();
void Schedule_IO(int io_op, void *addr, Process_State_t *proc);
