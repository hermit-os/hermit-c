#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define print_assert(expr)                                                     \
    {                                                                          \
        assert(expr);                                                          \
        printf("%s:%d: assert(%s) successful\n", __FILE__, __LINE__, #expr);   \
    }

#define print_assert_streq(left, right)                                        \
    {                                                                          \
        assert(strcmp(left, right) == 0);                                      \
        printf("%s:%d: assert_streq(%s, %s) successful\n", __FILE__, __LINE__, \
               #left, #right);                                                 \
    }
