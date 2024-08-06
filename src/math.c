#include "print_assert.h"

#include <math.h>
#include <stdio.h>

void test_expf(void) {
    puts(__func__);
    print_assert(expf(1.0F) == (float)M_E);
}

int main(void) { test_expf(); }
