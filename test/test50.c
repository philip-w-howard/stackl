#include <sysio.h>

// This test is to make sure that the compiler correctly handles arrays of 
// pointers. As of 6/3/2017, the compiler would report a syntax error 
// because it thought the array reference was a struct instead of a pointer.
//
// If this code compiles, then the bug was fixed

typedef struct
{
    int a;
    int b;
} struct_t;

static struct_t *array[5];

int main()
{
    struct_t *ptr;

    array[3] = ptr;
    array[4] = &ptr[4];

    prints("Test passed\n");

    return 0;
}
