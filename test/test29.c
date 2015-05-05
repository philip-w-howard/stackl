#include <string.h>
#include <system.h>

int g_a;
int g_c;
int g_d;
int* g_b;
int* g_e;
char* g_v;
char* g_y;
char g_x;
char g_z;
char g_w;

int main()
{
    int a; // 0 
    int c; // 4 
    int d; // 8
    int* b; // 12
    int* e; // 16
    char* v; // 20
    char* y; // 24
    char x; // 28
    char z; // 29
    char w; // 30

    // right side deref chars
    x = 103;
    y = &x;
    z = *y;
    printi(z);
    prints("\n");
    
    // global right side deref chars
    g_x = 103;
    g_y = &g_x;
    g_z = *g_y;
    printi(g_z);
    prints("\n");

    // right side deref ints
    a = 12345;
    b = &a;
    c = *b;
    printi(a);
    prints("\n");

    // global right side deref ints
    g_a = 12345;
    g_b = &g_a;
    g_c = *g_b;
    printi(g_a);
    prints("\n");

    // left side deref chars
    x = 126;
    y = &x;
    *y = 103;
    printi(x);
    prints("\n");

    // global left side deref chars
    g_x = 126; 
    g_y = &g_x;
    *g_y = 103;
    printi(g_x);
    prints("\n");

    // left side deref ints
    a = 5;
    b = &a;
    *b = 12345;
    printi(a);
    prints("\n");

    // global left side deref ints
    g_a = 5;
    g_b = &g_a;
    *g_b = 12345;
    printi(g_a);
    prints("\n");

    // deref both sides char
    x = 126;
    z = 103;
    v = &x;
    y = &z;
    *v = *y;
    printi(x);
    prints("\n");

    // global deref both sides char
    g_x = 126;
    g_z = 103;
    g_v = &g_x;
    g_y = &g_z;
    *g_v = *g_y;
    printi(g_x);
    prints("\n");

    // deref both sides int
    a = 54321;
    c = 12345;
    b = &a;
    e = &c;
    *b = *e;
    printi(a);
    prints("\n");
    
    // global deref both sides int
    g_a = 54321;
    g_c = 12345;
    g_b = &g_a;
    g_e = &g_c;
    *g_b = *g_e;
    printi(g_a);
    prints("\n");

    // deref both sides, mixed char/int
    a = 12345;
    x = 103;
    b = &a;
    y = &x;
    *b = *y;
    printi(a);
    prints("\n");

    // global deref both sides, mixed char/int
    g_a = 12345;
    g_x = 103;
    g_b = &g_a;
    g_y = &g_x;
    *g_b = *g_y;
    printi(g_a);
    prints("\n");

    return 0;
}
