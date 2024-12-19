#include "print_assert.h"

_Thread_local unsigned int a = 0x01234567;
_Thread_local unsigned int b = 0x89abcdef;

int main(void) {
    print_assert(a == 0x01234567);
    print_assert(b == 0x89abcdef);
}
