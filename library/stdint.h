#pragma once
#include <stddef.h>

// Exact-width integer types
typedef char            int8_t;
typedef int             int32_t;

// Minimum-width integer types
typedef char            int_least8_t;
typedef short           int_least16_t;
typedef int             int_least32_t;

// Fastest minimum-width integer types
typedef char            int_fast8_t;
typedef int             int_fast16_t;
typedef int             int_fast32_t;

// Integer types capable of holding object pointers
typedef int             intptr_t;

// Greatest-width integer types
typedef int             intmax_t;

// Limits of exact-width integer types
#define INT8_MIN        -128
#define INT8_MAX        127
#define INT32_MIN       -2147483648
#define INT32_MAX       2147483647

// Limits of fastest minimum-width integer types
#define INT_FAST8_MIN   -128
#define INT_FAST8_MAX   127
#define INT_FAST32_MIN  -2147483648
#define INT_FAST32_MAX  2147483647

// Limits of integer types capable of holding object pointers
#define INTPTR_MIN      -2147483648
#define INTPTR_MAX      2147483647

// Limits of greatest-width integer types
#define INTMAX_MIN       -2147483648
#define INTMAX_MAX       2147483647
