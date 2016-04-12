#include <string.h>
#include "test/io.h"

const int XVALX = 0xFF;
int global1;
int global2;

typedef struct
{
    int a;
    int b;
} struct_t;

struct_t global3;

int func()
{
    prints("g1: ");
    printi(global1);
    prints("\n");

    global2 = 15;

    prints("g3a: ");
    printi(global3.a);
    prints("\n");

    global3.b = 35;

    return 0;
}
int main()
{
    int local1;
    int local2;

    local1 = 1;
    local2 = 2;
    global1 = 11;
    global2 = 12;

    global3.a = 21;
    global3.b = 22;

    prints("l1: ");
    printi(local1);
    prints("\n");

    prints("l2: ");
    printi(local2);
    prints("\n");

    prints("g1: ");
    printi(global1);
    prints("\n");

    prints("g2: ");
    printi(global2);
    prints("\n");

    prints("g3.a: ");
    printi(global3.a);
    prints("\n");

    prints("g3.b: ");
    printi(global3.b);
    prints("\n");

    func();

    prints("l1: ");
    printi(local1);
    prints("\n");

    prints("l2: ");
    printi(local2);
    prints("\n");

    prints("g1: ");
    printi(global1);
    prints("\n");

    prints("g2: ");
    printi(global2);
    prints("\n");

    prints("g3.a: ");
    printi(global3.a);
    prints("\n");

    prints("g3.b: ");
    printi(global3.b);
    prints("\n");

    return 0;
}
