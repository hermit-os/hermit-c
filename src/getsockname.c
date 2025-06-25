#include "print_assert.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

enum { LISTEN_PORT = 9975 };

int test_getsockname(int type) {
    sa_family_t family = AF_INET;

    int fd = socket(family, type, 0);
    if (fd == -1) {
        perror("socket() failed");
        return -1;
    }

    struct sockaddr_in addr = {.sin_family = family,
                               .sin_port = htons(LISTEN_PORT),
                               .sin_addr = {htonl(INADDR_LOOPBACK)}};

    int bound = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (bound == -1) {
        perror("bind() failed");
        return -1;
    }

    struct sockaddr_in bound_addr;
    socklen_t bound_addrlen = sizeof(bound_addr);
    int get_bound =
        getsockname(fd, (struct sockaddr *)&bound_addr, &bound_addrlen);
    if (get_bound == -1) {
        perror("getsockname() failed");
        return -1;
    }

    print_assert(bound_addr.sin_family == addr.sin_family);
    print_assert(bound_addr.sin_port == addr.sin_port);
    print_assert(bound_addr.sin_addr.s_addr == addr.sin_addr.s_addr);

    return 0;
}

int main(void) {
    int tcp_status = test_getsockname(SOCK_STREAM);
    if (tcp_status == -1) {
        return EXIT_FAILURE;
    }

    int udp_status = test_getsockname(SOCK_DGRAM);
    if (udp_status == -1) {
        return EXIT_FAILURE;
    }
}
