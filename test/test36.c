// check precedence of *ptr++
// This was a bug reported in V0.7.5
//
#include <string.h>
#include "test/io.h"

int main()
{
    char *buff;
    char *ptr;

    buff = "1234";
    ptr = buff;

    while (*buff)
    {
        if (*ptr++ != *buff) prints("precedence is wrong\n");
        buff++;
    }

    prints("Test complete\n");

    return 0;
}
