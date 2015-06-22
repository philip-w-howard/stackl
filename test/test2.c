//#include <string.h>
//#include <system.h>
// test if/else
int main()
{
    if (1)
        asm(OUTS_OP, "if works\n");
    else
        asm(OUTS_OP, "if 1 broken\n");

    if (0)
        asm(OUTS_OP, "if 2 broken\n");
    else
        asm(OUTS_OP, "if-else works\n");

    return 0;
}

