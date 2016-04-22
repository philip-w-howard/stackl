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
            asm("OUTS", "EQ 1 broken\n");
        else
            asm("OUTS", "EQ works\n");
    else
        asm("OUTS", "EQ 2 broken\n");

    // NE
    x = 5;
    y = 6;
    z = 5;
    if (x != y)
        if (x != z)
            asm("OUTS", "NE 1 broken\n");
        else
            asm("OUTS", "NE works\n");
    else
        asm("OUTS", "NE 2 broken\n");

    // AND
    if (5 && 4)
        if (4 && 0)
           asm("OUTS", "AND 1 broken\n");
       else
           asm("OUTS", "AND works\n");
    else
        asm("OUTS", "AND 2 broken\n");

    // OR
    if ( (0 || 1) && (1 || 0) && (1 || 1))
        if (0 || 0)
           asm("OUTS", "OR 1 broken\n");
       else
           asm("OUTS", "OR works\n");
    else
        asm("OUTS", "OR 2 broken\n");

    // GT
    if (5 > 4)
        if (4 > 4 || 4 > 5)
           asm("OUTS", "GT 1 broken\n");
       else
           asm("OUTS", "GT works\n");
    else
        asm("OUTS", "GT 2 broken\n");

    // LT
    if (4 < 5)
        if (4 < 4 || 5 < 4)
           asm("OUTS", "LT 1 broken\n");
       else
           asm("OUTS", "LT works\n");
    else
        asm("OUTS", "LT 2 broken\n");

    // GE
    if (5 >=4 && 4 >= 4)
        if (4 >= 5)
           asm("OUTS", "GE 1 broken\n");
       else
           asm("OUTS", "GE works\n");
    else
        asm("OUTS", "GE 2 broken\n");

    // LE
    if (4 <= 5 && 4 <= 4)
        if (5 < 4)
           asm("OUTS", "LE 1 broken\n");
       else
           asm("OUTS", "LE works\n");
    else
        asm("OUTS", "LE 2 broken\n");

    // PLUS
    x = 2;
    y = x + 1;
    if (y == 3)
        asm("OUTS", "PLUS works\n");
    else
        asm("OUTS", "PLUS broken\n");

    // MINUS
    z = 11-y;
    if (z == 8)
        asm("OUTS", "MINUS works\n");
    else
        asm("OUTS", "MINUS broken\n");

    // TIMES
    z = x*3;
    if (z == 6)
        asm("OUTS", "TIMES works\n");
    else
        asm("OUTS", "TIMES broken\n");

    // DIVIDE
    x = 2;
    y = 11/x;
    if (y == 5)
        asm("OUTS", "DIVIDE works\n");
    else
        asm("OUTS", "DIVIDE broken\n");

    // MOD
    z = 13;
    x = z % 5;
    if (x == 3)
        asm("OUTS", "MOD works\n");
    else
        asm("OUTS", "MOD broken\n");

    // XOR
    x = 0x5A5AA5A5;
    z = x ^ 0x0F0F;
    if (z == 0x5A5AAAAA)
        asm("OUTS", "XOR works\n");
    else
        asm("OUTS", "XOR broken\n");

    // Bitwise AND
    x = 0x5A5AA5A5;
    z = x & 0x0F0F;
    if (z == 0x0505)
        asm("OUTS", "Bitwise AND works\n");
    else
        asm("OUTS", "Bitwise AND broken\n");

    // Bitwise OR
    x = 0x5A5AA5A5;
    z = x | 0x0F0F;
    if (z == 0x5A5AAFAF)
        asm("OUTS", "Bitwise OR works\n");
    else
        asm("OUTS", "Bitwise OR broken\n");

    // shift left
    z = 0x0555 << 1;
    if (z == 0x0AAA)
        asm("OUTS", "shift left works\n");
    else
        asm("OUTS", "shift left broken\n");
    
    // shift right
    x = z >> 1;
    if (x == 0x0555)
        asm("OUTS", "shift right works\n");
    else
        asm("OUTS", "shift right broken\n");

    // NEG
    x = 11;
    z = -x + 20;
    if (z == 9)
        asm("OUTS", "NEG works\n");
    else
        asm("OUTS", "NEG broken\n");

    // NOT
    x = 0x5A5AA5A5;
    z = !x;
    if (z == 0)
        asm("OUTS", "NOT 1 works\n");
    else
        asm("OUTS", "NOT 1 broken\n");

    z = 0;
    x = !z;
    if (x)
        asm("OUTS", "NOT 2 works\n");
    else
        asm("OUTS", "NOT 2 broken\n");

    // COMP
    x = 0x5A5AA5A5;
    z = ~x;

    if (z == 0xA5A55A5A)
        asm("OUTS", "COMP works\n");
    else
        asm("OUTS", "COMP broken\n");

    // PLUS_EQ
    z = 5;
    z += 7;
    if (z == 12)
        asm("OUTS", "PLUS_EQ works\n");
    else
        asm("OUTS", "PLUS_EQ broken\n");

    // MINUS_EQ
    z = 5;
    z -= 7;
    if (z == -2)
        asm("OUTS", "MINUS_EQ works\n");
    else
        asm("OUTS", "MINUS_EQ broken\n");

    // TIMES_EQ
    z = 5;
    z *= 7;
    if (z == 35)
        asm("OUTS", "TIMES_EQ works\n");
    else
        asm("OUTS", "TIMES_EQ broken\n");

    // DIVIDE_EQ
    z = 35;
    z /= 7;
    if (z == 5)
        asm("OUTS", "DIVIDE_EQ works\n");
    else
        asm("OUTS", "DIVIDE_EQ broken\n");

    // MOD_EQ
    z = 37;
    z %= 7;
    if (z == 2)
        asm("OUTS", "MOD_EQ works\n");
    else
        asm("OUTS", "MOD_EQ broken\n");

    // OR_EQ
    z = 0x0A5A5;
    z |= 0xFF;
    if (z == 0x0A5FF)
        asm("OUTS", "OR_EQ works\n");
    else
        asm("OUTS", "OR_EQ broken\n");

    // AND_EQ
    z = 0x0A5A5;
    z &= 0xFF;
    if (z == 0xA5)
        asm("OUTS", "AND_EQ works\n");
    else
        asm("OUTS", "AND_EQ broken\n");

    // XOR_EQ
    z = 0x0A5A5;
    z ^= 0xFF;
    if (z == 0x0A55A)
        asm("OUTS", "XOR_EQ works\n");
    else
        asm("OUTS", "XOR_EQ broken\n");

    // LEFT_EQ
    z = 0x0555;
    z <<= 2;
    if (z == 0x1554)
        asm("OUTS", "LEFT_EQ works\n");
    else
        asm("OUTS", "LEFT_EQ broken\n");

    // RIGHT_EQ
    z = 0x1554;
    z >>= 2;
    if (z == 0x0555)
        asm("OUTS", "RIGHT_EQ works\n");
    else
        asm("OUTS", "RIGHT_EQ broken\n");

    // post INC
    z = 35;
    x = z++;
    if (x == 35 && z == 36)
        asm("OUTS", "post INC works\n");
    else
        asm("OUTS", "post INC broken\n");

    // post DEC
    z = 37;
    x = z--;
    if (x == 37 && z == 36)
        asm("OUTS", "post DEC works\n");
    else
        asm("OUTS", "post DEC broken\n");

    // pre INC
    z = 36;
    x = ++z;
    if (x == 37 && z == 37)
        asm("OUTS", "pre INC works\n");
    else
        asm("OUTS", "pre INC broken\n");

    // pre DEC
    z = 36;
    x = --z;
    if (x == 35 && z == 35)
        asm("OUTS", "pre DEC works\n");
    else
        asm("OUTS", "pre DEC broken\n");
    
    return 0;
}
