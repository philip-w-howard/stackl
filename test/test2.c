// test if/else
int main()
{
    if (1)
        asm("OUTS", "if works\n");
    else
        asm("OUTS", "if 1 broken\n");

    if (0)
        asm("OUTS", "if 2 broken\n");
    else
        asm("OUTS", "if-else works\n");

    return 0;
}

