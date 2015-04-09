#include "system.h"

int DoNotPrintMe()
{
    prints("failure\n");
    return 1;  
}

int main()
{
    if(0 && DoNotPrintMe())
        prints("failure\n");
    if(1 || DoNotPrintMe())
        prints("success");
}

