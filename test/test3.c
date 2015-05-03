#include <string.h>
#include <system.h>

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
            prints("EQ broken\n");
        else
            prints("EQ works\n");
    else
        prints("EQ broken\n");

    // NE
    x = 5;
    y = 6;
    z = 5;
    if (x != y)
        if (x != z)
            prints("NE broken\n");
        else
            prints("NE works\n");
    else
        prints("NE broken\n");

    // AND
    if (5 && 4)
        if (4 && 0)
           prints("AND broken\n");
       else
           prints("AND works\n");
    else
        prints("AND broken\n");

    // OR
    if ( (0 || 1) && (1 || 0) && (1 || 1))
        if (0 || 0)
           prints("OR broken\n");
       else
           prints("OR works\n");
    else
        prints("OR broken\n");

    // GT
    if (5 > 4)
        if (4 > 4 || 4 > 5)
           prints("GT broken\n");
       else
           prints("GT works\n");
    else
        prints("GT broken\n");

    // LT
    if (4 < 5)
        if (4 < 4 || 5 < 4)
           prints("LT broken\n");
       else
           prints("LT works\n");
    else
        prints("LT broken\n");

    // GE
    if (5 >=4 && 4 >= 4)
        if (4 >= 5)
           prints("GE broken\n");
       else
           prints("GE works\n");
    else
        prints("GE broken\n");

    // LE
    if (4 <= 5 && 4 <= 4)
        if (5 < 4)
           prints("LE broken\n");
       else
           prints("LE works\n");
    else
        prints("LE broken\n");

    // PLUS
    x = 2;
    y = x + 1;
    if (y == 3)
        prints("PLUS works\n");
    else
        prints("PLUS broken\n");

    // MINUS
    z = 11-y;
    if (z == 8)
        prints("MINUS works\n");
    else
        prints("MINUS broken\n");

    // TIMES
    z = x*3;
    if (z == 6)
        prints("TIMES works\n");
    else
        prints("TIMES broken\n");

    // DIVIDE
    x = 2;
    y = 11/x;
    if (y == 5)
        prints("DIVIDE works\n");
    else
        prints("DIVIDE broken\n");

    // MOD
    z = 13;
    x = z % 5;
    if (x == 3)
        prints("MOD works\n");
    else
        prints("MOD broken\n");

    // NEG
    z = -11 + 20;
    if (z == 9)
        prints("NEG works\n");
    else
        prints("NEG broken\n");

    // PLUS_EQ
    z = 5;
    z += 7;
    if (z == 12)
        prints("PLUS_EQ works\n");
    else
        prints("PLUS_EQ broken\n");

    // MINUS_EQ
    z = 5;
    z -= 7;
    if (z == -2)
        prints("MINUS_EQ works\n");
    else
        prints("MINUS_EQ broken\n");

    // TIMES_EQ
    z = 5;
    z *= 7;
    if (z == 35)
        prints("TIMES_EQ works\n");
    else
        prints("TIMES_EQ broken\n");

    // DIVIDE_EQ
    z = 35;
    z /= 7;
    if (z == 5)
        prints("DIVIDE_EQ works\n");
    else
        prints("DIVIDE_EQ broken\n");

    // post INC
    z = 35;
    z++;
    if (z == 36)
        prints("post INC works\n");
    else
        prints("post INC broken\n");

    // pre INC
    ++z;
    if (z == 37)
        prints("pre INC works\n");
    else
        prints("pre INC broken\n");

    // post DEC
    z--;
    if (z == 36)
        prints("post DEC works\n");
    else
        prints("post DEC broken\n");

    // pre DEC
    --z;
    if (z == 35)
        prints("pre DEC works\n");
    else
        prints("pre DEC broken\n");

    z = 0xBAD;
    z = ~z;
    if (z == 0xFFFFF452)
        prints("Complement works\n");
    else
        prints("Complement broken\n");

    z = ~z;
    if (z == 0xBAD)
        prints("Complement works\n");
    else
        prints("Complement broken\n");
    
    z = 0x0555 << 1;
    if (z == 0x0AAA)
        prints("shift left works\n");
    else
        prints("shift left broken\n");
    
    x = z >> 1;
    if (x == 0x0555)
        prints("shift right works\n");
    else
        prints("shift right broken\n");

    z = x;

    z <<= 4;
    if (z == 0x5550)
        prints("shift left equal works\n");
    else
        prints("shift left equal broken\n");

    z >>= 3;
    if (z == 0x0AAA)
        prints("shift right equal works\n");
    else
        prints("shift right equal broken\n");

    return 0;
}
