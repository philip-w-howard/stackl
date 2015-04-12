#include <string.h>
#include <system.h>
// test simple variables and assignment
int main()
{
    int x;
    int y;
    y = 5;
    x = 2;
    printi(x);
    prints("\n");
    printi(y);
    prints("\n");
    y = x;
    printi(y);
    prints("\n");
}
