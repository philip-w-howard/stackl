#include <sysio.h>

int main() {
    int result = 0;

    // test shiftl-like semantics
    asm("push 15"); // push 0x0f
    asm("push 4");
    // 0xf << 4
    result = asm("rol");
    printx(result);
    prints("\n");

    // test shiftr-like semantics
    asm("push 240"); // push 0xf0
    asm("push 4");
    result = asm("ror");
    printx(result);
    prints("\n");

    // test full rol semantics
    asm("push 4294901760"); // push 0xffff0000
    asm("push 16");
    result = asm("rol");
    printx(result);
    prints("\n");

    // test ror semantics
    asm("push 65535"); // push 0x0000ffff
    asm("push 16");
    result = asm("rol");
    printx(result);
    prints("\n");

    // test rol semantics
    asm("push 4294901760"); // push 0xffff0000
    asm("push 8");
    result = asm("rol");
    printx(result);
    prints("\n");
    
    // test ror semantics
    asm("push 65535"); // push 0x0000ffff
    asm("push 8");
    result = asm("rol");
    printx(result);
    prints("\n");
}
