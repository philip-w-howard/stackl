#include "interp/system.h"

int main();

int syscall(int size, int op)
{
    trap();
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

