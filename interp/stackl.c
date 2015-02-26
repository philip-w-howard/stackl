#include <stdio.h>
#include "machine.h"

int main(int argc, char **argv)
{
    int result;

    if (argc < 2) 
    {
        printf("Need to specify an executable file\n");
        return 1;
    } else {
        result = Load_And_Go(argv[1]);
        if (result != 0) 
        {
            printf("Unable to execute %s\n", argv[1]);
            return 2;
        }
    }

    return 0;
}
