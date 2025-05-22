#include "print_assert.h"
#include <limits.h>
#include <stdint.h>

#ifdef __linux__
#define LONG_BIT 64
#define WORD_BIT 32
#endif

void numerical_limits(void) {
    print_assert(CHAR_BIT == 8);
    print_assert(CHAR_MAX == INT8_MAX);
    print_assert(CHAR_MIN == INT8_MIN);
    print_assert(INT_MAX == INT32_MAX);
    print_assert(INT_MIN == INT32_MIN);
    print_assert(LLONG_MAX == INT64_MAX);
    print_assert(LLONG_MIN == INT64_MIN);
    print_assert(LONG_BIT == 64);
    print_assert(LONG_MAX == INT64_MAX);
    print_assert(LONG_MIN == INT64_MIN);
#ifdef __hermit__
    print_assert(MB_LEN_MAX == 4);
#endif
    print_assert(SCHAR_MAX == INT8_MAX);
    print_assert(SCHAR_MIN == INT8_MIN);
    print_assert(SHRT_MAX == INT16_MAX);
    print_assert(SHRT_MIN == INT16_MIN);
    print_assert(SSIZE_MAX == INT64_MAX);
    print_assert(UCHAR_MAX == UINT8_MAX);
    print_assert(UINT_MAX == UINT32_MAX);
    print_assert(ULLONG_MAX == UINT64_MAX);
    print_assert(ULONG_MAX == UINT64_MAX);
    print_assert(USHRT_MAX == UINT16_MAX);
    print_assert(WORD_BIT == 32);
}

int main(void) {
    numerical_limits();
}
