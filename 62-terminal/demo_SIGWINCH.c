/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 62-5 */

/* demo_SIGWINCH.c

   Demonstrate the generation of the SIGWINCH signal. A handler for SIGWINCH
   allows the program to discover terminal window size changes; when that
   signal is generated, the program displays the new terminal window size.
*/
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "tlpi_hdr.h"

static void
sigwinchHandler(int sig)
{
}

static void
sigintHandler(int sig)
{
    struct winsize ws;
    ws.ws_row = 40;
    ws.ws_col = 100;

    // ウィンドウサイズを変更する場合
    if (ioctl(STDIN_FILENO, TIOCSWINSZ, &ws) == -1)
        errExit("ioctl");
    
    sleep(3);

    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
    sigset_t sigmask;
    sigaddset(&sigmask, SIGINT);
    sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
}

int
main(int argc, char *argv[])
{
    struct winsize ws;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigwinchHandler;
    if (sigaction(SIGWINCH, &sa, NULL) == -1)
        errExit("sigaction");
    
    sa.sa_handler = sigintHandler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        errExit("sigaction2");

    for (;;) {
        pause();                        /* Wait for SIGWINCH signal */

        // ウィンドウサイズを取得する
        if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1)
            errExit("ioctl");
        printf("Caught SIGWINCH, new window size: "
                "%d rows * %d columns\n", ws.ws_row, ws.ws_col);
    }
}
