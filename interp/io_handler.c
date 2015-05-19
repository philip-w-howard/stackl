#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "machine_def.h"
#include "syscodes.h"
#include "vmem.h"
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

typedef struct handler_s
{
    int id;
    int addr;
    int top;
    int (*get_word)(int id, int addr);
    int (*get_byte)(int id, int addr);
    void (*set_word)(int id, int addr, int value);
    void (*set_byte)(int id, int addr, int value);
    struct handler_s *next;
} handler_t;

handler_t *Handler_List;

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

            io_op = Abs_Get_Word(io_blk_addr);
            io_op &= ~IO_FLAGS;

            addr = Abs_Get_Addr( Abs_Get_Word(io_blk_addr + WORD_SIZE));
            Abs_Set_Word(io_blk_addr, io_op | IO_PENDING);

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
                    Abs_Set_Word(io_blk_addr + 2*WORD_SIZE, status);
                    break;
                default:
                    fprintf(stderr, "Invalid IO code: %d\n", io_op);
                    fprintf(stderr, "System halting\n");
                    exit(-1);
                    break;
            }

            io_op |= IO_COMPLETE;
            Abs_Set_Word(io_blk_addr, io_op);
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

    Handler_List = (handler_t *)malloc(sizeof(handler_t));
    assert(Handler_List != NULL);
    Handler_List->next = NULL;
}
//*************************************
void Finish_IO()
{
    IO_Q_Halt_Thread = 1;
    pthread_cond_signal(&IO_Q_Cond);
    pthread_join(IO_Q_Thread, NULL);
}
//*************************************
void Schedule_IO(Machine_State *cpu, int io_blk_addr)
{
    int op;

    // NOTE: Ignoring queue overflow
    pthread_mutex_lock(&IO_Q_Lock);

    IO_Q[IO_Q_Head].io_blk_addr = io_blk_addr;

    op = Get_Word(cpu, io_blk_addr);
    op &= ~IO_FLAGS;
    op |= IO_QUEUED;
    Set_Word(cpu, io_blk_addr, op);

    IO_Q_Head++;
    IO_Q_Head %= IO_Q_SIZE;

    pthread_mutex_unlock(&IO_Q_Lock);
    pthread_cond_signal(&IO_Q_Cond);
}

//*************************************
static handler_t *get_handler(int addr)
{
    handler_t *handler;

    handler = Handler_List->next;
    while (handler != NULL)
    {
        if (addr >= handler->addr && addr < handler->top) return handler;
        handler = handler->next;
    }

    return handler;
}
//*************************************
int IO_Get_Word(int address)
{
    handler_t *handler = get_handler(address);
    if (handler != NULL)
        return handler->get_word(handler->id, address - handler->addr);
    else
        return 0xFFFFFFFF;
}
//*************************************
void IO_Set_Word(int address, int value)
{
    handler_t *handler = get_handler(address);
    if (handler != NULL) 
    {
        handler->set_word(handler->id, address - handler->addr, value);
    }
}
//*************************************
int IO_Get_Byte(int address)
{
    handler_t *handler = get_handler(address);
    if (handler != NULL)
        return handler->get_byte(handler->id, address - handler->addr);
    else
        return 0xFF;
}
//*************************************
void IO_Set_Byte(int address, int value)
{
    handler_t *handler = get_handler(address);
    if (handler != NULL) 
    {
        handler->set_byte(handler->id, address - handler->addr, value);
    }
}
//*************************************
int IO_Register_Handler(int id, int addr, int size, 
        int (*get_word)(int id, int addr),
        int (*get_byte)(int id, int addr),
        void (*set_word)(int id, int addr, int value),
        void (*set_byte)(int id, int addr, int value)
        )
{
    handler_t *handler = (handler_t *)malloc(sizeof(handler_t));
    assert(handler != NULL);

    handler->id   = id;;
    handler->addr = addr;;
    handler->top  = addr + size;;
    handler->get_word = get_word;
    handler->get_byte = get_byte;
    handler->set_word = set_word;
    handler->set_byte = set_byte;
    handler->next = Handler_List->next;
    Handler_List->next = handler;

    return addr;
}

