// test the syscall interface

#include <machine_def.h>

#define EXIT_CALL 1
#define PRINTS_CALL 2

int main();

void *Interrupt_Vector[16];

int systrap(int size, int op, int parm1)
{
    if (op == PRINTS_CALL)
    {
        asm("OUTS", parm1);
    } else if (op == EXIT_CALL) {
        asm("HALT");
    }
    asm("RTI");
}

int syscall(int size, int op, int parm1)
{
    asm("TRAP");

    return 0;
}

int prints(char *string)
{
    syscall(3, PRINTS_CALL, string);
    return 0;
}

#pragma startup startup__
int startup__()
{
    int flag;

    asm2("POPREG", IVEC_REG, &Interrupt_Vector);
    Interrupt_Vector[1] = systrap;

    flag = asm2("PUSHREG", FLAG_REG);
    flag |= FL_USER_MODE;
    asm2("POPREG", FLAG_REG, flag);
    main();
    prints("Halt didn't work\n");
    asm("HALT");
}

int main()
{
    //sys_prints("Attempting to trap\n");
    prints("Printed from trap\n");
    syscall(3, EXIT_CALL, 0);

    return 0;
}
