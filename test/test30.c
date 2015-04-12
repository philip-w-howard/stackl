#include <string.h>
#include <system.h>
int prints(char *string);

int interrupt()
{
    prints("Interrupt\n");
    asm(31);        // RTI_OP
}

int main()
{
    int ii;
    int count;
    count = 0;
    for (ii=0; ii<200; ii++)
    {
        count++;
    }

    prints("Done\n");
    return 0;
}
