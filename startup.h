#include <syscodes.h>
#include <opcodes.h>

int main();

int interrupt()
{
    asm(31);        // RTI_OP
}

int systrap(// int SP, int FLAG, int FP, int IP, int LP, int BP,
        int size, int op, int parm1)
{
    if (op == PRINTS_CALL)
    {
        asm(24, parm1);     // OUTS_OP
    } else {
        asm(29);            // TRAPTOC_OP
        asm(18);            // POP_OP
    }
    asm(31);                // RTI_OP
}

int syscall(int size, int op, int parm1)
{
    if (op == PRINTS_CALL)
    {
        asm(30);            // TRAP_OP
    } else {
        asm(29);            // TRAPTOC_OP
        asm(18);            // POP_OP
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

