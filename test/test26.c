#include <string.h>
#include <system.h>

int main()
{
    typedef struct
    {
        int a;
        char *b;
        int c;
    } str_t;

    char * hello;
    char l;
    hello = "HELLO";
    
    l = hello[2];

    printi(l);
    prints("\n");

    return 0;
}
