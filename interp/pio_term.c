#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "pio_term.h"
#include "machine.h"
#include "io_handler.h"

static pthread_mutex_t IO_Q_Lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  IO_Q_Cond = PTHREAD_COND_INITIALIZER;

static volatile int IO_Q_Halt_Thread = 0;
static pthread_t IO_Q_Thread;

static volatile int RDR;
static volatile int XDR;
static volatile int IER;
static volatile int IIR;
static volatile int XDR_Written;

//************************************************
// Enable/Disable nonblocking mode
// Code from:
// http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
static void set_nonblock(int nonblock)
{
    struct termios ttystate;
         
    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    if (nonblock)
    {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    else
    {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}
//********************************************
// Check to see if a character is available on stdin
// Code from:
// http://cc.byexamples.com/2007/04/08/non-blocking-user-input-in-loop-without-ncurses/
static int kbhit()
{
    int hit;
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    hit = FD_ISSET(STDIN_FILENO, &fds);
    return hit;
}
//*************************************
static void check_kb()
{
    if (kbhit()) 
    {
        pthread_mutex_lock(&IO_Q_Lock);
        IIR |= PIO_T_IID_RECV | PIO_T_IID_INT;
        RDR = fgetc(stdin);
        pthread_mutex_unlock(&IO_Q_Lock);

        if (IER & PIO_T_IE_RECV) Machine_Signal_Interrupt(1);
    }
}
//*************************************
static void *terminal_device(void *arg)
{
    while (IO_Q_Halt_Thread == 0)
    {
        pthread_mutex_lock(&IO_Q_Lock);
        while ( !IO_Q_Halt_Thread && XDR_Written == 0)
        {
            pthread_cond_wait(&IO_Q_Cond, &IO_Q_Lock);
        }

        if (XDR_Written)
        {
            pthread_mutex_unlock(&IO_Q_Lock);
            usleep(10);       // ~100000 baud
            fputc(XDR, stdout);
            fflush(stdout);
            pthread_mutex_lock(&IO_Q_Lock);

            XDR_Written = 0;
            IIR |= PIO_T_IID_XMIT | PIO_T_IID_INT;
            pthread_mutex_unlock(&IO_Q_Lock);
            if (IER & PIO_T_IE_XMIT) Machine_Signal_Interrupt(1);
        } else {
            pthread_mutex_unlock(&IO_Q_Lock);
        }

        check_kb();
    }
    return NULL;
}
//***********************************
static int get_byte(int id, int addr)
{
    int value;

    check_kb();

    pthread_mutex_lock(&IO_Q_Lock);
    if (addr == 0) 
        value = RDR;
    else if (addr == 1) 
        value = IER;
    else if (addr == 2)
        value = IIR;

    pthread_mutex_unlock(&IO_Q_Lock);

    //printf("pio_term get_byte: %X %X\n", addr, value);

    return value;
}
//***********************************
static int get_word(int id, int addr)
{
    Machine_Check("pio_term registers are byte wide");
    return 0;
}
//***********************************
static void set_byte(int id, int addr, int value)
{
    check_kb();

    pthread_mutex_lock(&IO_Q_Lock);

    //printf("pio_term set byte: %X %X\n", addr, value);

    if (addr == 0) 
    {
        XDR = value;
        XDR_Written = 1;
        pthread_cond_signal(&IO_Q_Cond);
    }
    else if (addr == 1) 
    {
        IER = value;
    }
    else if (addr == 2)
    {
        IIR = value;
    }

    pthread_mutex_unlock(&IO_Q_Lock);
}
//***********************************
static void set_word(int id, int addr, int value)
{
    Machine_Check("pio_term registers are byte wide");
}
//*************************************
int PIO_T_Init()
{
    set_nonblock(1);

    IO_Q_Halt_Thread = 0;
    XDR_Written = 0;
    IIR = PIO_T_IID_XMIT;

    pthread_create(&IO_Q_Thread, NULL, terminal_device, NULL);

    IO_Register_Handler(0, PIO_T_RDR, 8,
            get_word, get_byte, set_word, set_byte);

    return 0;
}
//*************************************
int PIO_T_Finish()
{
    IO_Q_Halt_Thread = 1;
    pthread_cond_signal(&IO_Q_Cond);
    pthread_join(IO_Q_Thread, NULL);
    set_nonblock(0);

    return 0;
}
