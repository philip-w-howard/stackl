#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "vmem.h"
#include "loader.h"
#include "machine.h"
#include "io_handler.h"
#include "gen_io.h"

static volatile uint32_t Gen_IO_CSR_Reg = 0;
static volatile uint32_t Gen_IO_Buff_Reg = 0;
static volatile uint32_t Gen_IO_Size_Reg = 0;
static volatile uint32_t Gen_IO_Count_Reg = 0;

static volatile int IO_Q_Halt_Thread = 0;
static pthread_t IO_Q_Thread;
static pthread_cond_t  IO_Q_Cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t IO_Q_Lock = PTHREAD_MUTEX_INITIALIZER;

static void Gen_IO_Finish();

typedef struct
{
    uint32_t op;
    uint32_t buff;
    uint32_t size;

} gen_io_blk_t;

static void *gen_io_thread(void *arg);

#define INTERRUPT_MASK (GEN_IO_CSR_IE | GEN_IO_CSR_DONE)

//***********************************
static int32_t get_byte(int32_t id, int32_t addr)
{
    Machine_Check(MC_HW_WARNING | MC_ILLEGAL_ADDR,
            "timer registers are word wide");
    return 0;
}
//***********************************
static int32_t get_word(int32_t id, int32_t addr)
{
    int32_t value;

    if (addr == 0) 
    {
        value = Gen_IO_CSR_Reg;
    }
    else if (addr == 4) 
    {
        value = Gen_IO_Buff_Reg;
    }
    else if (addr == 8)
    {
        value = Gen_IO_Size_Reg;
    }
    else if (addr == 12)
    {
        value = Gen_IO_Count_Reg;
    }

    return value;
}
//***********************************
static void set_byte(int32_t id, int32_t addr, int32_t value)
{
    Machine_Check(MC_HW_WARNING | MC_ILLEGAL_ADDR,
            "timer registers are word wide");
}
//***********************************
static void set_word(int32_t id, int32_t addr, int32_t value)
{
    if (addr == 0) 
    {
        Gen_IO_CSR_Reg = value;
    }
    else if (addr == 4) 
    {
        Gen_IO_Buff_Reg = value;
    }
    else if (addr == 8)
    {
        Gen_IO_Size_Reg = value;
    }
    else if (addr == 12)
    {
        Gen_IO_Count_Reg = value;
    }
}
//*************************************
static void Gen_IO_Finish()
{
    IO_Q_Halt_Thread = 1;
    pthread_cond_broadcast(&IO_Q_Cond);
    pthread_join(IO_Q_Thread, NULL);
}
//*************************************
int Gen_IO_Init()
{
    IO_Register_Handler(0, GEN_IO_CSR, 16,
            get_word, get_byte, set_word, set_byte);

    pthread_create(&IO_Q_Thread, NULL, gen_io_thread, NULL);

    atexit(Gen_IO_Finish);

    return 0;
}

//*************************************
static void *gen_io_thread(void *arg)
{
    int op;
    int num;
    int status;
    int stackl_addr;
    char *addr;

    pthread_mutex_lock(&IO_Q_Lock);
    while (!IO_Q_Halt_Thread)
    {
        while (!IO_Q_Halt_Thread && (Gen_IO_CSR_Reg & GEN_IO_CSR_DONE))
        {
            pthread_cond_wait(&IO_Q_Cond, &IO_Q_Lock);
        }

        stackl_addr = Gen_IO_Buff_Reg + Gen_IO_Count_Reg;
        addr = Abs_Get_Addr(stackl_addr);

        op = Gen_IO_CSR_Reg & 0xFF;
        switch (op)
        {
            case GEN_IO_OP_PRINTS:
                if (Gen_IO_Size_Reg > Gen_IO_Count_Reg)
                {
                    pthread_mutex_unlock(&IO_Q_Lock);
                    usleep(100); // ~100000 baud
                    fputc(*addr, stdout);
                    fflush(stdout);
                    pthread_mutex_lock(&IO_Q_Lock);
                    Gen_IO_Count_Reg++;
                }
                else
                {
                    Gen_IO_CSR_Reg |= GEN_IO_CSR_DONE;
                }
                break;
            case GEN_IO_OP_PRINTC:
                pthread_mutex_unlock(&IO_Q_Lock);
                usleep(100); // ~100000 baud
                fputc(*addr, stdout);
                fflush(stdout);
                pthread_mutex_lock(&IO_Q_Lock);

                Gen_IO_Count_Reg++;

                Gen_IO_CSR_Reg |= GEN_IO_CSR_DONE;
                break;
            case GEN_IO_OP_GETL:
                if (fgets(addr, Gen_IO_Size_Reg, stdin) == NULL)
                {
                    Gen_IO_CSR_Reg |= GEN_IO_CSR_ERR;
                }
                Gen_IO_CSR_Reg |= GEN_IO_CSR_DONE;
                break;
            case GEN_IO_OP_GETI:
                num = scanf("%d", (int *)addr);
                if (num != 1) Gen_IO_CSR_Reg |= GEN_IO_CSR_ERR;
                Gen_IO_CSR_Reg |= GEN_IO_CSR_DONE;
                break;
            case GEN_IO_OP_EXEC:
                pthread_mutex_unlock(&IO_Q_Lock);
                status = Load(addr, 0);
                pthread_mutex_lock(&IO_Q_Lock);
                Gen_IO_Count_Reg = status;
                Gen_IO_CSR_Reg |= GEN_IO_CSR_DONE;
                if (status < 0)  Gen_IO_CSR_Reg |= GEN_IO_CSR_ERR;
                break;
        }

        if ((Gen_IO_CSR_Reg & INTERRUPT_MASK) == INTERRUPT_MASK)
        {
            Gen_IO_CSR_Reg |= GEN_IO_CSR_INT;
            Machine_Signal_Interrupt(1, GEN_IO_VECTOR);
        }
    }
    return NULL;
}
