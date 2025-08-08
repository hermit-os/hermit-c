#include "print_assert.h"
#include <stdatomic.h>
#include <pthread.h>

_Atomic __uint128_t x = 0;

// NOLINTNEXTLINE(clang-diagnostic-unused-parameter)
static void *thread_start(void *arg) {
    for (size_t i = 0; i < 1000; ++i) {
        atomic_fetch_add_explicit(&x, 1, memory_order_relaxed);
    }

    return NULL;
}

int main(void) {
    pthread_t thr[10];

    for (size_t i = 0; i < 10; ++i) {
        pthread_create(&thr[i], NULL, thread_start, NULL);
    }

    for (size_t i = 0; i < 10; ++i) {
        pthread_join(thr[i], NULL);
    }

    print_assert(atomic_load_explicit(&x, memory_order_relaxed) == 10000);
}
