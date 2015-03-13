#include "system.h"
int f2();
int f1(int a, int b);
int f1(int a, int b)
{
    printi(a + b);
}
int main()
{
    int ret;
    printi(0);
    f1(5, 8);
    ret = f2();
    printi(3);
    printi(ret);
    ret = 2*f2() - 3;
    printi(ret);
}
int f2()
{
    printi(2);
    return 4;
}
