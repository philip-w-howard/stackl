#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "syscodes.h"
#include "io_handler.h"
#include "loader.h"

#define IO_Q_SIZE     10      // Maximum number of queued IO requests

static pthread_mutex_t IO_Q_Lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  IO_Q_Cond = PTHREAD_COND_INITIALIZER;

typedef struct
{
    int io_blk_addr;
} io_op_t;

static io_op_t IO_Q[IO_Q_SIZE];
static int IO_Q_Head = 0;
static int IO_Q_Tail = 0;
static volatile int IO_Q_Halt_Thread = 0;
static pthread_t IO_Q_Thread;

//*************************************
static void *IO_Processor(void *arg)
{
    int io_blk_addr;
    int io_op;
    int status;
    void *addr;

    while (IO_Q_Halt_Thread == 0)
    {
        pthread_mutex_lock(&IO_Q_Lock);
        while ( (IO_Q_Head == IO_Q_Tail) && !IO_Q_Halt_Thread)
        {
            // Q is empty
            pthread_cond_wait(&IO_Q_Cond, &IO_Q_Lock);
        }

        if (IO_Q_Head != IO_Q_Tail)
        {
            status = 0;

            // alread have the lock from above
            // pthread_mutex_lock(&IO_Q_Lock);
            io_blk_addr = IO_Q[IO_Q_Tail].io_blk_addr;

            IO_Q_Tail++;
            IO_Q_Tail %= IO_Q_SIZE;

            pthread_mutex_unlock(&IO_Q_Lock);

            io_op = Get_Word(io_blk_addr);
            io_op &= ~IO_FLAGS;

            addr = Get_Addr( Get_Word(io_blk_addr + WORD_SIZE));
            Set_Word(io_blk_addr, io_op | IO_PENDING);

            switch (io_op)
            {
                case GETL_CALL:
                    gets((char *)addr);
                    break;
                case GETS_CALL:
                    scanf("%s", (char *)addr);
                    break;
                case GETI_CALL:
                    scanf("%d", (int *)addr);
                    break;
                case EXEC_CALL:
                    status = Load( (char *)addr );
                    if (status == 0) io_op |= IO_ERROR;
                    Set_Word(io_blk_addr + 2*WORD_SIZE, status);
                    break;
                default:
                    fprintf(stderr, "Invalid IO code: %d\n", io_op);
                    fprintf(stderr, "System halting\n");
                    exit(-1);
                    break;
            }

            io_op |= IO_COMPLETE;
            Set_Word(io_blk_addr, io_op);
        } else {
            // No data to grab, just release the lock
            pthread_mutex_unlock(&IO_Q_Lock);
        }


    }
    return NULL;
}
//*************************************
void Init_IO()
{
    IO_Q_Head = 0;
    IO_Q_Tail = 0;
    IO_Q_Halt_Thread = 0;

    pthread_create(&IO_Q_Thread, NULL, IO_Processor, NULL);

}
//*************************************
void Finish_IO()
{
    IO_Q_Halt_Thread = 1;
    pthread_cond_signal(&IO_Q_Cond);
    pthread_join(IO_Q_Thread, NULL);
}
//*************************************
void Schedule_IO(int io_blk_addr)
{
    int op;

    // NOTE: Ignoring queue overflow
    pthread_mutex_lock(&IO_Q_Lock);

    IO_Q[IO_Q_Head].io_blk_addr = io_blk_addr;

    op = Get_Word(io_blk_addr);
    op &= ~IO_FLAGS;
    op |= IO_QUEUED;
    Set_Word(io_blk_addr, op);

    IO_Q_Head++;
    IO_Q_Head %= IO_Q_SIZE;

    pthread_mutex_unlock(&IO_Q_Lock);
    pthread_cond_signal(&IO_Q_Cond);
}
