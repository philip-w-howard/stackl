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
            print("EQ broken\n");
        else
            print("EQ works\n");
    else
        print("EQ broken\n");

    // NE
    x = 5;
    y = 6;
    z = 5;
    if (x != y)
        if (x != z)
            print("NE broken\n");
        else
            print("NE works\n");
    else
        print("NE broken\n");

    // AND
    if (5 && 4)
        if (4 && 0)
           print("AND broken\n");
       else
           print("AND works\n");
    else
        print("AND broken\n");

    // OR
    if ( (0 || 1) && (1 || 0) && (1 || 1))
        if (0 || 0)
           print("OR broken\n");
       else
           print("OR works\n");
    else
        print("OR broken\n");

    // GT
    if (5 > 4)
        if (4 > 4 || 4 > 5)
           print("GT broken\n");
       else
           print("GT works\n");
    else
        print("GT broken\n");

    // LT
    if (4 < 5)
        if (4 < 4 || 5 < 4)
           print("LT broken\n");
       else
           print("LT works\n");
    else
        print("LT broken\n");

    // GE
    if (5 >=4 && 4 >= 4)
        if (4 >= 5)
           print("GE broken\n");
       else
           print("GE works\n");
    else
        print("GE broken\n");

    // LE
    if (4 <= 5 && 4 <= 4)
        if (5 < 4)
           print("LE broken\n");
       else
           print("LE works\n");
    else
        print("LE broken\n");

    // PLUS
    x = 2;
    y = x + 1;
    if (y == 3)
        print("PLUS works\n");
    else
        print("PLUS broken\n");

    // MINUS
    z = 11-y;
    if (z == 8)
        print("MINUS works\n");
    else
        print("MINUS broken\n");

    // TIMES
    z = x*3;
    if (z == 6)
        print("TIMES works\n");
    else
        print("TIMES broken\n");

    // DIVIDE
    x = 2;
    y = 11/x;
    if (y == 5)
        print("DIVIDE works\n");
    else
        print("DIVIDE broken\n");

    // MOD
    z = 13;
    x = z % 5;
    if (x == 3)
        print("MOD works\n");
    else
        print("MOD broken\n");

    // NEG
    z = -11 + 20;
    if (z == 9)
        print("NEG works\n");
    else
        print("NEG broken\n");

    return 0;
}
