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

static volatile int Status;
static volatile int Command;
static volatile int Data;

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
static void *terminal_device(void *arg)
{
    while (IO_Q_Halt_Thread == 0)
    {
        pthread_mutex_lock(&IO_Q_Lock);
        while ( !IO_Q_Halt_Thread && Data == 0)
        {
            pthread_cond_wait(&IO_Q_Cond, &IO_Q_Lock);
        }

        if (Data != 0)
        {
            fputc(Data, stdout);
            fflush(stdout);
            Status &= ~PIO_T_STATUS_WRITE_BUSY;
            Status |= PIO_T_STATUS_WRITE_DONE | PIO_T_STATUS_ATTN;
            Data = 0;
            pthread_mutex_unlock(&IO_Q_Lock);
            if (Command & PIO_T_CMD_INT_ENA) Machine_Signal_Interrupt(1);
        } else {
            pthread_mutex_unlock(&IO_Q_Lock);
        }
    }
    return NULL;
}
//***********************************
static int get_byte(int id, int addr)
{
    int value;

    pthread_mutex_lock(&IO_Q_Lock);

    if (addr == 0) 
    {
        if (kbhit()) Status |= PIO_T_STATUS_READ_DONE;
        value = Status;
        if (value & 0x0080) value |= 0xFFFFFF00;
        Status = 0;
    }
    else if (addr == 4) 
        value = Command;
    else if (addr == 8)
    {
        //value = Data;
        if (kbhit()) 
        {
            Data = fgetc(stdin);
            Status &= ~PIO_T_STATUS_READ_DONE;
        }
        value = Data;
    } 
    pthread_mutex_unlock(&IO_Q_Lock);

    //printf("pio_term get_byte: %X %X\n", addr, value);

    return value;
}
//***********************************
static int get_word(int id, int addr)
{
    return get_byte(id, addr);
}
//***********************************
static void set_byte(int id, int addr, int value)
{
    pthread_mutex_lock(&IO_Q_Lock);

    //printf("pio_term set byte: %X %X\n", addr, value);

    if (addr == 0) 
        Status = 0;
    else if (addr == 4) 
    {
        Command = value;
    }
    else if (addr == 8)
    {
        Status |= PIO_T_STATUS_WRITE_BUSY;
        Data = value;
        pthread_cond_signal(&IO_Q_Cond);
    }

    pthread_mutex_unlock(&IO_Q_Lock);
}
//***********************************
static void set_word(int id, int addr, int value)
{
    set_byte(id, addr, value);
}
//*************************************
int PIO_T_Init()
{
    set_nonblock(1);

    IO_Q_Halt_Thread = 0;

    pthread_create(&IO_Q_Thread, NULL, terminal_device, NULL);

    IO_Register_Handler(0, PIO_T_STATUS, 16,
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
