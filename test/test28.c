#include <string.h>
#include <io.h>

typedef struct
{
    int op;
    char *addr;
    int status;
} io_blk_t;

io_blk_t Current_Op;

int main()
{
    Current_Op.addr = "0123456789";
    char *buff;

    buff = Current_Op.addr;

    if(Current_Op.addr[0] != '0') prints("Array index 0 broken\n");
    if(Current_Op.addr[1] != '1') prints("Array index 1 broken\n");
    if(Current_Op.addr[2] != '2') prints("Array index 2 broken\n");
    if(Current_Op.addr[3] != '3') prints("Array index 3 broken\n");
    if(Current_Op.addr[4] != '4') prints("Array index 4 broken\n");
    
    if(buff[0] != '0') prints("Array index 0 broken\n");
    if(buff[1] != '1') prints("Array index 1 broken\n");
    if(buff[2] != '2') prints("Array index 2 broken\n");
    if(buff[3] != '3') prints("Array index 3 broken\n");
    if(buff[4] != '4') prints("Array index 4 broken\n");

    prints("Done with array test\n");
}
