#include <string.h>
#include <system.h>

int main()
{
    char arr[50];
    int val;

    prints("calling scanl: ");
    scanl(arr);
    prints(arr);
    prints("\n");

    prints("calling scans: ");
    scans(arr);
    prints(arr);
    prints("\n");

    prints("calling scani: ");
    scani(&val);
    prints("entered: ");
    printi(val);
    prints("\n");

    prints("Testing exit\n");
    exit(0);
    prints("Exit did not work\n");

    return 0;
}
