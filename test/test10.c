#include "test/io.h"
void voidfunc()
{
    asm("OUTS", "in voidfunc\n");
}
int main()
{
    voidfunc();
    voidfunc();

    prints("Test complete\n");
    return 0;
}
