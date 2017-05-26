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
    int32_t io_blk_addr;
    int32_t  buff;
} io_op_t;

static io_op_t IO_Q[IO_Q_SIZE];
static int IO_Q_Head = 0;
static int IO_Q_Tail = 0;
static volatile int IO_Q_Halt_Thread = 0;
static pthread_t IO_Q_Thread;

typedef struct handler_s
{
    int32_t id;
    int32_t addr;
    int32_t top;
    int32_t (*get_word)(int32_t id, int32_t addr);
    int32_t (*get_byte)(int32_t id, int32_t addr);
    void (*set_word)(int32_t id, int32_t addr, int32_t value);
    void (*set_byte)(int32_t id, int32_t addr, int32_t value);
    struct handler_s *next;
} handler_t;

static handler_t *Handler_List;

static int Allow_INP_Instr = 0;

//*************************************
static void *IO_Processor(void *arg)
{
    int32_t io_blk_addr;
    int32_t io_op;
    int32_t status;
    char *addr;

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
            addr        = Abs_Get_Addr( IO_Q[IO_Q_Tail].buff );

            IO_Q_Tail++;
            IO_Q_Tail %= IO_Q_SIZE;

            pthread_mutex_unlock(&IO_Q_Lock);

            io_op = Abs_Get_Word(io_blk_addr);
            io_op &= ~IO_FLAGS;

            Abs_Set_Word(io_blk_addr, io_op | IO_PENDING);

            if (!Allow_INP_Instr)
            {
                Machine_Check(MC_HW_WARNING | MC_ILLEGAL_INST,
                        "INP instruction not enabled: %d", io_op);
                io_op |= IO_ERROR;
            }
            else
            {
                switch (io_op)
                {
                    case GETL_CALL:
                        fgets((char *)addr, 256, stdin);
                        break;
                    case GETS_CALL:
                        scanf("%s", (char *)addr);
                        break;
                    case GETI_CALL:
                        scanf("%d", (int32_t *)addr);
                        break;
                    case EXEC_CALL:
                        status = Load( (char *)addr, 0);
                        if (status <= 0) io_op |= IO_ERROR;
                        Abs_Set_Word(io_blk_addr + 2*WORD_SIZE, status);
                        break;
                    case PRINTS_CALL:
                        while (*addr)
                        {
                            usleep(100); // ~100000 baud
                            fputc(*addr, stdout);
                            fflush(stdout);
                            addr++;
                        }
                        break;
                    default:
                        Machine_Check(MC_HW_WARNING | MC_ILLEGAL_INST,
                                "Invalid IO code: %d", io_op);
                        io_op |= IO_ERROR;
                        break;
                }
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
static void Finish_IO()
{
    IO_Q_Halt_Thread = 1;
    pthread_cond_broadcast(&IO_Q_Cond);
    pthread_join(IO_Q_Thread, NULL);
}
//*************************************
void IO_Enable_Inp()
{
    Allow_INP_Instr = 1;
}
//*************************************
void Init_IO(int allow_INP)
{
    Allow_INP_Instr = allow_INP;

    IO_Q_Head = 0;
    IO_Q_Tail = 0;
    IO_Q_Halt_Thread = 0;

    pthread_create(&IO_Q_Thread, NULL, IO_Processor, NULL);

    Handler_List = (handler_t *)malloc(sizeof(handler_t));
    assert(Handler_List != NULL);
    Handler_List->next = NULL;

    atexit(Finish_IO);
}
//*************************************
void Schedule_IO(Machine_State *cpu, int32_t io_blk_addr)
{
    int32_t op;

    // NOTE: Ignoring queue overflow
    pthread_mutex_lock(&IO_Q_Lock);

    IO_Q[IO_Q_Head].io_blk_addr = io_blk_addr;
    IO_Q[IO_Q_Head].buff        = Get_Word(cpu, io_blk_addr + WORD_SIZE);

    op = Get_Word(cpu, io_blk_addr);
    op &= ~IO_FLAGS;
    op |= IO_QUEUED;
    Set_Word(cpu, io_blk_addr, op);

    IO_Q_Head++;
    IO_Q_Head %= IO_Q_SIZE;

    pthread_cond_signal(&IO_Q_Cond);
    pthread_mutex_unlock(&IO_Q_Lock);
}

//*************************************
static handler_t *get_handler(int32_t addr)
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
int32_t IO_Get_Word(int32_t address)
{
    handler_t *handler = get_handler(address);
    if (handler != NULL)
        return handler->get_word(handler->id, address - handler->addr);
    else
        Machine_Check(MC_HW_WARNING | MC_ILLEGAL_ADDR,
                "Get from invalid IO word address: 0X%08X\n", address);
    return 0xFFFFFFFF;
}
//*************************************
void IO_Set_Word(int32_t address, int32_t value)
{
    handler_t *handler = get_handler(address);
    if (handler != NULL) 
    {
        handler->set_word(handler->id, address - handler->addr, value);
    }
    else
    {
        Machine_Check(MC_HW_WARNING | MC_ILLEGAL_ADDR,
                "Set to invalid IO word address: 0X%08X\n", address);
    }
}
//*************************************
int32_t IO_Get_Byte(int32_t address)
{
    handler_t *handler = get_handler(address);
    if (handler != NULL)
        return handler->get_byte(handler->id, address - handler->addr);
    else
        Machine_Check(MC_HW_WARNING | MC_ILLEGAL_ADDR,
                "Get from invalid IO byte address: 0X%08X\n", address);
    return 0xFF;
}
//*************************************
void IO_Set_Byte(int32_t address, int32_t value)
{
    handler_t *handler = get_handler(address);
    if (handler != NULL) 
    {
        handler->set_byte(handler->id, address - handler->addr, value);
    }
    else
    {
        Machine_Check(MC_HW_WARNING | MC_ILLEGAL_ADDR,
                "Set to invalid IO byte address: 0X%08X\n", address);
    }
}
//*************************************
int32_t IO_Register_Handler(int32_t id, int32_t addr, int32_t size, 
        int32_t (*get_word)(int32_t id, int32_t addr),
        int32_t (*get_byte)(int32_t id, int32_t addr),
        void (*set_word)(int32_t id, int32_t addr, int32_t value),
        void (*set_byte)(int32_t id, int32_t addr, int32_t value)
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

