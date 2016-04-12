#include <string.h>
#include "test/io.h"

// test string library
int main()
{
    char *ptr;
    char array[10];
    ptr = "1234\n";

    printi( strlen(ptr) );
    prints("\n");
    strcpy(array, ptr);
    prints( array );

    array[6] = '\n';
    array[7] = 0;
    ptr = "this is a test";
    strncpy(array, ptr, 6);
    prints(array);

    ptr = "abcdefghij\n";
    ptr = strchr(ptr, 'f');
    prints(ptr);
    return 0;
}
