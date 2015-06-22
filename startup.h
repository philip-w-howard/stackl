#include <machine_def.h>

int main();

int interrupt() {}
int systrap()   {}
int startup__()
{
    main();
    asm(HALT_OP);
}

int sys_prints(char *msg)
{
    asm(OUTS_OP, msg);
    return 0;
}
