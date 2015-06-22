//#include <string.h>
//#include <system.h>

int main()
{
    char *cptr;
    char carr[20];
    char carr1[51];

    cptr = carr;
    cptr[0] = 'x';
    carr[1] = 'x';
    cptr[2] = '\n';
    carr[3] = 0;
    sys_prints(carr);
    sys_prints(cptr);
    sys_prints("The end\n");

    cptr = "this is a cptr\n";
    sys_prints(cptr);

    cptr += 5;
    sys_prints(cptr);

    cptr = carr1;
    carr1[0] = 'h';
    carr1[1] = 'e';
    carr1[2] = 'l';
    carr1[3] = 'l';
    carr1[4] = 'o';
    carr1[5] = '\n';
    carr1[6] = 0;

    sys_prints(cptr);
}
