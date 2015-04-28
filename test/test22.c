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
    f = 0x1111;
    f &= e;
    int g;
    g = 0;
    g |= e & 0x0000;
    int h;
    h = 0xF0F0;
    int i;
    i = 0xFFFF;
    i ^= 0xEEEE;

    printi(e); // 0x1111 or 4369
    prints("\n");
    printi(f); // 0x1111 or 4369
    prints("\n");
    printi(g); // 0x0000 or 0
    prints("\n");
    printi(i); // 0x1111 or 4369
    prints("\n");
}
