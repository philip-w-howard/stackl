#include "interp/system.h"

int syscall(int size, int op)
{
    trap();
}

int exit(int status)
{
    syscall(3, EXIT_CALL, status);
    return 0;
}

int printi(int val)
{
    syscall(3, PRINTI_CALL, val);
    return 0;
}

int prints(char *string)
{
    syscall(3, PRINTS_CALL, string);
    return 0;
}

int printc(int string)
{
    syscall(3, PRINTC_CALL, string);
    return 0;
}

int scans(char *string)
{
    syscall(3, GETS_CALL, string);
    return 0;
}

int scanl(char *string)
{
    syscall(3, GETL_CALL, string);
    return 0;
}

int scani(int string)
{
    syscall(3, GETI_CALL, string);
    return 0;
}

int exec(char *file)
{
    syscall(3, EXEC_CALL, file);
    return 0;
}
