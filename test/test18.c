#include <string.h>
#include <system.h>

int empty()
{ }

int print(int ret, char *str)
{
    prints(str);
    return ret;
}

int main()
{
    int a;

    if (print(1, "if 1\n")) ;
    empty();
    if (print(0, "if 2\n")) 
    {}
    else prints("else 1\n");
    
    a = 2;
    while (print(0, "while 1\n")) ;
    while (print(0, "while 2\n")) 
    {}

    prints("Test done\n");
    return 0;
}
