int syscall(int size, int op)
{
    trap();
}

int exit(int status)
{
    syscall(3, 1, status);
    return 0;
}

int printi(int val)
{
    syscall(3, 2, val);
    return 0;
}

int prints(char *string)
{
    syscall(3, 3, string);
    return 0;
}

int scans(char *string)
{
    syscall(3, 4, string);
    return 0;
}

int scanl(char *string)
{
    syscall(3, 5, string);
    return 0;
}
// #define GETI_CALL       6

