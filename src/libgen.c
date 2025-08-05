#include "print_assert.h"
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

// As of POSIX 2024, this dirname and basename are thread-safe
// FIXME: remove this once `concurrency-mt-unsafe.FunctionSet: posix` silences
// this.
// NOLINTBEGIN(concurrency-mt-unsafe)
int main(void) {
    char *path = strdup("/foo/bar");
    print_assert_streq(dirname(path), "/foo");
    print_assert_streq(dirname(path), "/");
    print_assert_streq(dirname(path), "/");
    free(path);

    char *relative_path = strdup("foo/bar");
    print_assert_streq(dirname(relative_path), "foo");
    print_assert_streq(dirname(relative_path), ".");
    free(relative_path);

    char *a = strdup("/usr/bin/");
    print_assert_streq(basename(a), "bin");
    free(a);

    char *b = strdup("tmp/foo.txt");
    print_assert_streq(basename(b), "foo.txt");
    free(b);

    char *c = strdup("foo.txt/.");
    print_assert_streq(basename(c), ".");
    free(c);

    char *d = strdup("foo.txt/.//");
    print_assert_streq(basename(d), ".");
    free(d);

    char *e = strdup("foo.txt/..");
    print_assert_streq(basename(e), "..");
    free(e);

    char *f = strdup("/");
    print_assert_streq(basename(f), "/");
    free(f);
}
// NOLINTEND(concurrency-mt-unsafe)
