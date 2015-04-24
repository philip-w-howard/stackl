#include <string.h>
#include <system.h>

int main()
{
    int a;
    a = 0x0001;
    int b;
    b = 0x0010;
    int c;
    c = 0x0100;
    int d;
    d = 0x1000;
    int e;
    e = a | b | c | d;
    int f;
    f = e & 0x1111;
    int g;
    g = e & 0x0000;

    printi(e); // 0x1111 or 4369
    prints("\n");
    printi(f); // 0x1111 or 4369
    prints("\n");
    printi(g); // 0x0000 or 0
    prints("\n");
}
