// Version V0.10.2-75-g522eb4d prints "-" instead of "-2147483648"
#include <sysio.h>
int main()
{
    printi(0x80000000);
    prints("\n");
    return 0;
}
