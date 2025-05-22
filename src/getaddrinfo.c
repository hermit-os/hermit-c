#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

static void print_addrinfo(const struct addrinfo *ai) {
    for (const struct addrinfo *cur = ai; cur != NULL; cur = cur->ai_next) {
        fprintf(stderr, "ai_flags = %#4x    ", cur->ai_flags);
        fprintf(stderr, "ai_family = %#4x    ", cur->ai_family);
        fprintf(stderr, "ai_socktype = %#4x    ", cur->ai_socktype);
        fprintf(stderr, "ai_protocol = %#4x    ", cur->ai_protocol);
        fprintf(stderr, "ai_canonname = %s    ", cur->ai_canonname);

        const struct sockaddr_storage *storage =
            (const struct sockaddr_storage *)cur->ai_addr;

        if (storage == NULL) {
            fprintf(stderr, "no address\n");
            continue;
        }

        switch (storage->ss_family) {
        case AF_INET: {
            struct sockaddr_in *addr = (struct sockaddr_in *)storage;

            fprintf(stderr, "sin_port = %5i    ", ntohs(addr->sin_port));

            char str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr->sin_addr, str, INET_ADDRSTRLEN);
            fprintf(stderr, "sin_addr = %s\n", str);
            break;
        }

        case AF_INET6: {
            struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;

            fprintf(stderr, "sin_port = %5i    ", ntohs(addr6->sin6_port));

            char str6[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &addr6->sin6_addr, str6, INET6_ADDRSTRLEN);
            fprintf(stderr, "sin_addr = %s\n", str6);
            break;
        }

        default: {
            fprintf(stderr, "Unknown address family %u\n", storage->ss_family);
            break;
        }
        }
    }
}

static int test_addrinfo(const char *restrict nodename,
                         const char *restrict servname,
                         const struct addrinfo *restrict hints) {
    fprintf(stderr, "Resolving %s://%s with hints:\n", servname, nodename);
    print_addrinfo(hints);

    struct addrinfo *res;
    int error = getaddrinfo(nodename, servname, hints, &res);
    if (error != 0) {
        if (error == EAI_SYSTEM) {
            perror("getaddrinfo() failed");
        } else {
            fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(error));
        }
        return -1;
    }

    fprintf(stderr, "addrinfo:\n");
    print_addrinfo(res);

    freeaddrinfo(res);

    return 0;
}

int main(void) {
    struct addrinfo af_inet_hints = {.ai_family = AF_INET};

    if (test_addrinfo("dns.google", "https", &af_inet_hints) == -1) {
        return EXIT_FAILURE;
    }

    if (test_addrinfo("dns.google", "443", &af_inet_hints) == -1) {
        return EXIT_FAILURE;
    }

    if (test_addrinfo("8.8.8.8", "https", &af_inet_hints) == -1) {
        return EXIT_FAILURE;
    }

    if (test_addrinfo("8.8.8.8", "443", &af_inet_hints) == -1) {
        return EXIT_FAILURE;
    }

    if (test_addrinfo("8.8.8.8", NULL, &af_inet_hints) == -1) {
        return EXIT_FAILURE;
    }

    if (test_addrinfo(NULL, "443", &af_inet_hints) == -1) {
        return EXIT_FAILURE;
    }
}
