// Test iter through LL

#include <string.h>
#include <sysio.h>

typedef struct test1 
{
    int a;
    int b;
    int c;
    struct test1* next;
} test2;

int main()
{   
    test1 ta;
    test1 tb;
    test1 tc;
    test1 td;
    test1 te;
    test1 *travel;
    int i;

    ta.a = 1;
    tb.a = 2;
    tc.a = 3;
    td.a = 4;
    te.a = 5;

    ta.next = &tb;
    tb.next = &tc;
    tc.next = &td;
    td.next = &te;
    te.next = NULL;

    travel = &ta;
    while(travel->next != (void *)NULL)
    {
        printi(travel->a);
        travel = travel->next;
    }
    

    prints("\nTest J204 Complete\n");
    return 0;
}
