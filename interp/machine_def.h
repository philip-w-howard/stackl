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

