#pragma interrupt interrupt
#include <machine_def.h>

int interrupt()
{
    asm("OUTS", "interrupt triggered\n");
    asm2("POPREG", FLAG_REG, 0x00);
    asm("RTI");
}

int main()
{
    int value;

    asm2("POPREG", FLAG_REG, FL_I_FIRST_INT);

    value = asm("seid");
    if (value)
        asm("OUTS", "prior to seid was set\n");
    else
        asm("OUTS", "prior to seid was clear\n");

    value = asm2("PUSHREG", FLAG_REG);
    value |= FL_I_FIRST_INT;
    asm2("POPREG", FLAG_REG, value);

    asm("OUTS", "interrupt requested\n");

    value = asm("clid");
    if (value)
        asm("OUTS", "prior to clid was set\n");
    else
        asm("OUTS", "prior to clid was clear\n");

    return value;
}
