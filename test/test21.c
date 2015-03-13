#include "system.h"

int main()
{
    char arr[50];
    int val;

    prints("prints works\n");
    prints("print the number 88: ");
    printi(88);

    scans(arr);
    prints(arr);
    prints("\n");
    scanl(arr);
    prints(arr);
    prints("\n");

    scani(&val);
    prints("entered: ");
    printi(val);

    prints("Testing exit\n");
    exit(0);
    prints("Exit did not work\n");

    return 0;
}
/*END*/
