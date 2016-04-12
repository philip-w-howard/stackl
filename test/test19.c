#include <string.h>
#include "test/io.h"

int main()
{
    int a;
    a = 5;
    {
        int b;
        b = 4;
        {
            int c;
            c = 3;
            {
                int d;
                d = 2;
                {
                    int e;
                    e = 1;
                    {
                        int f;
                        f = 0;

                        printi(f);
                        prints(" ");
                        printi(e);
                        prints(" ");
                        printi(d);
                    }
                    prints(" ");
                    printi(c);
                }
                prints(" ");
                printi(b);
            }
            prints(" ");
            printi(a);
        }
    }

    prints("\n");
}
