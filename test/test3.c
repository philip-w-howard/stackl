//#include <string.h>
//#include <system.h>

// test expressions and all operators
//
int main()
{
    int x;
    int y;
    int z;

    // EQUAL
    if (5 == 5)
        if (5 == 6)
            asm(OUTS_OP, "EQ 1 broken\n");
        else
            asm(OUTS_OP, "EQ works\n");
    else
        asm(OUTS_OP, "EQ 2 broken\n");

    // NE
    x = 5;
    y = 6;
    z = 5;
    if (x != y)
        if (x != z)
            asm(OUTS_OP, "NE 1 broken\n");
        else
            asm(OUTS_OP, "NE works\n");
    else
        asm(OUTS_OP, "NE 2 broken\n");

    // AND
    if (5 && 4)
        if (4 && 0)
           asm(OUTS_OP, "AND 1 broken\n");
       else
           asm(OUTS_OP, "AND works\n");
    else
        asm(OUTS_OP, "AND 2 broken\n");

    // OR
    if ( (0 || 1) && (1 || 0) && (1 || 1))
        if (0 || 0)
           asm(OUTS_OP, "OR 1 broken\n");
       else
           asm(OUTS_OP, "OR works\n");
    else
        asm(OUTS_OP, "OR 2 broken\n");

    // GT
    if (5 > 4)
        if (4 > 4 || 4 > 5)
           asm(OUTS_OP, "GT 1 broken\n");
       else
           asm(OUTS_OP, "GT works\n");
    else
        asm(OUTS_OP, "GT 2 broken\n");

    // LT
    if (4 < 5)
        if (4 < 4 || 5 < 4)
           asm(OUTS_OP, "LT 1 broken\n");
       else
           asm(OUTS_OP, "LT works\n");
    else
        asm(OUTS_OP, "LT 2 broken\n");

    // GE
    if (5 >=4 && 4 >= 4)
        if (4 >= 5)
           asm(OUTS_OP, "GE 1 broken\n");
       else
           asm(OUTS_OP, "GE works\n");
    else
        asm(OUTS_OP, "GE 2 broken\n");

    // LE
    if (4 <= 5 && 4 <= 4)
        if (5 < 4)
           asm(OUTS_OP, "LE 1 broken\n");
       else
           asm(OUTS_OP, "LE works\n");
    else
        asm(OUTS_OP, "LE 2 broken\n");

    // PLUS
    x = 2;
    y = x + 1;
    if (y == 3)
        asm(OUTS_OP, "PLUS works\n");
    else
        asm(OUTS_OP, "PLUS broken\n");

    // MINUS
    z = 11-y;
    if (z == 8)
        asm(OUTS_OP, "MINUS works\n");
    else
        asm(OUTS_OP, "MINUS broken\n");

    // TIMES
    z = x*3;
    if (z == 6)
        asm(OUTS_OP, "TIMES works\n");
    else
        asm(OUTS_OP, "TIMES broken\n");

    // DIVIDE
    x = 2;
    y = 11/x;
    if (y == 5)
        asm(OUTS_OP, "DIVIDE works\n");
    else
        asm(OUTS_OP, "DIVIDE broken\n");

    // MOD
    z = 13;
    x = z % 5;
    if (x == 3)
        asm(OUTS_OP, "MOD works\n");
    else
        asm(OUTS_OP, "MOD broken\n");

    // XOR
    x = 0x5A5AA5A5;
    z = x ^ 0x0F0F;
    if (z == 0x5A5AAAAA)
        asm(OUTS_OP, "XOR works\n");
    else
        asm(OUTS_OP, "XOR broken\n");

    // Bitwise AND
    x = 0x5A5AA5A5;
    z = x & 0x0F0F;
    if (z == 0x0505)
        asm(OUTS_OP, "Bitwise AND works\n");
    else
        asm(OUTS_OP, "Bitwise AND broken\n");

    // Bitwise OR
    x = 0x5A5AA5A5;
    z = x | 0x0F0F;
    if (z == 0x5A5AAFAF)
        asm(OUTS_OP, "Bitwise OR works\n");
    else
        asm(OUTS_OP, "Bitwise OR broken\n");

    // shift left
    z = 0x0555 << 1;
    if (z == 0x0AAA)
        asm(OUTS_OP, "shift left works\n");
    else
        asm(OUTS_OP, "shift left broken\n");
    
    // shift right
    x = z >> 1;
    if (x == 0x0555)
        asm(OUTS_OP, "shift right works\n");
    else
        asm(OUTS_OP, "shift right broken\n");

    // NEG
    x = 11;
    z = -x + 20;
    if (z == 9)
        asm(OUTS_OP, "NEG works\n");
    else
        asm(OUTS_OP, "NEG broken\n");

    // NOT
    x = 0x5A5AA5A5;
    z = !x;
    if (z == 0)
        asm(OUTS_OP, "NOT 1 works\n");
    else
        asm(OUTS_OP, "NOT 1 broken\n");

    z = 0;
    x = !z;
    if (x)
        asm(OUTS_OP, "NOT 2 works\n");
    else
        asm(OUTS_OP, "NOT 2 broken\n");

    // COMP
    x = 0x5A5AA5A5;
    z = ~x;

    if (z == 0xA5A55A5A)
        asm(OUTS_OP, "COMP works\n");
    else
        asm(OUTS_OP, "COMP broken\n");

    // PLUS_EQ
    z = 5;
    z += 7;
    if (z == 12)
        asm(OUTS_OP, "PLUS_EQ works\n");
    else
        asm(OUTS_OP, "PLUS_EQ broken\n");

    // MINUS_EQ
    z = 5;
    z -= 7;
    if (z == -2)
        asm(OUTS_OP, "MINUS_EQ works\n");
    else
        asm(OUTS_OP, "MINUS_EQ broken\n");

    // TIMES_EQ
    z = 5;
    z *= 7;
    if (z == 35)
        asm(OUTS_OP, "TIMES_EQ works\n");
    else
        asm(OUTS_OP, "TIMES_EQ broken\n");

    // DIVIDE_EQ
    z = 35;
    z /= 7;
    if (z == 5)
        asm(OUTS_OP, "DIVIDE_EQ works\n");
    else
        asm(OUTS_OP, "DIVIDE_EQ broken\n");

    // MOD_EQ
    z = 37;
    z %= 7;
    if (z == 2)
        asm(OUTS_OP, "MOD_EQ works\n");
    else
        asm(OUTS_OP, "MOD_EQ broken\n");

    // OR_EQ
    z = 0x0A5A5;
    z |= 0xFF;
    if (z == 0x0A5FF)
        asm(OUTS_OP, "OR_EQ works\n");
    else
        asm(OUTS_OP, "OR_EQ broken\n");

    // AND_EQ
    z = 0x0A5A5;
    z &= 0xFF;
    if (z == 0xA5)
        asm(OUTS_OP, "AND_EQ works\n");
    else
        asm(OUTS_OP, "AND_EQ broken\n");

    // XOR_EQ
    z = 0x0A5A5;
    z ^= 0xFF;
    if (z == 0x0A55A)
        asm(OUTS_OP, "XOR_EQ works\n");
    else
        asm(OUTS_OP, "XOR_EQ broken\n");

    // LEFT_EQ
    z = 0x0555;
    z <<= 2;
    if (z == 0x1554)
        asm(OUTS_OP, "LEFT_EQ works\n");
    else
        asm(OUTS_OP, "LEFT_EQ broken\n");

    // RIGHT_EQ
    z = 0x1554;
    z >>= 2;
    if (z == 0x0555)
        asm(OUTS_OP, "RIGHT_EQ works\n");
    else
        asm(OUTS_OP, "RIGHT_EQ broken\n");

    // post INC
    z = 35;
    x = z++;
    if (x == 36 && z == 36)
        asm(OUTS_OP, "post INC works\n");
    else
        asm(OUTS_OP, "post INC broken\n");

    // post DEC
    z = 37;
    x = z--;
    if (x == 36 && z == 36)
        asm(OUTS_OP, "post DEC works\n");
    else
        asm(OUTS_OP, "post DEC broken\n");

    // pre INC
    z = 36;
    x = ++z;
    if (x == 36 && z == 37)
        asm(OUTS_OP, "pre INC works\n");
    else
        asm(OUTS_OP, "pre INC broken\n");

    // pre DEC
    z = 36;
    x = --z;
    if (x == 36 && z == 35)
        asm(OUTS_OP, "pre DEC works\n");
    else
        asm(OUTS_OP, "pre DEC broken\n");
    
    return 0;
}
