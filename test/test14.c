#include "system.h"

const int ONE = 1;
#define TWO 2

int main()
{
    int aa;
    aa = 5;
    char *ptr;
    ptr = "mixing decls and statements works\n";
    if (aa) prints(ptr);

    printi(ONE);
    printi(TWO);

    return 0;
}
