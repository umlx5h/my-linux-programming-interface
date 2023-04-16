/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 62-4 */

/* test_tty_functions.c

   Put the terminal in raw mode (or cbreak if a command-line argument is
   supplied), allowing the program to read input a character at a time, and
   read and echo characters.

   Usage: test_tty_functions [x]

   See also tty_functions.c.
*/

#include <termios.h>
#include <signal.h>
#include <ctype.h>
#include "tty_functions.h"              /* Declarations of ttySetCbreak()
                                           and ttySetRaw() */
#include "tlpi_hdr.h"

static struct termios userTermios; // @1
                        /* Terminal settings as defined by user */

static void             /* General handler: restore tty settings and exit */
handler(int sig)
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &userTermios) == -1) // @2
        errExit("tcsetattr");
    _exit(EXIT_SUCCESS);
}

static void             /* Handler for SIGTSTP */
tstpHandler(int sig) // @3
{
    struct termios ourTermios;          /* To save our tty settings */
    sigset_t tstpMask, prevMask;
    struct sigaction sa;
    int savedErrno;

    savedErrno = errno;                 /* We might change 'errno' here */

    /* Save current terminal settings, restore terminal to
       state at time of program startup */

    if (tcgetattr(STDIN_FILENO, &ourTermios) == -1) // @4
        errExit("tcgetattr");
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &userTermios) == -1) // @5
        errExit("tcsetattr");

    /* Set the disposition of SIGTSTP to the default, raise the signal
       once more, and then unblock it so that we actually stop */

    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR)
        errExit("signal");
    raise(SIGTSTP);

    sigemptyset(&tstpMask);
    sigaddset(&tstpMask, SIGTSTP);
    if (sigprocmask(SIG_UNBLOCK, &tstpMask, &prevMask) == -1)
        errExit("sigprocmask");

    /* Execution resumes here after SIGCONT */

    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
        errExit("sigprocmask");         /* Reblock SIGTSTP */

    sigemptyset(&sa.sa_mask);           /* Reestablish handler */
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = tstpHandler;
    if (sigaction(SIGTSTP, &sa, NULL) == -1)
        errExit("sigaction");

    /* The user may have changed the terminal settings while we were
       stopped; save the settings so we can restore them later */

    if (tcgetattr(STDIN_FILENO, &userTermios) == -1) // @6
        errExit("tcgetattr");

    /* Restore our terminal settings */

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ourTermios) == -1) // @7
        errExit("tcsetattr");

    errno = savedErrno;
}

int
main(int argc, char *argv[])
{
    char ch;
    struct sigaction sa, prev;
    ssize_t n;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (argc > 1) { // @8          /* Use cbreak mode */
        if (ttySetCbreak(STDIN_FILENO, &userTermios) == -1) // @9
            errExit("ttySetCbreak");

        /* Terminal special characters can generate signals in cbreak
           mode. Catch them so that we can adjust the terminal mode.
           We establish handlers only if the signals are not being ignored. */

        sa.sa_handler = handler; // @10

        // 第二引数のNULL -> 何も設定を変えないという意味
        if (sigaction(SIGQUIT, NULL, &prev) == -1)
            errExit("sigaction");
        if (prev.sa_handler != SIG_IGN)
            if (sigaction(SIGQUIT, &sa, NULL) == -1)
                errExit("sigaction");

        if (sigaction(SIGINT, NULL, &prev) == -1)
            errExit("sigaction");
        if (prev.sa_handler != SIG_IGN)
            if (sigaction(SIGINT, &sa, NULL) == -1)
                errExit("sigaction");

        sa.sa_handler = tstpHandler; // @11

        if (sigaction(SIGTSTP, NULL, &prev) == -1)
            errExit("sigaction");
        if (prev.sa_handler != SIG_IGN)
            if (sigaction(SIGTSTP, &sa, NULL) == -1)
                errExit("sigaction");
    } else {                            /* Use raw mode */
        if (ttySetRaw(STDIN_FILENO, &userTermios) == -1) // @12
            errExit("ttySetRaw");
    }

    sa.sa_handler = handler; // @13
    if (sigaction(SIGTERM, &sa, NULL) == -1)
        errExit("sigaction");

    setbuf(stdout, NULL);               /* Disable stdout buffering */

    for (;;) { // @14                   /* Read and echo stdin */
        n = read(STDIN_FILENO, &ch, 1);
        if (n == -1) {
            errMsg("read");
            break;
        }

        if (n == 0)                     /* Can occur after terminal disconnect */
            break;

        if (isalpha((unsigned char) ch)) // @15 /* Letters --> lowercase */
            putchar(tolower((unsigned char) ch));
        else if (ch == '\n' || ch == '\r')
            putchar(ch);
        else if (iscntrl((unsigned char) ch))
            printf("^%c", ch ^ 64);     /* Echo Control-A as ^A, etc. */
        else
            putchar('*');               /* All other chars as '*' */

        if (ch == 'q') // @16           /* Quit loop */
            break;
    }

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &userTermios) == -1) // @17
        errExit("tcsetattr");
    exit(EXIT_SUCCESS);
}
