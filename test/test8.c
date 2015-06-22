#include <string.h>
#include <io.h>
int f2();
int f1(int a, int b);
int f1(int a, int b)
{
    printi(a + b);
    prints("\n");
}
int main()
{
    int ret;
    printi(0);
    prints("\n");
    f1(5, 8);
    ret = f2();
    printi(3);
    prints("\n");
    printi(ret);
    prints("\n");
    ret = 2*f2() - 3;
    printi(ret);
    prints("\n");
}
int f2()
{
    printi(2);
    prints("\n");
    return 4;
}
