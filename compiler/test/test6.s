int f1();
int f2()
{
    print(2);
    return 4;
}
int main()
{
    int ret;
    print(0);
    f1();
    ret = f2();
    print(3);
    print(ret);
    ret = 2*f2() - 3;
    print(ret);
}
int f1()
{
    print(1);
}
