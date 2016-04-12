#include <string.h>
#include "test/io.h"

int main()
{
    char * ptr;
    char arr[10];
    ptr = "hello";
    ptr++;
    arr[0] = *ptr;
    arr[1] = 0;
    prints(arr);
    prints("\n");
    return 0;
}
