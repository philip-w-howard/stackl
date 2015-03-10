#include "test/string.h"

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

    print(stuff.aa);
    print(stuff.bb);
    print(stuff.cc);
    print(stuff.dd);

    return 0;
}
/*END*/
