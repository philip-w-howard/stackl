#include <string.h>
#include <system.h>

int main()
{
    typedef struct
    {
        int *b;
        int l;
        int c;
    } str_t;
    
    str_t str;
    str.b = &str.l; 
    str.l = 12345;
    str.c = *str.b;

    printi(str.c);
    prints("\n");
}
