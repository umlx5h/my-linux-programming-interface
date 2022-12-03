#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    // setbuf(stdout, NULL);
    printf("Hello world\n");
    write(STDOUT_FILENO, "Ciao\n", 5);

    // fflush(NULL);
    int childPid;
    if ((childPid = fork()) == -1) {
        errExit("fork");
    }
    // if (childPid == 0) {
    //     _exit(EXIT_SUCCESS);
    // }

    /* Both child and parent continue execution here */
    exit(EXIT_SUCCESS);
}
