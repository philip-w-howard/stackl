#include <syscodes.h>
#include <machine_def.h>
#include <pio_term.h>

char *pio_t_rdr;
char *pio_t_xdr;
char *pio_t_ier;
char *pio_t_iir;

#define RING_BUFF_SIZE  256
#define BELL     7
char *Input_Ring;
int  Input_Head;
int  Input_Tail;
int  Input_Have_Line;

int check_buff()
{
    if (Input_Ring[Input_Head] == 'x') asm("HALT");
}

int Input_Queue_Full()
{
    if (Input_Head == Input_Tail) return 1;
    return 0;
}

int Input_Queue_Empty()
{
    if (Input_Head+1 == Input_Tail) return 1;
    if (Input_Tail == 0 && Input_Head == RING_BUFF_SIZE-1) return 1;
    return 0;
}

int test_pio_term()
{
    int status;
    char byte;

    status = *pio_t_iir;
    if (status & PIO_T_IID_INT)
    {
        if (status & PIO_T_IID_RECV) 
        {
            byte = *pio_t_rdr;
            if (!Input_Queue_Full())
            {
                Input_Ring[Input_Tail] = byte;
                *pio_t_xdr = byte;
                Input_Tail++;
                if (Input_Tail >= RING_BUFF_SIZE) Input_Tail = 0;
            } else {
                *pio_t_xdr = BELL;
            }
            if (byte =='\n') Input_Have_Line++;
        }
    }
}

int interrupt()
{
    test_pio_term();
    asm("RTI");
}

int do_out(char *param)
{
    int status;
    while (*param)
    {
        status = *pio_t_iir;
        while ( (status & PIO_T_IID_XMIT) == 0)
        {
            status = *pio_t_iir;
        }
        pio_t_xdr[0] = *param;
        param++;
    }
    return 0;
}
int systrap(int size, int op, int parm1, int parm2)
{
    if (op == PRINTS_CALL)
    {
        do_out(parm1);
    } else if (op == EXIT_CALL) {
        asm("HALT");
    }

    asm("RTI");
}

int syscall(int size, int op, int parm1, int parm2)
{
    asm("TRAP");

    return 0;
}

int exit(int status)
{
    syscall(3, EXIT_CALL, status);
    return 0;
}

int getline(char *buff)
{
    int index;
    index = 0;

    while (Input_Queue_Empty())
    {}
    Input_Head++;
    if (Input_Head >= RING_BUFF_SIZE) Input_Head = 0;

    buff[index] = Input_Ring[Input_Head];
    while (buff[index] != '\n')
    {
        index++;

        while (Input_Queue_Empty())
        {}
        Input_Head++;
        if (Input_Head >= RING_BUFF_SIZE) Input_Head = 0;

        buff[index] = Input_Ring[Input_Head];
        buff[index+1] = 0;
    }

    buff[index+1] = 0;
    Input_Have_Line--;
    return 0;
}

int startup__()
{
    char ring_buff[RING_BUFF_SIZE];
    Input_Ring = ring_buff;
    char buff[80];

    Input_Tail = 1;
    Input_Head = 0;
    Input_Have_Line = 0;

    // define registers
    pio_t_rdr = PIO_T_RDR;
    pio_t_xdr = PIO_T_XDR;
    pio_t_ier = PIO_T_IER;
    pio_t_iir = PIO_T_IIR;

    // Inable recv interrupts
    *pio_t_ier = PIO_T_IE_RECV;

    syscall(3, PRINTS_CALL, "this is an OS test\n");

    int ii;
    for (ii=0; ii<3; ii++)
    {
        while (!Input_Have_Line)
        {}
        getline(buff);
        syscall(3, PRINTS_CALL, buff);
    }

    syscall(3, PRINTS_CALL, "Testing exit\n");
    exit(0);
    syscall(3, PRINTS_CALL, "exit() did not work\n");
}

