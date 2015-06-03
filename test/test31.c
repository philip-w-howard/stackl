#include <string.h>
#include <system.h>

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

int main()
{
    int index;
    int ii;
    int jj;
    struct2_t data[3];
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

    while (index > 0)
    {
        index--;
        if (datap[index-1] != index) 
        {
            prints("Error at offset ");
            printi(index);
            prints("\n");
        }
    }

    prints("Test complete\n");

    return 0;
}
