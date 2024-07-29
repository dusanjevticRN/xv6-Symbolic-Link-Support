#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BLOCK_SIZE 512 // Assuming a block size of 512 bytes for calculations

void print_usage(const char *path, uint size) {
    uint blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Manually adjust spacing - not dynamically perfect but simpler
    int pathLength = strlen(path);
    int spaceNeeded = 50 - pathLength; // Assuming 50 is more than any path length you have
    spaceNeeded = spaceNeeded > 0 ? spaceNeeded : 0; // Ensure no negative spaces

    printf("%s", path);
    for (int i = 0; i < spaceNeeded; i++) {
        printf(" "); // Print spaces manually
    }
    printf("%d\n", blocks);
}

int du(const char *path) {
    int fd;
    struct stat st;
    uint total_blocks = 0;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "du: cannot open %s\n", path);
        return 0;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "du: cannot stat %s\n", path);
        close(fd);
        return 0;
    }

    uint blocks = (st.size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Convert size to blocks

    if (st.type == T_FILE) {
        print_usage(path, st.size); // Use size in bytes
        total_blocks += blocks;
    } else if (st.type == T_DIR) {
        struct dirent de;
        char buf[512], *p;
        int len = strlen(path);
        strcpy(buf, path);
        p = buf + len;
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            total_blocks += du(buf);
        }
    }
    close(fd);
    return total_blocks;
}

int main(int argc, char *argv[]) {
    uint total = 0;
    if (argc < 2) {
        total = du(".");
    } else {
        for (int i = 1; i < argc; i++) {
            total += du(argv[i]);
        }
    }

    // Assuming "50" as the fixed width for alignment based on previous discussion
    const char* totalLabel = "Total";
    int totalLabelLength = strlen(totalLabel);
    int spaceNeededForTotal = 50 - totalLabelLength; // Adjust this value as needed
    spaceNeededForTotal = spaceNeededForTotal > 0 ? spaceNeededForTotal : 0;

    printf("%s", totalLabel);
    for (int i = 0; i < spaceNeededForTotal; i++) {
        printf(" "); // Print spaces to align with file paths and block counts
    }
    printf("%d\n", total); // Print the total block count

    exit(); // Assuming your environment supports a no-argument exit call
}
