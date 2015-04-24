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

static int Status;
static int Command;
static int Data;

//*************************************
static void *terminal_device(void *arg)
{
    while (IO_Q_Halt_Thread == 0)
    {
        pthread_mutex_lock(&IO_Q_Lock);
        while ( !IO_Q_Halt_Thread && (Command & PIO_T_CMD_START_READ) == 0)
        {
            pthread_cond_wait(&IO_Q_Cond, &IO_Q_Lock);
        }

        if (Command & PIO_T_CMD_START_READ)
        {
            /*
            Command &= ~PIO_T_CMD_START_READ;
            Status &= ~(PIO_T_STATUS_READ_DONE | PIO_T_STATUS_READ_ERROR);
            Status |= PIO_T_STATUS_READ_BUSY;

            size = Size;
            buffer = (char *)Get_Addr(Address);
            if (buffer == NULL) Machine_Check("PIO_T read to invalid address");

            pthread_mutex_unlock(&IO_Q_Lock);
            buffer = fgets(buffer, size, stdin);

            pthread_mutex_lock(&IO_Q_Lock);
            Status &= ~PIO_T_STATUS_READ_BUSY;
            Status |= PIO_T_STATUS_READ_DONE | PIO_T_STATUS_ATTN;
            if (buffer == NULL) Status |= PIO_T_STATUS_READ_ERROR;

            if (Command & PIO_T_CMD_INT_ENA) Machine_Signal_Interrupt();
            */
        }
        pthread_mutex_unlock(&IO_Q_Lock);
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
        value = Status;
        if (value & 0x0080) value |= 0xFFFFFF00;
        Status = 0;
    }
    else if (addr == 4) 
        value = Command;
    else if (addr == 8)
        value = Data;
    
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
        pthread_cond_signal(&IO_Q_Cond);
    }
    else if (addr == 8)
    {
        Status |= PIO_T_STATUS_WRITE_BUSY;
        Status &= PIO_T_STATUS_WRITE_DONE;
        fputc(value, stdout);
        Status &= ~PIO_T_STATUS_WRITE_BUSY;
        Status |= PIO_T_STATUS_WRITE_DONE | PIO_T_STATUS_ATTN;
        if (Command & PIO_T_CMD_INT_ENA) Machine_Signal_Interrupt();
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

    return 0;
}
