#include "system.h"
int main()
{
    char *cptr;
    char carr[20];
    char carr1[51];

    cptr = carr;
    carr[0] = 'x';
    cptr[1] = 'x';
    carr[2] = '\n';
    carr[3] = 0;
    printc(carr[0]);
    printc(carr[1]);
    printc(carr[2]);
    prints(cptr);
    prints("The end\n");

    cptr = "this is a cptr\n";
    prints(cptr);

    cptr += 5;
    prints(cptr);

    cptr = carr1;
    carr1[0] = 'h';
    carr1[1] = 'e';
    carr1[2] = 'l';
    carr1[3] = 'l';
    carr1[4] = 'o';
    carr1[5] = '\n';
    carr1[6] = 0;

    prints(cptr);
}
