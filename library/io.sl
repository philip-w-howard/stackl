#stackl V0.10.1-42-gf72ddff-dev stacklc
#once 
#once 
#library <string.sl>
.source /home2/philip.howard/bin/library/string.h 4
;  Source: /home2/philip.howard/bin/library/string.h Line: 4
.source /home2/philip.howard/bin/library/string.h 5
;  Source: /home2/philip.howard/bin/library/string.h Line: 5
.source /home2/philip.howard/bin/library/string.h 6
;  Source: /home2/philip.howard/bin/library/string.h Line: 6
.source /home2/philip.howard/bin/library/string.h 7
;  Source: /home2/philip.howard/bin/library/string.h Line: 7
.source /home2/philip.howard/bin/library/string.h 8
;  Source: /home2/philip.howard/bin/library/string.h Line: 8
.source /home2/philip.howard/bin/library/string.h 9
;  Source: /home2/philip.howard/bin/library/string.h Line: 9
.source /home2/philip.howard/bin/library/string.h 10
;  Source: /home2/philip.howard/bin/library/string.h Line: 10
.source /home2/philip.howard/bin/library/string.h 11
;  Source: /home2/philip.howard/bin/library/string.h Line: 11
.source /home2/philip.howard/bin/library/string.h 12
;  Source: /home2/philip.howard/bin/library/string.h Line: 12
.source /home2/philip.howard/bin/library/string.h 13
;  Source: /home2/philip.howard/bin/library/string.h Line: 13
.source test/io.c 4
;  Source: test/io.c Line: 4
.function prints
;  prints

prints:
.source test/io.c 6
;  Source: test/io.c Line: 6
PUSHVAR -12
OUTS
.source test/io.c 7
;  Source: test/io.c Line: 7
PUSH 0
RETURNV
.source test/io.c 18
;  Source: test/io.c Line: 18
PUSH 0
RETURNV
.source test/io.c 10
;  Source: test/io.c Line: 10
.function printi
;  printi

printi:
ADJSP 24
.source test/io.c 12
;  Source: test/io.c Line: 12
.source test/io.c 13
;  Source: test/io.c Line: 13
PUSH 0
PUSHFP
PLUS
PUSHVAR -12
CALL @itostr
SWAP
POP
SWAP
POP
POP
.source test/io.c 14
;  Source: test/io.c Line: 14
PUSH 0
PUSHFP
PLUS
CALL @prints
SWAP
POP
POP
.source test/io.c 15
;  Source: test/io.c Line: 15
PUSH 0
RETURNV
.source test/io.c 18
;  Source: test/io.c Line: 18
PUSH 0
RETURNV
