// Test goto statement
#include <sysio.h>
int main()
{
    int ii;
    ii = 0;

label1: prints("At label1\n");
    ii++;
    if (ii < 2) goto label1;

    goto label3;

label2: prints("At label2\n");
label3: prints("At label3\n");

    return 0;
}
