//*****************************************************
// Check divide by zero and modulo by zero
//
#include <machine_def.h>


#pragma interrupt machine_check
void machine_check()
{
    asm("OUTS", "machine check occurred\n");
    asm2("POPREG", FLAG_REG, 0x00);
    asm("RTI");
}

#pragma systrap trap_handler
void trap_handler()
{
    asm("OUTS", "Halting\n");
    asm("HALT");
}

int main()
{
    int val1 = 10;
    int val2 = 0;
    int  val3;

    asm2("POPREG", FLAG_REG, FL_USER_MODE);

    // test divide by zero: Shoud machine check
    val3 = val1 / val2;

    // test modulo by zero: Shoud machine check
    val3 = val1 % val2;

    asm("TRAP");

    return 0;
}
