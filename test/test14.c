#include <string.h>
#include <sysio.h>

const int ONE = 1;
#define TWO 2

// const decls
int main()
{
    int aa;
    aa = 5;
    char *ptr;
    ptr = "mixing decls and statements works\n";
    if (aa) prints(ptr);

    printi(ONE);
    prints("\n");
    printi(TWO);
    prints("\n");

    return 0;
}
