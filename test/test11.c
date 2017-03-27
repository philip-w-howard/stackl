#include <sysio.h>
void foo()
{
    asm("OUTS", "in foo()\n");
}

int main()
{
    foo();
    asm("CALLI", foo);
    prints("Test Complete\n");
    return 0;
}
