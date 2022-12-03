/* make_orphan.c
    孤児プロセスになったときに親がinitになることを確認する
*/
#include <signal.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    pid_t childPid;

    setbuf(stdout, NULL);       /* Disable buffering of stdout */
    printf("Parent PID=%ld\n", (long) getpid());

    switch (childPid = fork()) {
    case -1:
        errExit("fork");

    case 0:     /* Child: immediately exits to become zombie */
        printf("Child (PID=%ld)\n", (long) getpid());
        pause();
        _exit(EXIT_SUCCESS);

    default:    /* Parent */
        exit(EXIT_SUCCESS);
    }
}
