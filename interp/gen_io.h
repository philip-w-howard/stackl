#pragma once
//****************************************************
// Bit definitions for the Generic IO device
//
#define GEN_IO_CSR      0x0B000000      // Control/Status register
#define GEN_IO_BUFF     0x0B000004      // Buffer address register
#define GEN_IO_SIZE     0x0B000008      // Buffer size register
#define GEN_IO_COUNT    0x0B00000C      // Number of characters sent

#define GEN_IO_CSR_IE   0x00010000      // Interrupt enable
#define GEN_IO_CSR_INT  0x00020000      // Interrupt occurred
#define GEN_IO_CSR_DONE 0x80000000      // The operation is complete
#define GEN_IO_CSR_ERR  0x40000000      // The operation resulted in an error

#define GEN_IO_VECTOR   8

// Operations: occupy the lower 8 bits of the CSR register
#define GEN_IO_OP_PRINTS    1
#define GEN_IO_OP_PRINTC    2
#define GEN_IO_OP_GETL      3
#define GEN_IO_OP_GETI      4
#define GEN_IO_OP_EXEC      5

int Gen_IO_Init();                       // called by interpreter


