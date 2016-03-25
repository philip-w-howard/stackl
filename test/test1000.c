int syscall(int size, int op, int parm1);
#include <syscodes.h>
#include <machine_def.h>
#include <string.h>
#include <system.h>

int main();

int interrupt()
{
    prints("Interrupt\n");
    asm("RTI");        // RTI_OP
}

int systrap(// int SP, int FLAG, int FP, int IP, int LP, int BP,
        int size, int op, int parm1)
{
    if (op == PRINTS_CALL)
    {
        asm("OUTS", parm1);
    } else if (op == EXIT_CALL) {
        asm2("POPREG", FLAG_REG, 1); // halt state
    } else {
        asm("TRAPTOC");
        asm("POP");
    }
    asm("RTI");
}

int syscall(int size, int op, int parm1)
{
    typedef struct
    {
        int op;
        int addr;
        int status;
    } io_blk_t;
    io_blk_t io_blk;

    if (op == GETL_CALL || op == GETS_CALL || op == GETI_CALL) {
        io_blk.op = op;
        io_blk.addr = parm1;
        io_blk.status = 0;
        asm("INP", &io_blk);
        while(io_blk.status != 4)
        {
        }
    } else {
        asm("TRAP");
    }

    return 0;
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

int main()
{
    int ii;
    int count;
    count = 0;
    for (ii=0; ii<200; ii++)
    {
        count++;
    }

    prints("Done\n");
    return 0;
}
