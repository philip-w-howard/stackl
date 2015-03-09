#include "string.h"

int main()
{
    char *ptr;
    char array[10];
    int result;
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

    print("Checking strcmp\n");
    ptr = "12345";
    strcpy(array, ptr);
    if (strcmp(array, ptr) != 0) print("strcmp is broken\n");

    ptr = "12346";
    if ( strcmp(ptr, array) <= 0) print("strcmp is broken\n");
    if ( strcmp(array, ptr) >= 0) print("strcmp is broken\n");

    ptr = "1234";
    if ( strcmp(ptr, array) >= 0) print("strcmp is broken\n");
    if ( strcmp(array, ptr) <= 0) print("strcmp is broken\n");

    return 0;
}
/*END*/
