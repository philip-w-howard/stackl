// Test that return types and existence matches function delcaration
int foo()
{
    return;
}

void bar()
{
    return 1;
}

int main()
{
    foo();
    bar();
    return 0;
}
