#include <string.h>
#include <sysio.h>

int main()
{
    char * a;
    a = "testing\n";
    prints(a);
    char ** b;
    b = &a;
    prints(*b);
    char *** c;
    c = &b;
    prints(**c);
    char ** d;
    d = *c;
    prints(*d);
    char * e;
    e = *d;
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
    v = *w;
    int * u;
    u = *v;
    int t;
    t = *u;

    prints(e);
    printi(t);
    prints("\n");

    return 0;
}
