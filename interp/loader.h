#pragma once
#include "machine.h"

int Load(Machine_State *cpu, const char *filename);
int Load_And_Go(const char *filename);
