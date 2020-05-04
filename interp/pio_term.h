#pragma once
//**************************************************
// Constants for accessing the PIO Term device
//
#define PIO_T_RDR       0x0E000000  // Address of Read Data Register
#define PIO_T_XDR       0x0E000000  // Address of the Transmit Data Reg
#define PIO_T_IER       0x0E000001  // Address of the Interrupt Enable Reg
#define PIO_T_IIR       0x0E000002  // Address of the Interrupt Info Reg

#define PIO_T_IE_RECV   0x02        // Enable recv interrupts
#define PIO_T_IE_XMIT   0x04        // Enable xmit interrupts
#define PIO_T_IID_INT   0x01        // An interrupt has occured
#define PIO_T_IID_RECV  0x02        // A recv interrupt has occured
#define PIO_T_IID_XMIT  0x04        // an xmit interrupt has occured

#define PIO_T_VECTOR    5           // Vector number for PIO term device
