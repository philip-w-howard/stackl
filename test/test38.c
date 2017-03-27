// Pointer arithmetic and type casting
#include <sysio.h>
int main()
{
    int value1 = 0;
    int value2 = 1;
    *(&value1 + 1) = 10;
    *(((char *)(&value1))+1) = 5;
    printi(value1);
    prints("\n");
    printi(value2);
    prints("\n");

    return 0;
}
