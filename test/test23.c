#include <string.h>
#include <sysio.h>

int main()
{
    char * a;
    a = "testing\n";
    char ** b;
    b = &a;
    prints("b ");
    prints(*b);
    char *** c;
    c = &b;
    prints("c ");
    prints(**c);
    char ** d;
    d = c[0];
    prints("d ");
    prints(*d);
    char * e;
    e = d[0];
    prints("e ");
    prints(e);

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
