#include <sys/time.h>
#include <sys/select.h>
#include "tlpi_hdr.h"

static void
usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s {timeout|-} fd-num[rw]...\n", progName);
    fprintf(stderr, " - means infinite timeout; \n");
    fprintf(stderr, " r = monitor for read\n");

    fprintf(stderr, " w = monitor for write\n\n");
    fprintf(stderr, " e.g.: %s - 0rw 1w\n", progName);
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    fd_set readfds, writefds;
    int ready, nfds, fd, numRead, j;
    struct timeval timeout;
    struct timeval *pto;
    char buf[10];            /* Large enough to hold "rw\0" */

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageError(argv[0]);
    
    /* Timeout for select() is specified in argv[1] */
    if (strcmp(argv[1], "-") == 0) {
        pto = NULL;          /* Infinite timeout */
    } else {
        pto = &timeout;
        timeout.tv_sec = getLong(argv[1], 0, "timeout");
        timeout.tv_usec = 0; /* No microseconds */
    }

    /* Process remaining arguments to build file descriptor sets */
    nfds = 0;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);

    for (j = 2; j < argc; j++) {
        numRead = sscanf(argv[j], "%d%2[rw]", &fd, buf);
        if (numRead != 2)
            usageErr(argv[0]);
        if (fd >= FD_SETSIZE)
            cmdLineErr("file descriptor exceeds limit (%d)\n", FD_SETSIZE);
        
        if (fd >= nfds)
            nfds = fd + 1; /* Record maximum fd + 1 */
        if (strchr(buf, 'r') != NULL)
            FD_SET(fd, &readfds);
        if (strchr(buf, 'w') != NULL)
            FD_SET(fd, &writefds);
    }

    // int fd3 = 2000;
    // if (dup2(0, fd3) == -1)
    //     errExit("dup2");
    // FD_SET(fd3, &readfds);
    // nfds = fd3 + 1;
    // stop("check");

    /* We’ve built all of the arguments; now call select() */
    ready = select(nfds, &readfds, &writefds, NULL, pto);

    if (ready == -1)
        errExit("exit");
    
    /* Display results of select() */
    printf("read = %d\n", ready);
    for (fd = 0; fd < nfds; fd++)
        // if (FD_ISSET(fd, &readfds) || FD_ISSET(fd, &writefds))
            printf("%d: %s%s\n", fd,
                FD_ISSET(fd, &readfds) ? "r" : "",
                FD_ISSET(fd, &writefds) ? "w" : "");

    if (pto != NULL)
        printf("timeout after select(): %ld.%03ld\n",
            (long) timeout.tv_sec, (long) timeout.tv_usec / 1000);
    
    exit(EXIT_SUCCESS);
}
