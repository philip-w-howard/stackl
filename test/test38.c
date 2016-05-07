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

    return value;
}
