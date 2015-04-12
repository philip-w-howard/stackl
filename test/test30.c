#include <string.h>
#include <system.h>
int prints(char *string);

int interrupt()
{
    prints("Interrupt\n");
    rti();
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
