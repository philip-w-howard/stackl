#pragma once

typedef struct
{
    int *mem;
    int IP;
    int SP;
    int FP;
} Machine_State;

int Load_And_Go(const char *filename);
