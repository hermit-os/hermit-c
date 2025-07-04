#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

enum { LISTEN_PORT = 9975 };

static const char reply[] = "HTTP/1.1 200 OK\n"
                            "Content-Type: text/plain\n"
                            "Content-Length: 14\n"
                            "\n"
                            "Hello, world!\n";

void accept_all(int *restrict nfds, fd_set *restrict readfds, int listener) {
    for (;;) {
        int connection = accept(listener, NULL, 0);
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

        FD_SET(connection, readfds);
        if (connection >= *nfds) {
            *nfds = connection + 1;
        }
    }
}

void run(int *restrict nfds, fd_set *restrict readfds,
         fd_set *restrict writefds, int listener) {
    fd_set readfds_out = *readfds;
    fd_set writefds_out = *writefds;
    int event_count = select(*nfds, &readfds_out, &writefds_out, NULL, NULL);
    if (event_count == -1) {
        perror("select() failed");
    }

    int handled_events = 0;

    if (FD_ISSET(listener, &readfds_out)) {
        accept_all(nfds, readfds, listener);
        handled_events += 1;
        FD_CLR(listener, &readfds_out);
    }

    for (int fd = 0; fd < *nfds; ++fd) {
        if (handled_events == event_count) {
            break;
        }

        if (FD_ISSET(fd, &readfds_out)) {
            void *read_buf = malloc(BUFSIZ);

            ssize_t readed = read(fd, read_buf, BUFSIZ);
            if (readed == -1) {
                perror("read() failed");
            }

            FD_CLR(fd, readfds);
            FD_SET(fd, writefds);
            free(read_buf);

            handled_events += 1;
        }

        if (FD_ISSET(fd, &writefds_out)) {
            ssize_t written = write(fd, reply, sizeof(reply) - 1);
            if (written == -1) {
                perror("write() failed");
            } else if ((size_t)written != sizeof(reply) - 1) {
                fprintf(stderr, "partial write\n");
            }

            int closed = close(fd);
            if (closed == -1) {
                perror("close() failed");
            }

            FD_CLR(fd, writefds);

            handled_events += 1;
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
                               .sin_addr = {htonl(INADDR_ANY)}};

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

    int nfds = 0;
    fd_set readfds;
    fd_set writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    FD_SET(listener, &readfds);
    nfds = listener + 1;

    fprintf(stderr, "Listening on port %d...\n", LISTEN_PORT);
    for (;;) {
        run(&nfds, &readfds, &writefds, listener);
    }
}
