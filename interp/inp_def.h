#pragma once
//**************************************************
// inp_def.h
//
// Defines the operation codes used by the INP instruction
//

typedef struct
{
    int op;
    void *param1;
    int  param2;
} io_blk_t;

#define INP_PRINTS_CALL     3       // print a null terminated string
#define INP_GETS_CALL       5       // scanf("%s", param1);
#define INP_GETL_CALL       6       // fgets(param1, 256, stdin);
#define INP_GETI_CALL       7       // scanf("%d", param1);
#define INP_EXEC_CALL       8       // load a program into memory

#define INP_OP_DONE     0x80000000  // operation has finished
#define INP_OP_ERROR    0x40000000  // operation had an error
