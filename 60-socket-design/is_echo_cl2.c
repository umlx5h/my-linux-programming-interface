#include "inet_sockets.h"
#include "tlpi_hdr.h"

#define BUF_SIZE 100

int
main(int argc, char *argv[])
{
    int sfd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s host\n", argv[0]);

    sfd = inetConnect(argv[1], "echo", SOCK_STREAM);
    if (sfd == -1)
        errExit("inetConnect");

    for (;;) {
        numRead = read(STDIN_FILENO, buf, BUF_SIZE);
        if (numRead <= 0)                   /* Exit loop on EOF or error */
            break;
        if (write(sfd, buf, numRead) != numRead)
            fatal("write() failed");

        numRead = read(sfd, buf, BUF_SIZE);
        if (numRead <= 0)                   /* Exit on EOF or error */
            break;
        printf("%.*s", (int) numRead, buf);
    }

    // if (close(sfd) == -1)
    //     errExit("close");

    if (shutdown(sfd, SHUT_WR) == -1)
        errExit("shutdown");

    exit(EXIT_SUCCESS);
}
