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
    Use separate compilation                ###################
    Add other string function tests         ###################
8) Function calls
    return values
    functions in expressions
9) Unused
10) string library: Combine with Test 7     ###################
    check that all string functions are tested
11) More string library. Combite with 7     ###################
12) More string library. Combite with 7     ###################
13) Structs
14) Mixing decls and statements
    #define
    global consts with initializers
15) Unused
16) Short ciruit nature of || and && (should come earlier)  ###################
17) sizeof
    Incomplete                              ###################
18) Test return value of functions
    empty if's and while's
19) Scopes
20) Global variables accessed in different functions
21) Unused
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



n) use interrupt to test stack behavior for
    all operators as expressions
    all operators in assignment statements
m) semantic checks
    1. printi; prints
    2. variable declaration; variable assignment
    3. if statements; else statements
    4. operators
        x == y
        x != y
        x && y
        x || y
        x > y
        x < y
        x >= y
        x <= y
        x + y
        x = y
        x * y
        x / y
        x & y
        -x
        x += y
        x -= y
        x *= y
        x /= y
        x++
        ++x
        x--
        --x
        ~x
        x << y
        x >> y
        x <<= y
        x >>= y
    5. while loops
    6. for loops
    7. function calls; return statements
    9. arrays; pointers; array[x] assignment; array[x] reference; ptr[x] assignment; ptr[x] reference
    10. strcpy; strncpy; strchr
    11. strcmp
    12. strrev; itostr
    13. struct field assignment; struct field reference
    14. mixing declarations and statements; consts; defines
    15. exec
    16. short circuiting && and ||
    17. sizeof operator
    18. empty blocks; empty statements
    19. correct block stack manipulaation
    20. global variables
    21. scanl; scans; scani; exit
    22. bitwise operators
        |
        &
        |=
        &=
        ^=
    23. generic type pointers; ptr[x] reference
    24. generic type pointers; \*ptr reference
    25. generic type poinetrs; \*ptr reference
    26. proper pointer field dereferencing
    27. pointer artihemetic; \*ptr reference
    28. \*ptr reference
    30. syscall; systrap; exit; startup; interrupt
