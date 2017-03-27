// Test type 2 of main struct decl
#include <string.h>
#include <sysio.h>

typedef struct 
{
    int a;
    int b;
    int c;
    //struct test1 next; // <-- should throw an error
} test1;

int main()
{   
    struct test1 testa; 
    
    testa.a = 12;
    printi(testa.a);
    prints("\nTest J202 Complete\n"); 
    return 0;
}
