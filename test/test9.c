#include "test/io.h"

typedef struct
{
    int aa;
    int bb;
    char cc;
} struct_t;

int main()
{
    prints("Testing pointer arithmetic\n");

    {
        char data[10];
        int ii;

        for (ii=0; ii<10; ii++)
        {
            data[ii] = ii;
        }

        char *ptr;
        ptr = data;
        for (ii=0; ii<10; ii++)
        {
            if (*ptr++ != data[ii]) 
            {
                prints("pointer error ");
                printi(ptr);
                prints(" ");
                printi(&data[ii]);
                prints("\n");
            }
            else
            {
                prints("Index ");
                printi(ii);
                prints(" OK\n");
            }
        }
    }

    {
        char *cptr;
        int  *iptr;
        struct_t *sptr;
        int  ii;

        cptr = 0;
        iptr = 0;
        sptr = 0;
        for (ii=0; ii<10; ii++)
        {
            if (cptr++ != ii) 
                prints("char pointer arithmetic error\n");
            else
                prints("char pointer arithmetic OK\n");
            if (iptr++ != ii*4) 
                prints("int pointer arithmetic error\n");
            else
                prints("int pointer arithmetic OK\n");
            if (sptr++ != ii*sizeof(struct_t)) 
                prints("struct pointer arithmetic error\n");
            else
                prints("struct pointer arithmetic OK\n");
        }
    }

    prints("Test complete\n");
    return 0;
}
