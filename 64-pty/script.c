#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <termios.h>
#include <sys/select.h>
#include "pty_fork.h"
#include "tty_functions.h"
#include "tlpi_hdr.h"

#define BUF_SIZE 256
#define MAX_SNAME 1000

struct termios ttyOrig;

static void /* Reset terminal mode on program exit */
ttyReset(void)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyOrig) == -1) 
        errExit("tcsetattr");
}

int
main(int argc, char *argv[])
{
    char slaveName[MAX_SNAME];
    char *shell;
    int masterFd, scriptFd;
    struct winsize ws;
    fd_set inFds;
    char buf[BUF_SIZE];
    ssize_t numRead;
    pid_t childPid;

    setbuf(stdout, NULL);

    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1) // @1
        errExit("tcgetattr");
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
        errExit("ioctl-TIOCGWINSZ");
    
    childPid = ptyFork(&masterFd, slaveName, MAX_SNAME, &ttyOrig, &ws); // @2
    if (childPid == -1)
        errExit("ptyFork");
    
    if (childPid == 0) { /* Child: execute a shell on pty slave */
        shell = getenv("SHELL"); // @3
        if (shell == NULL || *shell == '\0')
            shell = "/bin/sh";
        
        printf("[%ld] Child starting exec...\n", (long) getpid());

        execlp(shell, shell, (char *) NULL); // @4
        errExit("execlp"); /* If we get here, something went wrong */
    }

    /* Parent: relay data between terminal and pty master */
    scriptFd = open((argc > 1) ? argv[1] : "typescript", // @5
        O_WRONLY | O_CREAT | O_TRUNC,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    
    if (scriptFd == -1)
        errExit("open typescript");
    
    ttySetRaw(STDIN_FILENO, &ttyOrig); // @6
    if (atexit(ttyReset) != 0) // @7
        errExit("atexit");

    printf("[%ld] Parent starting loop...\n", (long) getpid());
    
    for (;;) { // @8
        FD_ZERO(&inFds);
        FD_SET(STDIN_FILENO, &inFds);
        FD_SET(masterFd, &inFds);

        if (select(masterFd + 1, &inFds, NULL, NULL, NULL) == -1) // @9
            errExit("select");
        
        if (FD_ISSET(STDIN_FILENO, &inFds)) { // @10 /* stdin --> pty */
            numRead = read(STDIN_FILENO, buf, BUF_SIZE);
            if (numRead <= 0)
                exit(EXIT_SUCCESS);
            
            if (write(masterFd, buf, numRead) != numRead)
                fatal("partial/failed write (masterFd)");
        } 

        if (FD_ISSET(masterFd, &inFds)) { // @11     /* pty --> stdout+file */
            numRead = read(masterFd, buf, BUF_SIZE);
            if (numRead <= 0)
                exit(EXIT_SUCCESS);
            
            if (write(STDOUT_FILENO, buf, numRead) != numRead)
                fatal("partial/failed write (STDOUT_FILENO)");
            if (write(scriptFd, buf, numRead) != numRead)
                fatal("partial/faield write (scriptFd)");
        }
    }
}
