#undef NDEBUG
#include <assert.h>
#include <stdlib.h>

#define vec_impl(name, ty)                                                     \
                                                                               \
    struct name {                                                              \
        ty *ptr;                                                               \
        size_t cap;                                                            \
        size_t len;                                                            \
    };                                                                         \
                                                                               \
    /* NOLINTNEXTLINE(clang-diagnostic-unused-function) */                     \
    static inline struct name name##_new(void) {                               \
        struct name v = {                                                      \
            .ptr = NULL,                                                       \
            .cap = 0,                                                          \
            .len = 0,                                                          \
        };                                                                     \
        return v;                                                              \
    }                                                                          \
                                                                               \
    /* NOLINTNEXTLINE(clang-diagnostic-unused-function) */                     \
    static inline void name##_grow_one(struct name *v) {                       \
        if (v->cap == 0) {                                                     \
            v->cap = 1;                                                        \
        } else {                                                               \
            v->cap *= 2;                                                       \
        }                                                                      \
        /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                       \
        ty *ptr = (ty *)realloc(v->ptr, v->cap * sizeof(ty));                  \
        assert(ptr != NULL);                                                   \
        v->ptr = ptr;                                                          \
    }                                                                          \
                                                                               \
    /* NOLINTNEXTLINE(clang-diagnostic-unused-function) */                     \
    static inline void name##_push(struct name *v, ty value) {                 \
        if (v->len == v->cap) {                                                \
            name##_grow_one(v);                                                \
        }                                                                      \
                                                                               \
        *(v->ptr + v->len) = value;                                            \
        v->len += 1;                                                           \
    }                                                                          \
                                                                               \
    /* NOLINTNEXTLINE(clang-diagnostic-unused-function) */                     \
    static inline ty name##_pop(struct name *v) {                              \
        assert(v->len != 0);                                                   \
                                                                               \
        v->len -= 1;                                                           \
        return *(v->ptr + v->len);                                             \
    }                                                                          \
                                                                               \
    /* NOLINTNEXTLINE(clang-diagnostic-unused-function) */                     \
    static inline ty name##_swap_remove(struct name *v, size_t index) {        \
        assert(index < v->len);                                                \
                                                                               \
        ty value = *(v->ptr + index);                                          \
                                                                               \
        v->len -= 1;                                                           \
        *(v->ptr + index) = *(v->ptr + v->len);                                \
                                                                               \
        return value;                                                          \
    }                                                                          \
                                                                               \
    /* NOLINTNEXTLINE(clang-diagnostic-unused-function) */                     \
    static inline void name##_drop(struct name *v) {                           \
        free(v->ptr);                                                          \
        *v = name##_new();                                                     \
    }
