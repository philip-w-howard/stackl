#include <string.h>
#include <system.h>

int main()
{
    // tests issue where an array reference on a character array
    // would cause a machine check/not function properly
    char buff[10];
    strcpy(buff, "0123456789");

    buff[0] = '7';

    if(buff[0] != '7') prints("Array index 0 broken\n");
    if(buff[1] != '1') prints("Array index 1 broken\n");
    if(buff[2] != '2') prints("Array index 2 broken\n");
    if(buff[3] != '3') prints("Array index 3 broken\n");
    if(buff[4] != '4') prints("Array index 4 broken\n");

    // tests pointer dereference
    // Generates a misaligned address error because it does a word access
    // instead of a byte access.
    // ALSO: Does not seem to push the lval prior to the pop The result is that
    // the 2 is interpreted as the destingation of the assignment, and the data
    // is whatever was on the stack prior to this code snippet.
    char *ier_reg;
    ier_reg = 0xE000001;
    *ier_reg = *ier_reg | 2;

    return 0;
}
