#include <sys/wait.h>
#include "print_wait_status.h"
#include "tlpi_hdr.h"

#define MAX_CMD_LEN 200

int
simple_system(char *command)
{
    int status;
    pid_t childPid;

    switch (childPid = fork()) {
    case -1:
        return -1;
    
    case 0:
        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        _exit(127);
    
    default:
        if (waitpid(childPid, &status, 0) == -1)
            return -1;
        else
            return status;
    }
}


int
my_system(const char *command)
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


int
main(int argc, char *argv)
{
    char str[MAX_CMD_LEN];
    int status;

    for (;;) {
        printf("Command: ");
        fflush(stdout);
        if (fgets(str, MAX_CMD_LEN, stdin) == NULL)
            break;
        
        // status = system(str);
        // status = simple_system(str);
        status = my_system(str);
        printf("system(3) returned: status=0x%04x (%d,%d)\n",
            (unsigned int) status, status >> 8, status & 0xff);
        
        if (status == -1) {
            errExit("system");
        } else {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 127)
                printf("(Probably) could not invoke shell\n");
            else
                printWaitStatus(NULL, status);
        }
    }

    exit(EXIT_SUCCESS);
}
