The test cases in this directory can be used as regression tests to ensure 
newly added functionality does not break things that worked previously. The 
regression tests can be ran using the ./regress found in the directory. Below 
is a listing of what each test was designed to ensure following tests could 
use properly. Please keep in mind that both the tests and this file are works 
in progress.

TESTS
Include files
Separate compilation
Makefile?

1) outs
2) if (const) else
3) Expressions with all operators
    NOTE: Also includes nested ifs
4) for loop
    NOTE: Also includes io.h
5) while loop
6) Character arrays and pointers
    Assigment ptr = array
    pointer arithmetic
7) string library
    string functions including printi
8) Function calls
    return values
    functions in expressions
9) Pointer arithmetic
10) void functions
11) function call via function address
12) variable init on decl line
13) Structs
14) Mixing decls and statements
    #define
    global consts with initializers
15) do while test
16) checks that GetType() for binary logical ops works
17) sizeof
18) Test return value of functions
    empty if's and while's
19) Scopes
20) Global variables accessed in different functions
21) Make sure const ints are correct type (char/int)
22) Hex values and bitwise operators (more earlier?) ###################
23) Addresses and derefferences of multple stars
    check for completeness and validity     ###################
24) More addresses and derefference (combine with 23) ###################
25) Global multi-star pointers
26) more structs? No sure of diff from 13
27) More pointer stuff
28) IO block struct test
29) Global vs. local pointers
30) Arrays of structs
31) arrays of structs of arrays of structs
32) Global arrays of structs of ...
33) Multidimensional arrays
34) Pass Address of struct to func
35) Pass address of struct to func
36) Precedence of ###################
37) Interrupts, pragmas, and the flag reg
38) Type casts
39) Test that pre-inc doesn't mess up the stack
40) Test trap handlers



n) use interrupt to test stack behavior for
    all operators as expressions
    all operators in assignment statements
m) semantic checks
