
#include "print_assert.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>

enum { ITERATIONS = 10000000 };

struct thread_info {
    pthread_t thread_id;
    _Atomic __uint128_t *count;
};

static void *thread_start(void *arg) {
    struct thread_info *info = arg;
    _Atomic __uint128_t *count = info->count;

    for (size_t i = 0; i < ITERATIONS; ++i) {
        atomic_fetch_add_explicit(count, 1, memory_order_relaxed);
    }

    return NULL;
}

int main(void) {
    size_t parallelism = 4;

    _Atomic __uint128_t count = 0;

    struct thread_info *thr_arg = malloc(parallelism * sizeof(*thr_arg));
    for (size_t i = 0; i < parallelism; ++i) {
        thr_arg[i].count = &count;

        pthread_create(&thr_arg[i].thread_id, NULL, &thread_start, &thr_arg[i]);
    }

    for (size_t i = 0; i < parallelism; ++i) {
        pthread_join(thr_arg[i].thread_id, NULL);
    }
    free(thr_arg);

    size_t count_value = atomic_load_explicit(&count, memory_order_relaxed);
    print_assert(count_value == parallelism * ITERATIONS);
}
