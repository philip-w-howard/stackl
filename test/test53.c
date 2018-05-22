//***********************************
// Test the user mode machine check operation
//
#include <machine_def.h>
#include <string.h>

typedef struct
{
    int sp;
    int flag;
    int bp;
    int lp;
    int ip;
    int fp;
} isr_call_frame_t;

isr_call_frame_t *g_isr_call_frame;

void machine_check(isr_call_frame_t *frame)
{
    int reason;
    char buff[30];

    itostr(frame->ip, buff);

    reason = asm2("PUSHREG", FLAG_REG);
    reason &= FL_MC_MASK;
    reason >>= FL_MC_SHIFT;
    if (reason & MC_ILLEGAL_INST)
    {
        asm("OUTS", "Machine Check: Illegal instruction at ");
        asm("OUTS", buff);
        asm("OUTS", "\n");
    }
    else if (reason & MC_ILLEGAL_ADDR)
    {
        asm("OUTS", "Machine Check: Illegal address at ");
        asm("OUTS", buff);
        asm("OUTS", "\n");
    }
    else if (reason & MC_PROT_INST)
    {
        asm("OUTS", "Machine Check: Protected instruction at ");
        asm("OUTS", buff);
        asm("OUTS", "\n");
    }
    else
    {
        asm("OUTS", "Machine Check: Unknown failure at ");
        asm("OUTS", buff);
        asm("OUTS", "\n");
    }
}

#pragma interrupt interrupt
void interrupt()
{
    g_isr_call_frame = asm2("PUSHREG", SP_REG);
    g_isr_call_frame--;

    machine_check(g_isr_call_frame);
    asm("HALT");
}

void main()
{
    asm("OUTS", "System is starting up\n");
    asm2("POPREG", FLAG_REG, FL_USER_MODE);
    asm("OUTS", "This should not print\n");
    asm("HALT");
}

