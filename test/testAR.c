#include <string.h>
#include <system.h>

int main()
{
    char * src; 
    src = "ABCDEFG";
    char * dest;
    dest = "GFEDCBA";

    strcpy(dest, src);

    //int count;
    //count = 0;
    //while (src[count] != 0)
    //{
    //    dest[count] = src[count];
    //    count++;
    //}
    //dest[count] = 0;

    prints(dest);

    return 0;
}
