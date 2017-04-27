// This tests the type of logical operators. 
// Prior to stacklc V0.10.2-dev Apr 27 2017 08:01:55 this code
// generated a compiler error because of the GetType() on binary ops
// 
// This test is just verifying that the code compiles.
int main()
{
    char *msg;
    int *localInt;
    int bp;
    int lp;
    
    if (msg < 0 || msg >= (lp - bp) || localInt ) 
        lp = 1;

    asm("OUTS", "Bug in type of logical ops is fixed\n");

    return 0;
}
