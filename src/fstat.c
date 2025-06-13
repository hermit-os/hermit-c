#include "print_assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int test_stdio(int fildes) {
    struct stat buf;

    int fstat_status = fstat(fildes, &buf);
    if (fstat_status == -1) {
        perror("fstat() failed");
        return -1;
    }

    fprintf(stderr, "mode = %#o\n", buf.st_mode);
#ifdef __hermit__
    print_assert((buf.st_mode & S_IFCHR) == S_IFCHR);
#endif /* __hermit__ */

    return 0;
}

int main(void) {
    int stdin_status = test_stdio(STDIN_FILENO);
    if (stdin_status == -1) {
        return EXIT_FAILURE;
    }

    int stdout_status = test_stdio(STDOUT_FILENO);
    if (stdout_status == -1) {
        return EXIT_FAILURE;
    }

    int stderr_status = test_stdio(STDERR_FILENO);
    if (stderr_status == -1) {
        return EXIT_FAILURE;
    }
}
