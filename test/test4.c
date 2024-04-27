#include <sysio.h>
int main()
{
    int ii;
    for (ii = 0 ; ii<5 ; ii = ii+1)
    {
        prints("for loop\n");
        continue;
        prints("failed\n");
    }
    for (ii = 0 ; ii<5 ; ii = ii+1)
    {
        prints("for loop\n");
        break;
    }
    return 0;
}
