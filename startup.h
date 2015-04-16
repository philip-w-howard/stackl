#include <syscodes.h>
#include <opcodes.h>

int main();

int interrupt()
{
    asm(RTI_OP);
}

int systrap(// int SP, int FLAG, int FP, int IP, int LP, int BP,
        int size, int op, int parm1)
{
    if (op == PRINTS_CALL)
    {
        asm(OUTS_OP, parm1);
    } else {
        asm(TRAPTOC_OP);
        asm(POP_OP);
    }
    asm(RTI_OP);
}

int syscall(int size, int op, int parm1)
{
    if (op == PRINTS_CALL)
    {
        asm(TRAP_OP);
    } else {
        asm(TRAPTOC_OP);
        asm(POP_OP);
    }
    return 0;
}

int exit(int status)
{
    syscall(3, EXIT_CALL, status);
    return 0;
}

int startup__()
{
    int result;

    result = main();

    exit(result);
}

