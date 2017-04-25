// Test copying data over from struct to struct
#include <string.h>
#include <sysio.h>

#define NULL 0

typedef struct tag_name {
   int a;
   struct tag_name *next;
} tag;

int main()
{   
    struct tag_name xtx; 
    struct tag_name btx;
    
    xtx.a = 12;
    xtx.next = NULL;
    btx = xtx;

    printi(btx.a);
    prints("\nTest J205 Complete\n"); 
    return 0;
}
