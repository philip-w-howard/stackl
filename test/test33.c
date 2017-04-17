// Test multidimensional arrays
//
#include <string.h>
#include <sysio.h>

int main()
{
    int index;
    int ii;
    int jj;
    int kk;
    int *datap;
    int data[4][3][2];

    datap = &data[0][0][0];

    index = 1;
    for (ii=0; ii<4; ii++)
    {
        for (jj=0; jj<3; jj++)
        {
            for (kk=0; kk<2; kk++)
            {
                prints("Setting ");
                printi(ii);
                prints(" ");
                printi(jj);
                prints(" ");
                printi(kk);
                prints(" to ");
                printi(index);
                prints("\n");
                data[ii][jj][kk] = index;
                index++;
            }
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
