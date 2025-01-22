#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    struct timespec tp;
    int gettime_res = clock_gettime(CLOCK_REALTIME, &tp);
    if (gettime_res == -1) {
        perror("clock_gettime() failed");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "%jd seconds since the epoch began\n", (intmax_t)tp.tv_sec);

    struct tm localtime_result;
    struct tm *localtime_ptr = localtime_r(&tp.tv_sec, &localtime_result);
    if (localtime_ptr == NULL) {
        perror("localtime_r() failed");
        return EXIT_FAILURE;
    }

    char localtime_buf[26];
    // NOLINTNEXTLINE(bugprone-unsafe-functions)
    char *localtime_str = asctime_r(localtime_ptr, localtime_buf);
    if (localtime_str == NULL) {
        perror("asctime_r() failed");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "local time: %s", localtime_str);

    struct tm gmtime_result;
    struct tm *gmtime_ptr = gmtime_r(&tp.tv_sec, &gmtime_result);
    if (gmtime_ptr == NULL) {
        perror("gmtime_r() failed");
        return EXIT_FAILURE;
    }

    char gmtime_buf[26];
    // NOLINTNEXTLINE(bugprone-unsafe-functions)
    char *gmtime_str = asctime_r(gmtime_ptr, gmtime_buf);
    if (gmtime_str == NULL) {
        perror("asctime_r() failed");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "UTC time: %s", gmtime_str);
}
