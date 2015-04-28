#include <string.h>
#include <system.h>

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
int *t;

int main()
{
    a = "testing\n";
    b = &a;
    c = &b;
    d = *c;
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
    //printi(z);
    //*y = 7;
    //printi(z);
    prints("\n");

    return 0;
}
