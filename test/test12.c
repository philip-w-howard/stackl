#include <string.h>
#include <io.h>

// additional string library tests
int main()
{
    char str1[10];
    char str2[10];
    char buff[20];

    strcpy(str1, "1234");
    strcpy(str2, "789");

    strrev(str1);
    strrev(str2);
    if (strcmp(str1, "4321") == 0)
        prints("strrev worked\n");
    else
        prints("strrev broken\n");

    if (strcmp(str2, "987") == 0)
        prints("strrev worked\n");
    else
        prints("strrev broken\n");

    itostr(123, buff);
    if (strcmp("123", buff) == 0)
        prints("itostr worked\n");
    else
        prints("itostr broken\n");

    if (strcmp("1234", itostr(1234, buff)) == 0)
        prints("itostr worked\n");
    else
        prints("itostr broken\n");

    itostr(-7890, buff);
    if (strcmp("-7890", buff) == 0)
        prints("itostr worked\n");
    else
        prints("itostr broken\n");

    itostr(-456, buff);
    if (strcmp("-456", buff) == 0)
        prints("itostr worked\n");
    else
        prints("itostr broken\n");

    itostr(0, buff);
    if (strcmp("0", buff) == 0)
        prints("itostr worked\n");
    else
        prints("itostr broken\n");

    return 0;
}
