// Check sizeof
// NOTE: This test is incomplete
//
#include <string.h>
#include <io.h>

int main()
{
    int a;
    char *b;

    printi(sizeof a);
    prints("\n");
    printi(sizeof(a));
    prints("\n");
    printi(sizeof(int));
    prints("\n");
    printi(sizeof b);
    return 0;
}
