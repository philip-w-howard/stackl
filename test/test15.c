#include <string.h>
#include <sysio.h>
int main()
{
    int ii;
    ii = 0;
    do
    {
        prints("do while loop\n");
        ii = ii + 1;
    } while (ii < 3);

    do
    {
        prints("do while loop\n");
        ii = ii + 1;
    } while (ii < 3);
}
