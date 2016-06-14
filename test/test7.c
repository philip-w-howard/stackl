#include <string.h>
#include "test/io.h"
int main()
{
    int x;

    x = 1234;
    printi(x);
    prints("\n");

    x = 0;
    printi(x);
    prints("\n");

    x = -789;
    printi(x);
    prints("\n");

    {
        char *ptr;
        char array[20];
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

        strcpy(array, "this is");
        strcat(array, "a test");
        strcat(array, "\n");
        prints(array);
    }
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
    }

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

    return 0;
}
