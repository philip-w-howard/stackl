#include <stdio.h>

int puts(char* s)
{
    asm("OUTS", s);
    asm("OUTS", "\n");
    return 0;
}