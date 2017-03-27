#include <string.h>
#include <sysio.h>

char *a;
char **b;
char ***c;
char **d;
char *e;

int z;
int *y;
int **x;
int ***w;
int **v;
int *u;

int main()
{
    a = "testing\n";
    b = &a;
    c = &b;
    d = *c;
    e = *b;
    prints(e);

    z = 12345;
    y = &z;
    x = &y;
    w = &x;
    v = *w;
    u = *v;

    printi(z);
    prints("\n");
    *u = 54321;
    printi(z);
    prints("\n");

    return 0;
}
