#pragma once

typedef struct
{
    char *mem;
    int IP;
    int SP;
    int FP;
} Machine_State;

int Load(Machine_State *cpu, const char *filename);
int Load_And_Go(const char *filename);
