#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "syscodes.h"
#include "sched.h"
#include "io_handler.h"

#define IO_Q_SIZE     10      // Maximum number of queued IO requests

static pthread_mutex_t IO_Q_Lock = PTHREAD_MUTEX_INITIALIZER;

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
    void *addr;

    while (IO_Q_Halt_Thread == 0)
    {
        if (IO_Q_Head == IO_Q_Tail)
        {
            // Q is empty
            usleep(10000);
        }
        else
        {
            pthread_mutex_lock(&IO_Q_Lock);
            io_blk_addr = IO_Q[IO_Q_Tail].io_blk_addr;

            IO_Q_Tail++;
            IO_Q_Tail %= IO_Q_SIZE;

            pthread_mutex_unlock(&IO_Q_Lock);

            io_op = Get_Word(io_blk_addr);
            addr = Get_Addr( Get_Word(io_blk_addr + WORD_SIZE));
            Set_Word(io_blk_addr + 2*WORD_SIZE, IO_PENDING);

            switch (io_op)
            {
                case GETL_CALL:
                    gets((char *)addr);
                    Set_Word(io_blk_addr + 2*WORD_SIZE, IO_COMPLETE);
                    break;
                case GETS_CALL:
                    scanf("%s", (char *)addr);
                    Set_Word(io_blk_addr + 2*WORD_SIZE, IO_COMPLETE);
                    break;
                case GETI_CALL:
                    scanf("%d", (int *)addr);
                    Set_Word(io_blk_addr + 2*WORD_SIZE, IO_COMPLETE);
                    break;
                default:
                    fprintf(stderr, "Invalid IO code: %d\n", io_op);
                    fprintf(stderr, "System halting\n");
                    exit(-1);
                    break;
            }

            // Sched_Reschedule(proc);
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
    pthread_join(IO_Q_Thread, NULL);
}
//*************************************
void Schedule_IO(int io_blk_addr)
{
    // NOTE: Ignoring queue overflow
    pthread_mutex_lock(&IO_Q_Lock);

    IO_Q[IO_Q_Head].io_blk_addr = io_blk_addr;
    Set_Word(io_blk_addr + 2*WORD_SIZE, IO_QUEUED);

    IO_Q_Head++;
    IO_Q_Head %= IO_Q_SIZE;

    pthread_mutex_unlock(&IO_Q_Lock);
}
