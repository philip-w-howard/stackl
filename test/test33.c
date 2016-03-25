// Test multidimensional arrays
//
#include <string.h>
#include <io.h>

int main()
{
    int index;
    int ii;
    int jj;
    int *datap;
    int data[3][2];

    datap = &data[0][0];

    index = 1;
    for (ii=0; ii<3; ii++)
    {
        for (jj=0; jj<2; jj++)
        {
            prints("Setting ");
            printi(ii);
            prints(" ");
            printi(jj);
            prints(" to ");
            printi(index);
            prints("\n");
            asm("NOP");
            asm("NOP");
            asm("NOP");
            asm("NOP");
            data[ii][jj] = index;
            index++;
        }
    }

    prints("Index reached ");
    printi(index);
    prints("\n");

    index--;
    while (index > 0)
    {
        index--;
        if (datap[index] != index+1) 
        {
            prints("Error at offset ");
            printi(index);
            prints(" ");
            printi(datap[index]);
            prints("\n");
        }
        else
        {
            prints("Data at index ");
            printi(index);
            prints(" is OK\n");
        }
    }

    prints("Test complete\n");

    return 0;
}
