// Test having a struct decl in a struct
#include <string.h>
#include "test/io.h"

typedef struct test1 
{
    int a;
    int b;
    int c;
    struct test1 next;
} test2;

test2 global;

int main()
{   
    test2 testa;
    struct test1 testb;
    
    testa.next.a = 12;
    printi(testa.next.a);
    prints("\nTest J201 Complete\n");
    return 0;
}
