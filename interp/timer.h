#pragma once
//****************************************************
// Bit definitions for the timer
//
#define TIMER_CSR       0x0C000000      // Control/Status register
#define TIMER_COUNT     0x0C000004      // Current count value
#define TIMER_LIMIT     0x0C000008      // Interrupt when reaching this value

#define TIMER_CSR_IE    0x00000001      // Interrupt enable
#define TIMER_CSR_INT   0x80000000      // Interrupt occurred

int Timer_Init();                       // called by interpreter
int Timer_Heartbeat();                  // called by interpreter
