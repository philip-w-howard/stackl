#include <sysio.h>

int foo(char a, int b)
{
    int c;

    c = a + b;
    printi(c);
    return c;
}

int main()
{
    foo((char)5,4);
    return 0;
}
