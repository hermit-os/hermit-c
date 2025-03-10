
#include "vec.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

enum { LISTEN_PORT = 9975 };

static const char reply[] = "HTTP/1.1 200 OK\n"
                            "Content-Type: text/plain\n"
                            "Content-Length: 14\n"
                            "\n"
                            "Hello, world!\n";

vec_impl(vec_pollfd, struct pollfd); // NOLINT(clang-diagnostic-extra-semi)

void accept_all(struct vec_pollfd *pollfds) {
    for (;;) {
        int connection = accept(pollfds->ptr[0].fd, NULL, 0);
        if (connection == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }

            perror("accept() failed");
            return;
        }

        // Set nonblocking
        int previous = fcntl(connection, F_GETFL);
        if (previous == -1) {
            perror("fcntl() failed");
            return;
        }

        int fcntl_status = fcntl(connection, F_SETFL, previous | O_NONBLOCK);
        if (fcntl_status == -1) {
            perror("fcntl() failed");
            return;
        }

        struct pollfd pollfd_listener = {
            .fd = connection,
            .events = POLLIN,
        };
        vec_pollfd_push(pollfds, pollfd_listener);
    }
}

void run(struct vec_pollfd *pollfds) {
    int event_count = poll(pollfds->ptr, pollfds->len, -1);
    if (event_count == -1) {
        perror("poll() failed");
    }

    int handled_events = 0;

    if (pollfds->ptr[0].revents == POLLIN) {
        accept_all(pollfds);
        handled_events += 1;
    }

    for (size_t i = 1; i < pollfds->len; ++i) {
        if (handled_events == event_count) {
            break;
        }

        struct pollfd *connection = &pollfds->ptr[i];

        if (connection->revents != 0) {
            handled_events += 1;
        }

        if (connection->revents == POLLIN) {
            void *read_buf = malloc(BUFSIZ);

            ssize_t readed = read(connection->fd, read_buf, BUFSIZ);
            if (readed == -1) {
                perror("read() failed");
            }

            connection->events = POLLOUT;
            free(read_buf);
        }

        if (connection->revents == POLLOUT) {
            ssize_t written = write(connection->fd, reply, sizeof(reply) - 1);
            if (written == -1) {
                perror("write() failed");
            } else if ((size_t)written != sizeof(reply) - 1) {
                fprintf(stderr, "partial write\n");
            }

            int closed = close(connection->fd);
            if (closed == -1) {
                perror("close() failed");
            }

            vec_pollfd_swap_remove(pollfds, i);
        }
    }
}

int main(void) {
    int listener = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listener == -1) {
        perror("socket() failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr = {.sin_family = AF_INET,
                               .sin_port = htons(LISTEN_PORT),
                               .sin_addr = INADDR_ANY};

    int bound = bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    if (bound == -1) {
        perror("bind() failed");
        return EXIT_FAILURE;
    }

    int listened = listen(listener, 128);
    if (listened == -1) {
        perror("listen() failed");
        return EXIT_FAILURE;
    }

    struct vec_pollfd pollfds = vec_pollfd_new();

    struct pollfd pollfd_listener = {
        .fd = listener,
        .events = POLLIN,
    };
    vec_pollfd_push(&pollfds, pollfd_listener);

    fprintf(stderr, "Listening on port %d...\n", LISTEN_PORT);
    for (;;) {
        run(&pollfds);
    }
    vec_pollfd_drop(&pollfds);
}
