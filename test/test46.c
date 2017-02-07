#include "test/io.h"
//int printi(int val);

int foo(char a, int b)
{
    int c;

    c = a + b;
    printi(c);
    return c;
}

int main()
{
    foo(5,4);
    return 0;
}
