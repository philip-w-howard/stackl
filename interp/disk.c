#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

#include "disk.h"
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
static int32_t Block;

static int Disk_fd = -1;
static const char *Disk_Filename = "stackl.disk";
static int Num_Disk_Blocks = -1;

//*************************************
static void init_disk()
{
    off_t offset;

    if (Disk_fd == -1)
    {
        Disk_fd = open(Disk_Filename, O_RDWR | O_SYNC);
        if (Disk_fd < 0) Machine_Check("Failed to open disk drive");

        offset = lseek(Disk_fd, 0, SEEK_END);
        if (offset < 0) Machine_Check("Unformatted disk");

        Num_Disk_Blocks = offset / DISK_BLOCK_SIZE;
    }
}
//*************************************
static void close_disk()
{
    int status;

    status = close(Disk_fd);
    if (status != 0) Machine_Check("Failed to shut down disk drive");
}
//*************************************
static int start_disk_read()
{
    char *buffer;
    int error = 0;
    int status;

    if (Command & DISK_CMD_START_READ)
    {
        Command &= ~DISK_CMD_START_READ;
        Status &= ~(DISK_STATUS_READ_DONE | DISK_STATUS_READ_ERROR);
        Status |= DISK_STATUS_READ_BUSY;

        buffer = (char *)Abs_Get_Addr(Address);
        if (buffer == NULL) Machine_Check("DISK_T read to invalid address");
        pthread_mutex_unlock(&IO_Q_Lock);

        status = lseek(Disk_fd, Block*DISK_BLOCK_SIZE, SEEK_SET);
        if (status < 0)
            error = 1;
        else
        {
            status = read(Disk_fd, buffer, DISK_BLOCK_SIZE);
            if (status != DISK_BLOCK_SIZE) error = 1;
        }

        pthread_mutex_lock(&IO_Q_Lock);
        if (error) Status |= DISK_STATUS_READ_ERROR;
        Status &= ~DISK_STATUS_READ_BUSY;
        Status |= DISK_STATUS_READ_DONE | DISK_STATUS_ATTN;

        if (Command & DISK_CMD_INT_ENA) Machine_Signal_Interrupt(1);
    }

    return error;
}
//*************************************
static int start_disk_write()
{
    char *buffer;
    int error = 0;
    int status;

    if (Command & DISK_CMD_START_WRITE)
    {
        Command &= ~DISK_CMD_START_WRITE;
        Status &= ~(DISK_STATUS_WRITE_DONE | DISK_STATUS_WRITE_ERROR);
        Status |= DISK_STATUS_WRITE_BUSY;

        buffer = (char *)Abs_Get_Addr(Address);
        if (buffer == NULL) Machine_Check("DISK_T write from invalid address");

        pthread_mutex_unlock(&IO_Q_Lock);

        status = lseek(Disk_fd, Block*DISK_BLOCK_SIZE, SEEK_SET);
        if (status == -1) 
            error = 1;
        else
        {
            status = write(Disk_fd, buffer, DISK_BLOCK_SIZE);
            if (status != DISK_BLOCK_SIZE) error = 1;
        }

        pthread_mutex_lock(&IO_Q_Lock);
        if (error) Status |= DISK_STATUS_WRITE_ERROR;
        Status &= ~DISK_STATUS_WRITE_BUSY;
        Status |= DISK_STATUS_WRITE_DONE | DISK_STATUS_ATTN;

        if (Command & DISK_CMD_INT_ENA) Machine_Signal_Interrupt(1);
    }

    return error;
}
//*************************************
static void *disk_device(void *arg)
{
    while (IO_Q_Halt_Thread == 0)
    {
        pthread_mutex_lock(&IO_Q_Lock);
        while ( !IO_Q_Halt_Thread && 
                (Command & (DISK_CMD_START_READ | DISK_CMD_START_WRITE)) == 0)
        {
            pthread_cond_wait(&IO_Q_Cond, &IO_Q_Lock);
        }

        if (Block >= Num_Disk_Blocks)
        {
            Status |= DISK_STATUS_BAD_BLOCK;
            continue;
            // <<<<<<----------
        }

        if (Command & DISK_CMD_START_READ)
        {
            start_disk_read();
        }

        if (Command & DISK_CMD_START_WRITE)
        {
            start_disk_write();
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
        value = Block;
    
    pthread_mutex_unlock(&IO_Q_Lock);

    return value;
}
//***********************************
static int32_t get_byte(int32_t id, int32_t addr)
{
    Machine_Check("Disk: illegal byte register access");
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
        pthread_cond_signal(&IO_Q_Cond);
    }
    else if (addr == 8)
        Address = value;
    else if (addr == 12)
        Block = value;

    pthread_mutex_unlock(&IO_Q_Lock);
}
//***********************************
static void set_byte(int32_t id, int32_t addr, int32_t value)
{
    Machine_Check("Disk: illegal byte register access");
}
//*************************************
static void Disk_Finish()
{
    IO_Q_Halt_Thread = 1;
    pthread_cond_signal(&IO_Q_Cond);
    pthread_join(IO_Q_Thread, NULL);

    close_disk();
}
//*************************************
int Disk_Init()
{
    IO_Q_Halt_Thread = 0;

    init_disk();

    pthread_create(&IO_Q_Thread, NULL, disk_device, NULL);

    IO_Register_Handler(0, DISK_STATUS, 16,
            get_word, get_byte, set_word, set_byte);

    atexit(Disk_Finish);

    return 0;
}
//*************************************
int Disk_Load_Boot_Block()
{
    int status;
    void *buffer;

    buffer = Abs_Get_Addr(0);

    lseek(Disk_fd, 0, SEEK_SET);
    status = read(Disk_fd, buffer, DISK_BLOCK_SIZE);
    if (status != DISK_BLOCK_SIZE) return 1;

    return 0;
}
