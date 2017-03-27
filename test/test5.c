#include <sysio.h>
int main()
{
    int ii;
    ii = 0;
    while (ii < 3)
    {
        prints("while loop\n");
        ii = ii + 1;
    }
}
