#include <string.h>
#include <io.h>

// additional string library tests
int main()
{
    char *ptr;
    char array[10];
    int result;
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

    prints("Checking strcmp\n");
    ptr = "12345";
    strcpy(array, ptr);
    if (strcmp(array, ptr) != 0) prints("strcmp is broken\n");

    ptr = "12346";
    if ( strcmp(ptr, array) <= 0) prints("strcmp is broken\n");
    if ( strcmp(array, ptr) >= 0) prints("strcmp is broken\n");

    ptr = "1234";
    if ( strcmp(ptr, array) >= 0) prints("strcmp is broken\n");
    if ( strcmp(array, ptr) <= 0) prints("strcmp is broken\n");

    return 0;
}
