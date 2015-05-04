The test cases in this directory can be used as regression tests to ensure newly added functionality
does not break things that worked previously. The regression tests can be ran using the ./regress
found in the directory. Below is a listing of what each test was designed to ensure following tests
could use properly:

TESTS
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
    8. prints
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
