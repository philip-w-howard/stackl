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

int printc(int string)
{
    syscall(3, 4, string);
    return 0;
}

int scans(char *string)
{
    syscall(3, 5, string);
    return 0;
}

int scanl(char *string)
{
    syscall(3, 6, string);
    return 0;
}

int scani(int string)
{
    syscall(3, 7, string);
    return 0;
}

int exec(char *file)
{
    syscall(3, 8, file);
    return 0;
}
