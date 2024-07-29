#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(2, "Usage: sln target linkname\n");
        exit();
    }
    //printf(argv[1]);
    //printf(argv[2]);

    char *target = argv[1];
    char *link = argv[2];

    //printf(target);
    //printf(link);


    if (symlink(target, link) < 0)
    {
        fprintf(2, "sln: cannot create symbolic link\n");
        exit();
    }

    exit();
}
