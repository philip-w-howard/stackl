#include "string.h"
int main()
{
    char *ptr;
    char arr[100];

    ptr = "stuff";
    if (strcmp(ptr, "stuff") == 0) print("strcmp worked\n");
    strcpy(arr, "stuff");
    if (strcmp(ptr, arr) == 0) print("strcpy worked\n");

    return 0;
}
/*END*/
