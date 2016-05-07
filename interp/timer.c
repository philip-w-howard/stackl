#include <stdint.h>
#include "timer.h"
#include "machine.h"
#include "io_handler.h"

static volatile uint32_t Timer_CSR_Reg = 0;
static volatile uint32_t Timer_Count_Reg = 0;
static volatile uint32_t Timer_Limit_Reg = 0xFFFFFFFF;

//***********************************
static int get_byte(int id, int addr)
{
    Machine_Check("timer registers are word wide");
    return 0;
}
//***********************************
static int get_word(int id, int addr)
{
    int value;

    if (addr == 0) 
    {
        value = Timer_CSR_Reg;
        Timer_CSR_Reg &= ~TIMER_CSR_INT;
    }
    else if (addr == 4) 
        value = Timer_Count_Reg;
    else if (addr == 8)
    {
        value = Timer_Limit_Reg;
    }

    return value;
}
//***********************************
static void set_byte(int id, int addr, int value)
{
    Machine_Check("timer registers are word wide");
}
//***********************************
static void set_word(int id, int addr, int value)
{
    if (addr == 0) 
    {
        Timer_CSR_Reg = value;
    }
    else if (addr == 4) 
    {
        Timer_Count_Reg = value;
    }
    else if (addr == 8)
    {
        Timer_Limit_Reg = value;
    }
}
//*************************************
int Timer_Init()
{
    IO_Register_Handler(0, TIMER_CSR, 12,
            get_word, get_byte, set_word, set_byte);

    return 0;
}
//*************************************
// Must be called once per instruction.
// Assumes the Machine_Lock is held when it is called
int Timer_Heartbeat()
{
    Timer_Count_Reg++;
    if (Timer_Count_Reg >= Timer_Limit_Reg)
    {
        Timer_CSR_Reg |= TIMER_CSR_INT;
        Timer_Count_Reg = 0;

        if (Timer_CSR_Reg & TIMER_CSR_IE)
        {
            Machine_Signal_Interrupt(0);
        }
    }
    return 0;
}
