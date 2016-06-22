#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

#include "dma_term.h"
#include "machine.h"
#include "vmem.h"
#include "io_handler.h"

static pthread_mutex_t IO_Q_Lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  IO_Q_Cond = PTHREAD_COND_INITIALIZER;

static volatile int IO_Q_Halt_Thread = 0;
static pthread_t IO_Q_Thread;

static int32_t Status;
static int32_t Command;
static int32_t Address;
static int32_t Size;

//*************************************
static void *terminal_device(void *arg)
{
    int size;
    char *buffer;

    while (IO_Q_Halt_Thread == 0)
    {
        pthread_mutex_lock(&IO_Q_Lock);
        while ( !IO_Q_Halt_Thread && (Command & DMA_T_CMD_START_READ) == 0)
        {
            pthread_cond_wait(&IO_Q_Cond, &IO_Q_Lock);
        }

        if (Command & DMA_T_CMD_START_READ)
        {
            Command &= ~DMA_T_CMD_START_READ;
            Status &= ~(DMA_T_STATUS_READ_DONE | DMA_T_STATUS_READ_ERROR);
            Status |= DMA_T_STATUS_READ_BUSY;

            size = Size;
            buffer = (char *)Abs_Get_Addr(Address);
            if (buffer == NULL) Machine_Check("DMA_T read to invalid address");

            pthread_mutex_unlock(&IO_Q_Lock);
            buffer = fgets(buffer, size, stdin);

            pthread_mutex_lock(&IO_Q_Lock);
            Status &= ~DMA_T_STATUS_READ_BUSY;
            Status |= DMA_T_STATUS_READ_DONE | DMA_T_STATUS_ATTN;
            if (buffer == NULL) Status |= DMA_T_STATUS_READ_ERROR;

            if (Command & DMA_T_CMD_INT_ENA) 
            {
                Machine_Signal_Interrupt(1, DMA_T_VECTOR);
            }
        }
        pthread_mutex_unlock(&IO_Q_Lock);
    }
    return NULL;
}
//***********************************
static int32_t get_word(int32_t id, int32_t addr)
{
    int32_t value;

    pthread_mutex_lock(&IO_Q_Lock);

    if (addr == 0) 
    {
        value = Status;
        Status = 0;
    }
    else if (addr == 4) 
        value = Command;
    else if (addr == 8)
        value = Address;
    else
        value = Size;
    
    pthread_mutex_unlock(&IO_Q_Lock);

    return value;
}
//***********************************
static int32_t get_byte(int32_t id, int32_t addr)
{
    return 0xFFFFFFFF;
}
//***********************************
static void set_word(int32_t id, int32_t addr, int32_t value)
{
    pthread_mutex_lock(&IO_Q_Lock);

    if (addr == 0) 
        Status = 0;
    else if (addr == 4) 
    {
        Command = value;
        if (Command & DMA_T_CMD_START_WRITE)
        {
            char *buffer = Abs_Get_Addr(Address);
            if (buffer == NULL) Machine_Check("DMA_T write from invalid address");
            printf("%s", buffer);
            Status &= ~(DMA_T_STATUS_WRITE_BUSY | DMA_T_STATUS_WRITE_ERROR);
            Status |= DMA_T_STATUS_WRITE_DONE | DMA_T_STATUS_ATTN;
            if (Command & DMA_T_CMD_INT_ENA) 
            {
                Machine_Signal_Interrupt(0, DMA_T_VECTOR);
            }
        }

        pthread_cond_signal(&IO_Q_Cond);
    }
    else if (addr == 8)
        Address = value;
    else
        Size = value;

    pthread_mutex_unlock(&IO_Q_Lock);
}
//***********************************
static void set_byte(int32_t id, int32_t addr, int32_t value)
{
}
//*************************************
static void DMA_T_Finish()
{
    IO_Q_Halt_Thread = 1;
    pthread_cond_signal(&IO_Q_Cond);
    pthread_join(IO_Q_Thread, NULL);
}
//*************************************
int DMA_T_Init()
{
    IO_Q_Halt_Thread = 0;

    pthread_create(&IO_Q_Thread, NULL, terminal_device, NULL);

    IO_Register_Handler(0, DMA_T_STATUS, 16,
            get_word, get_byte, set_word, set_byte);

    atexit(DMA_T_Finish);
    return 0;
}
