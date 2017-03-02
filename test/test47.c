// This code used to segfault because the struct was not word-sized so the 
// array was not word aligned
#include <string.h>
#include "test/io.h"
int main()
{
    typedef struct 
    {
        int a;
        char b;
    } struct_t;

    struct_t var[6];
    int ii;

    var[1].a =3;

    printi(var[1].a);

    return 0;
}

