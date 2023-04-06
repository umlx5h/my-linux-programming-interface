#define _GNU_SOURCE /* Get declaration of strsignal() from <string.h> */
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

static int cmdNum; /* Out position in pipeline */

static void        /* Handler for various signals */
handler(int sig)
{
    /* UNSAFE: This handler uses non-async-signal-safe functions
        (fprintf(), strsignal(); see Section 21.1.2) */    
    
    if (getpid() == getpgrp()) // @1 /* If process group leader */
        fprintf(stderr, "Terminal FG process group: %ld\n",
            (long) tcgetpgrp(STDERR_FILENO));
    
    fprintf(stderr, "Process %ld (%d) received signal %d (%s)\n", // @2
        (long) getpid(), cmdNum, sig, strsignal(sig));
    
    /* If we catch SIGTSTP, it won’t actually stop us. Therefore we
        raise SIGSTOP so we actually get stopped. */
    if (sig == SIGTSTP) // @3
        raise(SIGSTOP);
}

int
main(int argc, char *argv[])
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1) // @4
        errExit("sigaction");
    if (sigaction(SIGTSTP, &sa, NULL) == -1)
        errExit("sigaction");
    if (sigaction(SIGCONT, &sa, NULL) == -1)
        errExit("sigaction");

    /* If stdin is a terminal, this is the first process in pipeline:
        print a heading and initialize message to be sent down pipe */    
    if (isatty(STDIN_FILENO)) { // @5
        fprintf(stderr, "Terminal FG process group: %ld\n",
            (long) tcgetpgrp(STDIN_FILENO));
        fprintf(stderr, "Command     PID  PPID  PGRP   SIG\n"); // @6
        cmdNum = 0;
    } else { /* Not first in pipeline, so read message from pipe */
        if (read(STDIN_FILENO, &cmdNum, sizeof(cmdNum)) <= 0) // @7
            fatal("read got EOF or error");
    }

    cmdNum++; // @8
    fprintf(stderr, "   %4d   %5ld %5ld %5ld %5ld\n", cmdNum, // @9
        (long) getpid(), (long) getppid(), (long) getpgrp(), (long) getsid(0));
    
    /* If not the last process, pass a message to the next process */
    if (!isatty(STDOUT_FILENO)) /* If not tty, then should be pipe */
        if (write(STDOUT_FILENO, &cmdNum, sizeof(cmdNum)) == -1) // @10
            errMsg("write");
    
    for (;;)    /* Wait for signals */
        pause();
}
