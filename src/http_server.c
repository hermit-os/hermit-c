#include <netinet/in.h>
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

int main(void) {
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket() failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr = {.sin_family = AF_INET,
                               .sin_port = htons(LISTEN_PORT),
                               .sin_addr = INADDR_ANY};

    int bound = bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    if (bound < 0) {
        perror("bind() failed");
        return EXIT_FAILURE;
    }

    int listened = listen(listener, 1);
    if (listened < 0) {
        perror("listen() failed");
        return EXIT_FAILURE;
    }

    void *read_buf = malloc(BUFSIZ);

    fprintf(stderr, "Listening on port %d...\n", LISTEN_PORT);
    for (;;) {
        int connection = accept(listener, NULL, 0);
        if (connection < 0) {
            perror("accept() failed");
        }

        ssize_t readed = read(connection, read_buf, BUFSIZ);
        if (readed < 0) {
            perror("read() failed");
        }

        ssize_t written = write(connection, reply, sizeof(reply) - 1);
        if (written < 0) {
            perror("write() failed");
        } else if ((size_t)written != sizeof(reply) - 1) {
            fprintf(stderr, "partial write\n");
        }

        int closed = close(connection);
        if (closed < 0) {
            perror("close() failed");
        }
    }
}
