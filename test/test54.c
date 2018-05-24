// Test the INP EXEC and PRINTS functionality
//

#pragma feature inp
#include <machine_def.h>
#include <syscodes.h>

typedef struct
{
    int op;
    char *buff;
    int status;
} io_blk_t;

//**************************************
void syscall(int op, char *buff)
{
    asm("TRAP");
}

//**************************************
void do_systrap(int op, char *buff)
{
    io_blk_t io_blk;

    if (op == 0)
    {
        asm("OUTS", "System is halting\n");
        asm("HALT");
    }
    else if (op == 1)
    {
        io_blk.op = PRINTS_CALL;
        io_blk.buff = asm2("PUSHREG", BP_REG);
        io_blk.buff += (int)buff;
        io_blk.status = 0;

        asm("INP", &io_blk);
        while (io_blk.op >= 0)
        {}
    }
}


//**************************************
#pragma systrap systrap
void systrap(int op, char *buff)
{
    do_systrap(op, buff);
    asm("RTI");
}

//**************************************
int system_startup()
{
    io_blk_t io_blk;

    int bp;
    int high_mem;
    int *stack_size;

    // Set the BP leaving enough room for our stack
    bp = asm2("PUSHREG", SP_REG);
    bp += 64;
    asm2("POPREG", BP_REG, bp);

    // Load user.slb into memory
    io_blk.op = EXEC_CALL;
    io_blk.buff = "test/test54";
    io_blk.status = 0;
    asm("INP", &io_blk);

    // change the BP to make sure the INP instruction saved it
    asm2("POPREG", BP_REG, 100000);

    while (io_blk.op >= 0)
    {
    }

    // restore the BP
    asm2("POPREG", BP_REG, bp);

    // Set the LP leaving 1000 bytes of stack space
    stack_size = io_blk.status;
    high_mem = io_blk.status + *stack_size;
    asm2("POPREG", LP_REG, high_mem);

    // Set SP and FP
    // NOTE: FP must be set LAST!
    high_mem = io_blk.status + 4 - bp;
    asm("DUP", high_mem);
    asm2("POPREG", FP_REG);
    asm2("POPREG", SP_REG);

    // Execute user.slb
    asm2("JMPUSER", 8); 
    asm("HALT");
}

//*****************************************
void user_startup()
{
    syscall(1, "This is a test\n");
    syscall(1, "This is another test\n");
    syscall(0, 0);
}

//*****************************************
#pragma startup startup__
int startup__()
{
    io_blk_t io_blk;

    int bp;
    int high_mem;
    int *stack_size;

    bp = asm2("PUSHREG", BP_REG);
    if (bp == 0)
        system_startup();
    else
        user_startup();
    while (1)
    {}
}
