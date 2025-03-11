// NOLINTNEXTLINE(bugprone-reserved-identifier)
#define _GNU_SOURCE
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#define _LARGEFILE64_SOURCE
#include "print_assert.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int create_dir(const char *path) {
    fprintf(stderr, "create_dir(\"%s\")\n", path);

    mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

    fprintf(stderr, "mkdir(\"%s\")\n", path);
    int mkdir_status = mkdir(path, mode);
    if (mkdir_status == -1) {
        perror("mkdir() failed");
        return -1;
    }

    fprintf(stderr, "stat(\"%s\")\n", path);
    struct stat buffer;
    int stat_status = stat(path, &buffer);
    if (stat_status == -1) {
        perror("stat() failed");
        return -1;
    }

    print_assert((buffer.st_mode & mode) == mode);

    fputc('\n', stderr);
    return 0;
}

static int remove_dir(const char *path) {
    fprintf(stderr, "remove_dir(\"%s\")\n", path);

    fprintf(stderr, "rmdir(\"%s\")\n", path);
    int rmdir_status = rmdir(path);
    if (rmdir_status == -1) {
        perror("rmdir() failed");
        return -1;
    }

    fputc('\n', stderr);
    return 0;
}

static int create_files(const char *dir_path, size_t file_count) {
    fprintf(stderr, "create_files(\"%s\", %zu)\n", dir_path, file_count);

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int oflag = O_RDWR | O_CREAT | O_TRUNC;

    for (size_t i = 0; i < file_count; ++i) {
        char file_path[256];
        int printed =
            snprintf(file_path, sizeof(file_path), "%s/file_%zu", dir_path, i);
        if (printed < 0) {
            perror("snprintf() failed");
            return -1;
        }
        if ((size_t)printed + 1 > sizeof(file_path)) {
            fprintf(stderr, "snprintf() failed: buffer too small");
            return -1;
        }

        fprintf(stderr, "open(\"%s\")\n", file_path);
        int fd = open(file_path, oflag, mode);
        if (fd == -1) {
            perror("open() failed");
            return -1;
        }

        fprintf(stderr, "fstat(%i)\n", fd);
        struct stat buffer;
        int fstat_status = fstat(fd, &buffer);
        if (fstat_status == -1) {
            perror("fstat() failed");
            return -1;
        }

        print_assert((buffer.st_mode & mode) == mode);

        char buf[512];
        memset(buf, 0x7f, sizeof(buf));

        ssize_t written = write(fd, buf, sizeof(buf));
        if (written == -1) {
            perror("write() failed");
            return -1;
        }
        if ((size_t)written != sizeof(buf)) {
            fprintf(stderr, "partial write\n");
            return -1;
        }

        off_t seeked = lseek(fd, 0, SEEK_SET);
        if (seeked == -1) {
            perror("lseek() failed");
            return -1;
        }

        memset(buf, 0, sizeof(buf));
        ssize_t readed = read(fd, buf, sizeof(buf));
        if (readed == -1) {
            perror("read() failed");
            return -1;
        }
        if ((size_t)readed != sizeof(buf)) {
            fprintf(stderr, "readed = %zu\n", readed);
            fprintf(stderr, "partial read\n");
            return -1;
        }
        fprintf(stderr, "readed = %zu\n", readed);

        for (size_t i = 0; i < sizeof(buf); ++i) {
            assert(buf[i] == 0x7f);
        }

        fprintf(stderr, "close(%i)\n", fd);
        int closed = close(fd);
        if (closed == -1) {
            perror("close() failed");
            return -1;
        }
    }

    fputc('\n', stderr);
    return 0;
}

static int unlink_files(const char *dir_path, size_t file_count) {
    fprintf(stderr, "unlink_files(\"%s\", %zu)\n", dir_path, file_count);

    for (size_t i = 0; i < file_count; ++i) {
        char file_path[256];
        int written =
            snprintf(file_path, sizeof(file_path), "%s/file_%zu", dir_path, i);
        if (written < 0) {
            perror("snprintf() failed");
            return -1;
        }
        if ((size_t)written + 1 > sizeof(file_path)) {
            fprintf(stderr, "snprintf() failed: buffer too small");
            return -1;
        }

        fprintf(stderr, "unlink(\"%s\")\n", file_path);
        int fd = unlink(file_path);
        if (fd == -1) {
            perror("unlink() failed");
            return -1;
        }
    }

    fputc('\n', stderr);
    return 0;
}

struct linux_dirent64 {
    ino64_t d_ino;
    off64_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};

static int print_getdents64(const char *path) {
    fprintf(stderr, "print_getdents64(\"%s\")\n", path);

    fprintf(stderr, "open(\"%s\")\n", path);
    int fd = open(path, 0);
    if (fd == -1) {
        perror("open() failed");
        return -1;
    }

    fprintf(stderr, "getdents64(%i)\n", fd);
    char buf[512];
    ssize_t read = getdents64(fd, (void *)buf, sizeof(buf));
    if (read == -1) {
        perror("getdents64() failed");
        return -1;
    }

    fputc('\n', stderr);
    for (size_t pos = 0; pos < (size_t)read;) {
        struct linux_dirent64 *dirent = (struct linux_dirent64 *)(buf + pos);

        fprintf(stderr, "d_ino = %" PRIu64 "\n", dirent->d_ino);
        fprintf(stderr, "d_off = %" PRId64 "\n", dirent->d_off);
        fprintf(stderr, "d_reclen = %hu\n", dirent->d_reclen);
        fprintf(stderr, "d_type = %hhu\n", dirent->d_type);
        fprintf(stderr, "d_name = %s\n\n", dirent->d_name);

        pos += dirent->d_reclen;
    }

    fprintf(stderr, "close(%i)\n", fd);
    int closed = close(fd);
    if (closed == -1) {
        perror("close() failed");
        return -1;
    }

    fputc('\n', stderr);
    return 0;
}

int main(void) {
#ifdef __hermit__
    const char *dir_path = "/my-dir";
#else
    const char *dir_path = "my-dir";
#endif

    int file_count = 3;

    int create_dir_status = create_dir(dir_path);
    if (create_dir_status == -1) {
        return EXIT_FAILURE;
    }

    int create_files_status = create_files(dir_path, file_count);
    if (create_files_status == -1) {
        return EXIT_FAILURE;
    }

    int getdents64_status = print_getdents64(dir_path);
    if (getdents64_status == -1) {
        return EXIT_FAILURE;
    }

    int unlink_status = unlink_files(dir_path, file_count);
    if (unlink_status == -1) {
        return EXIT_FAILURE;
    }

    int remove_dir_status = remove_dir(dir_path);
    if (remove_dir_status == -1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
