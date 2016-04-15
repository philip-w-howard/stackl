// Test passing address of struct to func
// This was a bug reported in V0.7.1
//
#include <string.h>
#include "test/io.h"

typedef struct
{
    int a;
    int b;
} struct_t;

int set(int value, struct_t *s)
{
    int i1;
    int i2;
    int i3;

    s->a = value;
    s->b = value+1;

    i1 = 5;
    i2 = 6;
    i3 = 7;

    return 0;
}

int main()
{
    int ii;

    int const1;
    int const2;
    struct_t s;
    int const3;
    int const4;

    const1 = 1234;
    const2 = 2345;
    const3 = 3456;
    const4 = 4567;

    for (ii=0; ii<10; ii++)
    {
        prints("Inter ");
        printi(ii);
        prints("\n");

        set(ii, &s);
    }

    if (const1 != 1234 ||
        const2 != 2345 ||
        const3 != 3456 ||
        const4 != 4567)
    {
        prints("Stack overwrite\n");
    }

    if (s.a != ii-1 || s.b != ii)
    {
        prints("Error setting struct\n");
    }

    return 0;
}
