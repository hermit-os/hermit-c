#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#define print_assert(expr)                                                     \
    {                                                                          \
        assert(expr);                                                          \
        printf("%s:%d: assert(%s) successful\n", __FILE__, __LINE__, #expr);   \
    }
