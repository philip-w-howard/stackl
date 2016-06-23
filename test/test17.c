// Check sizeof
// NOTE: This test is incomplete
//
#include <string.h>
#include "test/io.h"

typedef struct
{
    int a;
    int b;
    int c;
} size_12_t;

int main()
{
    int a;
    char *b;
    typedef struct
    {
        int aa;
        int bb;
        char cc;
    } struct_t;
    struct_t c;

    int d[50];

    typedef struct
    {
        int a;
        int b;
        int c;
        int d;
        int e;
        int f;
    } size_24_t;

    int value;

    printi(sizeof a);
    prints("\n");
    printi(sizeof(a));
    prints("\n");
    printi(sizeof(int));
    prints("\n");
    printi(sizeof b);
    prints("\n");
    printi(sizeof(*b));
    prints("\n");
    printi(sizeof(c));
    prints("\n");
    printi(sizeof(d));
    prints("\n");

    value = sizeof(char);
    printi(value);
    prints("\n");

    value = sizeof(int);
    printi(value);
    prints("\n");

    value = sizeof(size_12_t);
    printi(value);
    prints("\n");

    value = sizeof(size_24_t);
    printi(value);
    prints("\n");

    value = sizeof(void *);
    printi(value);
    prints("\n");

    return value;
}
