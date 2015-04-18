#include <syscodes.h>
#include <opcodes.h>

int main();

int interrupt()
{
    asm(RTI_OP);
}

int do_inp(int op, int param)
{
    struct
    {
        int op;
        int addr;
        int status;
    } io_blk_t;
    io_blk_t io_blk;

    io_blk.op = op;
    io_blk.addr = param;
    io_blk.status = 0;
    asm(INP_OP, &io_blk);
    while(io_blk.status != 4)
    {
        asm(NOP);
    }

    return 0;
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
    } else if (op == GETL_CALL || op == GETS_CALL || op == GETI_CALL) {
        do_inp(op, parm1);
    //} else {
    //    asm(TRAPTOC_OP);
    //    asm(POP_OP);
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

