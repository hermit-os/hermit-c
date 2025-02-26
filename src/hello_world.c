#include <stdio.h>
#include <unistd.h>

#ifdef __linux__
extern char **environ;
#endif

int main(int argc, char *argv[]) {
    printf("argc = %d\n", argc);
    for (int ndx = 0; ndx != argc; ++ndx) {
        printf("argv[%d] --> %s\n", ndx, argv[ndx]);
    }
    printf("argv[argc] = %p\n", (void *)argv[argc]);

    for (size_t i = 0;; ++i) {
        printf("environ[%zu] = %s\n", i, environ[i]);
        if (environ[i] == NULL) {
            break;
        }
    }
}
