int prints(char *msg)
{
    asm("OUTS", msg);
    return 0;
}

int printi(int val)
{
    char buff[20];
    itostr(val, buff);
    prints(buff);
    return 0;
}

