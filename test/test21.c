// Make sure that const ints in the range of -128..127 are type char
// This test will generate compilation errors if it's broken
#include <sysio.h>

#define CONST1 0x23
const int CONST2 = 126;
int main()
{
    char var;

    var = 127;
    var = -128;
    var = CONST1;
    var = CONST2;

    prints("const chars properly detected\n");

    return 0;
}
