// check precedence of *ptr++
// This was a bug reported in V0.7.5
//
#include <string.h>
#include <sysio.h>

//#include <stdio.h>
//#define prints printf

int main()
{
    char *buff;
    char *ptr;

    buff = "1234";
    ptr = buff;

    while (*buff)
    {
        if (*ptr++ != *buff) prints("preinc precedence is wrong\n");
        buff++;
    }

    if (*--ptr != '4') prints("predec precedence is wrong\n");

    prints("Test complete\n");

    return 0;
}
