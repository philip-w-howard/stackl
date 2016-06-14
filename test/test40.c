#include <string.h>

int main()
{
    char buff[20];

    asm("nop");
    buff = itostr(5, buff);
    asm("nop");

    asm("outs", buff);
    asm("outs", "\n");

    return 0;
}
