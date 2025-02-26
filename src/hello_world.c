#include <stdio.h>
#include <unistd.h>

#ifdef __linux__
extern char **environ;
#endif

int main(int argc, char *argv[]) {
    printf("argc = %d\n", argc);
    for (int i = 0; i != argc; ++i) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    printf("argv[argc] = %p\n", (void *)argv[argc]);

    for (size_t i = 0;; ++i) {
        printf("environ[%zu] = %s\n", i, environ[i]);
        if (environ[i] == NULL) {
            break;
        }
    }
}
