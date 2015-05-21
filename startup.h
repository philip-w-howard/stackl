#include <syscodes.h>
#include <machine_def.h>
#include <dma_term.h>

int main();

int interrupt()
{
    asm(RTI_OP);
}

int do_inp(int op, int param)
{
    typedef struct
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
    while(io_blk.op >= 0)
    {
        asm(NOP);
    }

    return 0;
}

int do_inp2(int op, int parm1, int parm2)
{
    int status;
    status = 0;
    asm(POPVARIND_OP, DMA_T_ADDR, parm1);
    asm(POPVARIND_OP, DMA_T_SIZE, parm2);
    asm(POPVARIND_OP, DMA_T_CMD, DMA_T_CMD_START_READ);
    while(status >= 0)
    {
        status = asm(PUSHVARIND_OP, DMA_T_STATUS);
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
    } else if (op == GETL_CALL) {
        do_inp2(op, parm1);
    } else if (op == GETS_CALL || op == GETI_CALL) {
        do_inp(op, parm1);
    //} else {
    //    asm(TRAPTOC_OP);
    //    asm(POP_OP);
    }
    asm(RTI_OP);
}

int syscall(int size, int op, int parm1)
{
    int mode;
    asm(PUSHREG_OP);
    asm(FLAG_REG);
    mode = asm(NOP);

    if (mode == 2)  // user mode
    {
        asm(PUSHREG_OP);
        asm(BP_REG);
        parm1 = asm(PLUS_OP, parm1);
    }

    asm(TRAP_OP);

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

