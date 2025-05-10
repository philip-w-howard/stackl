#include <sysio.h>
int main()
{
    int ii;
    for (ii = 0 ; ii<5 ; ii = ii+1)
    {
        prints("for loop 1\n");
    }
    for (ii = 0 ; ii<5 ; ii = ii+1)
    {
        prints("for loop 2\n");
        continue;
        prints("failed\n");
    }
    for (ii = 0 ; ii<5 ; ii = ii+1)
    {
        prints("for loop 3\n");
        break;
    }
    return 0;
}
