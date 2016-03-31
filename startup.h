// Default startup code for stacklc compiler

int main();

int interrupt() {}
int systrap()   {}
int startup__()
{
    main();
    asm("HALT");
}

int sys_prints(char *msg)
{
    asm("OUTS", msg);
    return 0;
}
