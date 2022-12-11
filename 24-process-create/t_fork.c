#include "tlpi_hdr.h"

static int idata = 111; /* Allocated in data segment */

int
main(int argc, char *argv[])
{
    int istack = 222; /* Allocated in stack segment */
    pid_t childPid;

    write(2, "before fork:\n", 13);
    switch (childPid = fork()) {
    case -1:
        errExit("fork");
    case 0:
        idata *= 3;
        istack *= 3;
        break;

    default:
        sleep(3); /* Give child a chance to execute */
        break;
    }

    printf("PID=%ld %s idata=%d istack=%d\n",
        (long) getpid(),
        (childPid == 0) ? "(child)" : "(parent)",
        idata, istack
    );
    exit(EXIT_SUCCESS);
}
