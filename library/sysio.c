#pragma once
#include <string.h>
#include <sysio.h>

int prints(char *msg)
{
    asm("OUTS", msg);
    return 0;
}

int printi(int val)
{
    char buff[20];
    itostr(val, buff);
    prints(buff);
    return 0;
}


int printx(int val)
{
    char buff[20];
    xtostr(val, buff);
    prints(buff);
    return 0;
}

int printxn(int val, int len)
{
    char buff[20];
    xtostr(val, buff);
    len -= strlen(buff);
    while (len > 0)
    {
        asm("OUTS", "0");
        len--;
    }
    prints(buff);
    return 0;
}

