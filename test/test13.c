#include "string.h"
#include "system.h"

int main()
{
    struct
    {
        int aa;
        int bb;
        char cc[20];
        char *dd;
    } s1;

    s1 stuff;

    stuff.aa = 1;
    stuff.bb = 2;
    strcpy(stuff.cc, "some text\n");
    stuff.dd = "this is a test\n";

    printi(stuff.aa);
    printi(stuff.bb);
    prints(stuff.cc);
    prints(stuff.dd);

    return 0;
}
