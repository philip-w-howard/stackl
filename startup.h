#include <syscodes.h>
#include <opcodes.h>

int main();

int interrupt()
{
    asm(31);        // RTI_OP
}

int systrap()
{
    asm(29);        // TRAPTOC_OP
    asm(18);        // POP_OP
    asm(31);        // RTI_OP
}

int syscall(int size, int op)
{
    int result;
    result = asm(29);  // TRAPTOC_OP
    return result;
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

