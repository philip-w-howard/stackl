#pragma once
#include "machine.h"

//int Load(Machine_State *cpu, const char *filename, int base, int top);
void Loader_Debug();
int Boot(const char *filename);
int Boot_From_Disk();
int Load(const char *filename);
int Load_Text(const char *filename);
