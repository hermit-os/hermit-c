#include "print_assert.h"

#include <math.h>
#include <stdio.h>

void test_expf(void) {
    puts(__func__);
    float (*ptr)(float arg) = &expf;

    print_assert(ptr(1.0F) == (float)M_E);
}

void test_modf(void) {
    puts(__func__);
    double (*ptr)(double arg, double *iptr) = &modf;

    double f3;
    double f2 = ptr(123.25, &f3);
    print_assert(f2 == 0.25);
    print_assert(f3 == 123.0);
}

int main(void) {
    test_expf();
    test_modf();
}
