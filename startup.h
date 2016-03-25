//#include <opcode_defs.h>

int main();

int interrupt() {}
int systrap()   {}
int startup__()
{
    main();
    //asm(HALT_OP);
    asm("HALT");
}

int sys_prints(char *msg)
{
    //asm(OUTS_OP, msg);
    asm("OUTS", msg);
    return 0;
}
