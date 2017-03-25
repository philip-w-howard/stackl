#include <sysio.h>

int main()
{
    void *pointers[5];
    void *one_pointer;
    int  variable;

    one_pointer = &variable;
    pointers[1] = &variable;

    variable = one_pointer - pointers[1];
    if (one_pointer == pointers[1])
        prints("pointer arrays work\n");
    else
        prints("pointer arrays are broken\n");

    return 0;
}
