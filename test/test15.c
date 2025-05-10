#include <string.h>
#include <sysio.h>
int main()
{
    int ii;
    ii = 0;
    do
    {
        prints("do while loop 1\n");
        ii = ii + 1;
        continue;
        prints("failed\n");
    } while (ii < 3);

    ii = 0;
    do
    {
        prints("do while loop 2\n");
        ii = ii + 1;
    } while (ii < 3);

    ii = 0;
    do
    {
        prints("do while loop 3\n");
        ii = ii + 1;
        break;
    } while (ii < 3);
}
