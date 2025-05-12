// Test copying data over from struct to struct
// NOTE: the compiler doesn't support this, so this should generate an error
#include <string.h>
#include <sysio.h>

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
    printi((int)btx.next);
    prints("\nTest J205 Complete\n"); 
    return 0;
}
