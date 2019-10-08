#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int filecopy(int fd)
{
    char buf[8192];
    ssize_t number_r;

    while ((number_r = read(fd, buf, sizeof buf)) > 0) {
        ssize_t total_w = 0;
        while (total_w < number_r) {
            ssize_t number_w = write(STDOUT_FILENO, buf + total_w, number_r - total_w);
            if (number_w < 1) {
                return -1;
            }
            total_w += number_w;
        }
    }

    return number_r == 0 ? 0 : -1;
}

/* cat:  concatenate files, version 2 */
int main(int argc, char *argv[])
{
    int fd, success;
    char *prog = argv[0];   /* program name for errors */

    if (argc == 1)  /* no args; copy standard input */
        fprintf(stderr, "%s: no args\n", prog);
    else
        while (--argc > 0)
            if ((fd = open(*++argv, O_RDONLY)) == -1) {
                fprintf(stderr, "%s: can′t open %s\n", prog, *argv);
                return 1;
            } else {
                filecopy(fd);
                if (close(fd) != 0) {
                    return -1;
                }
            }

    if (ferror(stdout)) {
        fprintf(stderr, "%s: error writing stdout\n", prog);
        return 2;
    }

    return 0;
}
