//! Adapted from the examples on cppreference.com

#undef NDEBUG
#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_aligned(void *ptr, size_t align) {
    return ((uintptr_t)ptr & ~((uintptr_t)align - 1)) == (uintptr_t)ptr;
}

void test_malloc(void) {
    puts(__func__);

    char *p1 = malloc(4096 * sizeof *p1);
    assert(p1 != NULL);
    assert(is_aligned(p1, alignof(max_align_t)));

    char *p2 = malloc(sizeof *p2);
    assert(p2 != NULL);
    assert(is_aligned(p2, alignof(max_align_t)));

    int *p3 = malloc(4 * sizeof *p3);
    assert(p3 != NULL);
    assert(is_aligned(p3, alignof(max_align_t)));

    if (p3) {
        for (int n = 0; n < 4; ++n) {
            p3[n] = n * n;
        }
        for (int n = 0; n < 4; ++n) {
            assert(p3[n] == n * n);
        }
    }

    free(p1);
    free(p2);
    free(p3);
}

void test_calloc(void) {
    puts(__func__);

    char *p1 = calloc(4096, sizeof *p1);
    assert(p1 != NULL);
    assert(is_aligned(p1, alignof(max_align_t)));

    char *p2 = calloc(1, sizeof *p2);
    assert(p2 != NULL);
    assert(is_aligned(p2, alignof(max_align_t)));

    int *p3 = calloc(4, sizeof *p3);
    assert(p3 != NULL);
    assert(is_aligned(p3, alignof(max_align_t)));

    if (p3) {
        for (int n = 0; n < 4; ++n) {
            assert(p3[n] == 0);
        }
    }

    free(p1);
    free(p2);
    free(p3);
}

void test_realloc(void) {
    puts(__func__);

    const int pattern[] = {1, 2, 3, 4, 5, 6, 7, 8};
    const int pattern_size = sizeof pattern / sizeof(int);

    int *prev = NULL;
    int *next = malloc(pattern_size * sizeof *next);
    assert(next != NULL);
    assert(is_aligned(next, alignof(max_align_t)));
    memcpy(next, pattern, sizeof pattern);
    printf("%s location: %p. Size: %d ints (%ld bytes).\n",
           (next != prev ? "New" : "Old"), (void *)next, pattern_size,
           pattern_size * sizeof(int));

    const int realloc_size[] = {10, 12, 512, 32768, 65536, 32768};
    for (size_t i = 0; i != sizeof realloc_size / sizeof(int); ++i) {
        next = realloc(prev = next, realloc_size[i] * sizeof *next);
        assert(next != NULL);
        assert(is_aligned(next, alignof(max_align_t)));
        printf("%s location: %p. Size: %d ints (%ld bytes).\n",
               (next != prev ? "New" : "Old"), (void *)next, realloc_size[i],
               realloc_size[i] * sizeof(int));
        assert(!memcmp(next, pattern, sizeof pattern));
    }

    free(next);
}

void test_free(void) {
    puts(__func__);

    int *p1 = malloc(10 * sizeof *p1);
    assert(p1 != NULL);
    assert(is_aligned(p1, alignof(max_align_t)));
    free(p1);

    int *p2 = calloc(10, sizeof *p2);
    assert(p2 != NULL);
    assert(is_aligned(p2, alignof(max_align_t)));

    int *p3 = realloc(p2, 1000 * sizeof *p3);
    assert(p3 != NULL);
    assert(is_aligned(p3, alignof(max_align_t)));
    free(p3);
}

#ifdef __hermit__
// FIXME: remove once included via `stdlib.h`
void free_sized(void *ptr, size_t size);
void free_aligned_sized(void *ptr, size_t alignment, size_t size);

void test_free_sized(void) {
    puts(__func__);

    int *p1 = malloc(10 * sizeof *p1);
    assert(p1 != NULL);
    assert(is_aligned(p1, alignof(max_align_t)));
    free_sized(p1, 10 * sizeof *p1);

    int *p2 = calloc(10, sizeof *p2);
    assert(p2 != NULL);
    assert(is_aligned(p2, alignof(max_align_t)));

    int *p3 = realloc(p2, 1000 * sizeof *p3);
    assert(p3 != NULL);
    assert(is_aligned(p3, alignof(max_align_t)));
    free_sized(p3, 1000 * sizeof *p3);
}

void test_free_aligned_sized(void) {
    puts(__func__);

    int *p1 = aligned_alloc(alignof(max_align_t), 10 * sizeof *p1);
    assert(p1 != NULL);
    assert(is_aligned(p1, alignof(max_align_t)));
    free_aligned_sized(p1, alignof(max_align_t), 10 * sizeof *p1);

    char *p2 = aligned_alloc(alignof(char), 10 * sizeof *p2);
    assert(p2 != NULL);
    assert(is_aligned(p2, alignof(char)));
    free_aligned_sized(p2, alignof(char), 10 * sizeof *p2);

    int *p3 = aligned_alloc(1024, 1024 * sizeof *p3);
    assert(p3 != NULL);
    assert(is_aligned(p3, 1024));
    free_aligned_sized(p3, 1024, 1024 * sizeof *p3);
}
#endif // __hermit__

void test_aligned_alloc(void) {
    puts(__func__);

    int *p1 = aligned_alloc(1024, 1024 * sizeof *p1);
    assert(is_aligned(p1, 1024));
    free(p1);
}

int main(void) {
    test_malloc();
    test_calloc();
    test_realloc();
    test_free();
#ifdef __hermit__
    test_free_sized();
    test_free_aligned_sized();
#endif
    test_aligned_alloc();
}
