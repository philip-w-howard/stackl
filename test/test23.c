#include <string.h>
#include "test/io.h"

int main()
{
    char * a;
    a = "testing\n";
    char ** b;
    b = &a;
    char *** c;
    c = &b;
    char ** d;
    d = c[0];
    char * e;
    e = b[0];

    int z;
    z = 12345;
    int *y;
    y = &z;
    int **x;
    x = &y;
    int ***w;
    w = &x;
    int ** v;
    v = w[0];
    int * u;
    u = v[0];
    int t;
    t = u[0];

    prints(e);
    printi(t);
    prints("\n");

    return 0;
}
