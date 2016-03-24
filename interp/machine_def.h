#pragma once

#include "opcode_defs.h"

/*
#define NOP         0
#define PLUS_OP     1
#define MINUS_OP    2
#define TIMES_OP    3
#define DIVIDE_OP   4
#define MOD_OP      5
#define EQ_OP       6
#define NE_OP       7
#define GT_OP       8
#define LT_OP       9
#define GE_OP       10
#define LE_OP       11
#define AND_OP      12
#define OR_OP       13
#define NOT_OP      14
#define SWAP_OP     15
#define DUP_OP      16
#define HALT_OP     17
#define POP_OP      18
#define RETURN_OP   19
#define RETURNV_OP  20
#define NEG_OP      21
#define PUSHCVARIND_OP  22
#define POPCVARIND_OP   23
#define OUTS_OP     24
#define INP_OP      25
#define PUSHFP_OP   26
#define SETMODE_OP  27
#define JMPUSER_OP  28
#define TRAPTOC_OP  29
#define TRAP_OP     30
#define RTI_OP      31
#define CALLI_OP    32
#define PUSHREG_OP  33
#define POPREG_OP   34
#define BAND_OP     35
#define BOR_OP      36
#define BXOR_OP     37
#define SHIFTL_OP   38
#define SHIFTR_OP   39
#define PUSHVARIND_OP  40
#define POPVARIND_OP   41
#define COMP_OP     42

#define PUSH_OP     64
#define JUMP_OP     65
#define JUMPE_OP    66
#define PUSHVAR_OP  67
#define POPVAR_OP   68
#define ADJSP_OP    69
#define CALL_OP     70
#define PUSHCVAR_OP 71
#define POPCVAR_OP  72

#define TRACE_ON_OP  125
#define TRACE_OFF_OP 126

#define ILLEGAL_OP  127
*/

// Register definitions for SAVEREG and RESTREG
#define BP_REG      0
#define LP_REG      1
#define IP_REG      2
#define SP_REG      3
#define FP_REG      4
#define FLAG_REG    5

// word size for stackl machine
#define WORD_SIZE 4

// Bits for FLAG register
#define FL_HALTED       0x0001
#define FL_USER_MODE    0x0002
#define FL_INT_MODE     0x0004
#define FL_INT_DIS      0x0008
#define FL_INT_PENDING  0x0010
#define FL_VMEM         0x0020

