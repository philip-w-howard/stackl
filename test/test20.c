#include <string.h>
#include <system.h>

const int XVALX = 0xFF;
int global1;
int global2;

int func()
{
    prints("g1: ");
    printi(global1);
    prints("\n");

    global2 = 15;

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

    return 0;
}
