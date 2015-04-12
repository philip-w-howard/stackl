int prints(char *string)
{
    syscall(3, PRINTS_CALL, string);
    return 0;
}

int printi(int val)
{
    char buff[16];
    itostr(val, buff);
    prints(buff);
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

int fork()
{
    return syscall(2, FORK_CALL);
}

int wait(int process)
{
    syscall(3, WAIT_CALL, process);
    return 0;
}
