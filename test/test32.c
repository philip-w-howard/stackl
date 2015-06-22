// Test global arrays or structs of arrays of structs
//
#include <string.h>
#include <io.h>

typedef struct
{
    int a;
    int b[3];
} struct1_t;

typedef struct
{
    int a[3];
    struct1_t s1[2];
    int b;
} struct2_t;

struct2_t data[3];

int main()
{
    int index;
    int ii;
    int jj;
    int *datap;

    datap = data;

    index = 1;
    for (ii=0; ii<3; ii++)
    {
        data[ii].a[0] = index;
        index++;
        data[ii].a[1] = index;
        index++;
        data[ii].a[2] = index;
        index++;

        for (jj=0; jj<2; jj++)
        {
            data[ii].s1[jj].a = index;
            index++;
            data[ii].s1[jj].b[0] = index;
            index++;
            data[ii].s1[jj].b[1] = index;
            index++;
            data[ii].s1[jj].b[2] = index;
            index++;
        }

        data[ii].b = index;
        index++;
    }

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
