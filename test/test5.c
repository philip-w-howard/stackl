#include <sysio.h>
int main()
{
    int ii;
    ii = 0;
    while (ii < 3)
    {
        prints("while loop 1\n");
        ii = ii + 1;
    }

    ii = 0;
    while (ii < 3)
    {
        prints("while loop 2\n");
        ii = ii + 1;
        continue;
        prints("failed\n");
    }

    ii = 0;
    while (ii < 3)
    {
        prints("while loop 3\n");
        break;
    }
}
