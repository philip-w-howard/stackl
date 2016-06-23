//#include "test/io.h"
// Test character arrays and pointers
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
    asm("OUTS", carr);
    asm("OUTS", cptr);
    asm("OUTS", "The end\n");
    //prints(carr);
    //prints(cptr);
    //prints("The end\n");

    cptr = "this is a cptr\n";
    asm("OUTS", cptr);
    //prints(cptr);

    cptr += 5;
    asm("OUTS", cptr);
    //prints(cptr);

    cptr = carr1;
    carr1[0] = 'h';
    carr1[1] = 'e';
    carr1[2] = 'l';
    carr1[3] = 'l';
    carr1[4] = 'o';
    carr1[5] = '\n';
    carr1[6] = 0;

    asm("OUTS", cptr);
    //prints(cptr);
}
