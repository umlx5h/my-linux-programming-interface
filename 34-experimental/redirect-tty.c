#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <my.h>


/**
 * fd1, 2が端末ではなくても、スペシャルファイル/dev/ttyを開くことで、端末に読み書きできる
 * $ gcc -g -I../lib redirect-tty.c ../libtlpi.a && ./a.out 1> /dev/null 2>/dev/null
 */

int
main(int argc, char *argv[])
{
    int ttyFd;
    FILE *ttyOut, *ttyIn;
    char strBuf[256];

    setbuf(stdout, NULL);

    fprintf(stdout, "controlling terminal path name: %s\n", ctermid(NULL));

    fprintf(stdout, "write to stdout!\n");
    fprintf(stdout, "write to stdout2!\n");
    fprintf(stderr, "write to stderr!\n");
    fprintf(stderr, "write to stderr2!\n");

    ttyFd = open("/dev/tty", O_RDWR);
    if (ttyFd == -1) {
        perror("failed to open tty");
        exit(EXIT_FAILURE);
    }
    ttyOut = fdopen(ttyFd, "w");
    ttyIn = fdopen(ttyFd, "r");
    setbuf(ttyOut, NULL);

    fprintf(ttyOut, "write to /dev/tty\n");
    fprintf(ttyOut, "reading from /dev/tty: ");
    fgets(strBuf, sizeof strBuf, ttyIn);
    fprintf(ttyOut, "\nread: %s", strBuf);

    stop("exit");
    exit(EXIT_SUCCESS);
}
