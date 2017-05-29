#pragma feature gen_io

#include <string.h>
#include <sysio.h>
#include <gen_io.h>

const int *Gen_IO_CSR = GEN_IO_CSR;
const int *Gen_IO_Buff = GEN_IO_BUFF;
const int *Gen_IO_Size = GEN_IO_SIZE;
const int *Gen_IO_Count = GEN_IO_COUNT;

typedef struct
{
    int op;
    char *buff;
    int size;
    int count;
} gen_io_op_t;

int main()
{
    char *str = "Hello world\n";
    int size;

    size = strlen(str);
    
    *Gen_IO_Count = 0;
    *Gen_IO_Size = size;
    *Gen_IO_Buff = (int)str;
    *Gen_IO_CSR = GEN_IO_OP_PRINTS;

    while (*Gen_IO_CSR >= 0)
    {}

    printi(*Gen_IO_CSR);
    prints("\n");

    printi(*Gen_IO_Size);
    prints("\n");

    printi(*Gen_IO_Count);
    prints("\n");

    prints("Done\n");

    return 0;
}

