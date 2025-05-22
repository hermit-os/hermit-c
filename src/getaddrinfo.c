#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

static int print_addrinfo(const char *nodename) {
    struct addrinfo hints = {0};
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *res;

    fprintf(stderr, "Resolving %s\n", nodename);
    int error = getaddrinfo(nodename, NULL, &hints, &res);
    if (error != 0) {
        if (error == EAI_SYSTEM) {
            perror("getaddrinfo");
        } else {
            fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
        }
        return -1;
    }

    for (struct addrinfo *cur = res; cur != NULL; cur = cur->ai_next) {
        struct sockaddr_storage *storage =
            (struct sockaddr_storage *)cur->ai_addr;

        if (storage->ss_family != AF_INET) {
            continue;
        }

        struct sockaddr_in *addr = (struct sockaddr_in *)storage;

        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr->sin_addr, str, INET_ADDRSTRLEN);

        fprintf(stderr, "%s\n", str);
    }

    freeaddrinfo(res);

    return 0;
}

int main(void) {
    if (print_addrinfo("example.com") == -1) {
        return EXIT_FAILURE;
    }

    if (print_addrinfo("github.com") == -1) {
        return EXIT_FAILURE;
    }

    if (print_addrinfo("rwth-aachen.de") == -1) {
        return EXIT_FAILURE;
    }
}
