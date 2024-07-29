#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include <stddef.h>
#include "kernel/fcntl.h"

#define BUF_SIZE 512

char *xv6_strcat(char *dest, const char *src) {
    char *ret = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++))
        ;
    return ret;
}

int readlink(const char *path, char *buf, int bufsiz) {
    int fd, n;
    if ((fd = open(path, O_RDONLY | O_NOFOLLOW)) < 0) {
        return -1;
    }
    if ((n = read(fd, buf, bufsiz - 1)) < 0) {
        close(fd);
        return -1;
    }
    close(fd);
    buf[n] = '\0';
    return n;
}

void simlinkinfo(const char *path) {
    int fd;
    struct stat st;
    struct dirent de;
    char buf[BUF_SIZE];
    char target[BUF_SIZE];

    if ((fd = open(path, O_RDONLY)) < 0) {
        printf("Cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        printf("Cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        printf("%s is not a directory\n", path);
        close(fd);
        return;
    }

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        strcpy(buf, path);
        xv6_strcat(buf, "/");
        xv6_strcat(buf, de.name);

        int file_fd = open(buf, O_RDONLY | O_NOFOLLOW);
        if (file_fd < 0) {
            continue;
        }

        if (fstat(file_fd, &st) < 0) {
            printf("Cannot stat %s\n", buf);
            close(file_fd);
            continue;
        }

        close(file_fd);

        if (st.type == T_SYMLINK) {

            int target_len = readlink(buf, target, sizeof(target) - 1);
            if (target_len < 0) {
                printf("Cannot read symbolic link %s\n", buf);
                continue;
            }
            target[target_len] = '\0';

            printf("%s ->     ./%s\n", de.name, target);
        } else {
            // Regular file or directory, print its name
            //printf("%s\n", de.name);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        simlinkinfo(".");
    } else {
        simlinkinfo(argv[1]);
    }
    exit();
}
