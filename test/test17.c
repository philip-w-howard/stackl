// Check sizeof
// NOTE: This test is incomplete
//
#include <string.h>
#include "test/io.h"

int main()
{
    int a;
    char *b;
    typedef struct
    {
        int aa;
        int bb;
        char cc;
    } struct_t;
    struct_t c;

    int d[50];

    printi(sizeof a);
    prints("\n");
    printi(sizeof(a));
    prints("\n");
    printi(sizeof(int));
    prints("\n");
    printi(sizeof b);
    prints("\n");
    printi(sizeof(*b));
    prints("\n");
    printi(sizeof(c));
    prints("\n");
    printi(sizeof(d));
    prints("\n");
    return 0;
}
