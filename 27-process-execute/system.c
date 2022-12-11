#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

int
system(const char *command)
{
    sigset_t blockMask, origMask;
    struct sigaction saIgnore, saOrigQuit, saOrigInt, saDefault;

    pid_t childPid;
    int status, savedErrno;

    if (command == NULL) // [1]
        return system(":") == 0; /* Is a shell available? */

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD); /* Block SIGCHLD */
    sigprocmask(SIG_BLOCK, &blockMask, &origMask); // [2]

    saIgnore.sa_handler = SIG_IGN; /* Ignore SIGINT and SIGQUIT */
    saIgnore.sa_flags = 0;
    sigemptyset(&saIgnore.sa_mask);
    sigaction(SIGINT, &saIgnore, &saOrigInt); // [3]
    sigaction(SIGQUIT, &saIgnore, &saOrigQuit);

    switch (childPid = fork()) {
    case -1:
        status = -1;
        break; /* Carry on to reset signal attributes */
    
    case 0:
        saDefault.sa_handler = SIG_DFL;
        saDefault.sa_flags = 0;
        sigemptyset(&saDefault.sa_mask);

        if (saOrigInt.sa_handler != SIG_IGN) // [4]
            sigaction(SIGINT, &saDefault, NULL);
        if (saOrigQuit.sa_handler != SIG_IGN)
            sigaction(SIGQUIT, &saDefault, NULL);

        sigprocmask(SIG_SETMASK, &origMask, NULL); // [5]

        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        _exit(127); // [6]
    
    default:
        while (waitpid(childPid, &status, 0) == -1) { // [7]
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
        break;
    }

    /* Unblobk SIGCHLD, restore dispositions of SIGINT and SIGQUIT */
    savedErrno = errno; /* The following may change 'errno' */ // [8]

    sigprocmask(SIG_SETMASK, &origMask, NULL); // [9]
    sigaction(SIGINT, &saOrigInt, NULL);
    sigaction(SIGQUIT, &saOrigInt, NULL);

    errno = savedErrno; // [10]

    return status;
}
