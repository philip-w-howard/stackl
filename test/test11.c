#include "test/test11.h"

int main()
{
    char *ptr;
    char array[10];
    ptr = "1234\n";

    print( strlen(ptr) );
    strcpy(array, ptr);
    print( array );

    array[6] = '\n';
    array[7] = 0;
    ptr = "this is a test";
    strncpy(array, ptr, 6);
    print(array);

    ptr = "abcdefghij\n";
    ptr = strchr(ptr, 'f');
    print(ptr);
    return 0;
}
