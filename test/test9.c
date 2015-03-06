int main()
{
    char *cptr;
    char carr[20];
    char carr1[51];

    carr[0] = 'x';
    carr[1] = '\n';
    printc(carr[0]);
    printc(carr[0]);
    printc(carr[1]);
    printc(carr[0]);
    printc(carr[0]);
    printc(carr[1]);
    print("The end\n");

    cptr = "this is a cptr\n";
    print(cptr);

    cptr += 5;
    print(cptr);

    cptr = carr1;
    carr1[0] = 'h';
    carr1[1] = 'e';
    carr1[2] = 'l';
    carr1[3] = 'l';
    carr1[4] = 'o';
    carr1[5] = '\n';
    carr1[6] = 0;

    print(cptr);
}
