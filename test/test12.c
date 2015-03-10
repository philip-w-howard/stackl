#include "system.h"
#include "string.h"
int main()
{
    char *ptr;
    char arr[100];

    ptr = "stuff";
    if (strcmp(ptr, "stuff") == 0) prints("strcmp worked\n");
    strcpy(arr, "stuff");
    if (strcmp(ptr, arr) == 0) prints("strcpy worked\n");

    return 0;
}
/*END*/
