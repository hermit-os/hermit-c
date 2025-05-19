#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

int main(void) {
    struct utsname name;
    int unamed = uname(&name);
    if (unamed == -1) {
        perror("uname() failed");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "sysname = %s\n", name.sysname);
    fprintf(stderr, "nodename = %s\n", name.nodename);
    fprintf(stderr, "release = %s\n", name.release);
    fprintf(stderr, "version = %s\n", name.version);
    fprintf(stderr, "machine = %s\n", name.machine);
    printf("%s %s %s %s %s\n", name.sysname, name.nodename, name.release,
           name.version, name.machine);
}
