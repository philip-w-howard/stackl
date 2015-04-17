int syscall(int size, int op, int parm1);
#include <syscodes.h>
#include <opcodes.h>
#include <string.h>
#include <system.h>

int main();

int interrupt()
{
    prints("Interrupt\n");
    asm(31);        // RTI_OP
}

int systrap(// int SP, int FLAG, int FP, int IP, int LP, int BP,
        int size, int op, int parm1)
{
    if (op == PRINTS_CALL)
    {
        asm(OUTS_OP, parm1);
    } else if (op == EXIT_CALL) {
        asm(POPREG_OP, 1); // halt state
        asm(FLAG_REG);
    } else {
        asm(TRAPTOC_OP);
        asm(POP_OP);
    }
    asm(RTI_OP);
}

int syscall(int size, int op, int parm1)
{
    struct
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
        asm(INP_OP, &io_blk);
        while(io_blk.status != 4)
        {
            asm(NOP);
        }
    } else {
        asm(TRAP_OP);
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
