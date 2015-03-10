#include "system.h"

int main()
{
    char arr[50];

    prints("prints works\n");
    prints("print the number 88: ");
    printi(88);

    scans(arr);
    prints(arr);
    prints("\n");
    scanl(arr);
    prints(arr);
    prints("\n");

    print("Testing exit\n");
    exit(0);
    print("Exit did not work\n");

    return 0;
}
/*END*/
