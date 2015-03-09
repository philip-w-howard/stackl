int main()
{
    char *ptr;
    char stuff[100];
    int value;

    //ptr = "Enter 'this is a test': ";
    //print(ptr);
    ptr = stuff;
    gets(ptr);
    print(stuff);

    getl(ptr);
    print(stuff);
    print("\n");

    //ptr = "Enter '55': ";
    geti(value);
    print(value);
    print("\n");

    return 0;
}
/*END*/
