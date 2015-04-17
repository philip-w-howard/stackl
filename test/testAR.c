#include <string.h>
#include <system.h>

int main()
{
    struct 
    {
        int z;
    char b[20];
    } s;
    
    char * src;
    src = "ABCDEFG";
    char * dest;
    dest = "GFEDCBA";
    s a;

    int count;
    count = 0;
    while (src[count] != 0)
    {
        a.b[count] = src[count];
        //dest[count] = src[count];
        count++;
    }
    a.b[count] = 0;
    //dest[count] = 0;  
    prints(a.b);
    //prints(dest);

    return 0;
}
