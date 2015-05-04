#include <string.h>
#include <system.h>

int main()
{
    char buff[10];
    strcpy(buff, "0123456789");

    buff[0] = '7';

    if(buff[0] != '7') prints("Array index 0 broken\n");
    if(buff[1] != '1') prints("Array index 1 broken\n");
    if(buff[2] != '2') prints("Array index 2 broken\n");
    if(buff[3] != '3') prints("Array index 3 broken\n");
    if(buff[4] != '4') prints("Array index 4 broken\n");

    return 0;
}
