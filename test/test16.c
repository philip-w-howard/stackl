#include "system.h"

int DoNotPrintMe()
{
    prints("failure\n");
    return 1;  
}

int DoPrintMe(int ret)
{
    prints("success\n");
    return ret;  
}

int main()
{
    if(0 && DoNotPrintMe())
        prints("failure\n");
    if(1 || DoNotPrintMe())
        prints("success\n");
    if(1 && DoPrintMe(0))
        prints("failure\n");
    if(0 || DoPrintMe(0))
        prints("failure\n");
    if(1 && DoPrintMe(1))
        prints("success\n");
    if(0 || DoPrintMe(1))
        prints("success\n");
}

