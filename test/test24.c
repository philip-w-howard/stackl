#include <string.h>
#include <io.h>

int main()
{
    char * a;
    a = "testing\n";
    char ** b;
    b = &a;
    char *** c;
    c = &b;
    char ** d;
    d = *c;         // FIX THIS: pushes a char instead of a char**
    char * e;
    e = *b;

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
