#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    printf("Parent: PID=%ld, PGID=%ld, SID=%ld\n",
        (long) getpid(), (long) getpgrp(), (long) getsid(0));
    if (fork() != 0) { /* Exit if parent, or on error */
        // stop("parent");
        _exit(EXIT_SUCCESS);
    }
    
    // if (setsid() == -1)
    //     errExit("setsid");

    printf("Child:  PID=%ld, PGID=%ld, SID=%ld\n",
        (long) getpid(), (long) getpgrp(), (long) getsid(0));
    
    int ttyFD;
    ttyFD = open("/dev/tty", O_RDWR);
    if (ttyFD == -1) {
        // stop("tty open failed");
        // sleep(1000);
        errExit("open /dev/tty");
    }

    write(ttyFD, "dummy write to tty\n", 19);
    // stop("tty open success");
    // sleep(1000);
    
    exit(EXIT_SUCCESS);
}
