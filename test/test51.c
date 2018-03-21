// This test is to see if an array of pointers can be passed to a function
// as a void*
// As of 3/20/2018, this reported an error

#include <sysio.h>

void func(void *array)
{
}

int main()
{
    void *stuff[20];
    void more_stuff[20];

    func(stuff);
    prints("Test passed\n");
    func(more_stuff);

    return 0;
}
