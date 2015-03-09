int f2();
int f1(int a, int b);
int f1(int a, int b)
{
    print(a + b);
}
int main()
{
    int ret;
    print(0);
    f1(5, 8);
    ret = f2();
    print(3);
    print(ret);
    ret = 2*f2() - 3;
    print(ret);
}
int f2()
{
    print(2);
    return 4;
}
/*END*/
