#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define LOG_NAME "dummy"

int
main(int argc, char *argv[])
{
    int logfd;
    logfd = open(LOG_NAME, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, S_IRUSR | S_IWUSR);
    FILE *log;
    log = fdopen(logfd, "a");
    // disable full buffering
    setbuf(log, NULL);
    unlink(LOG_NAME);

    printf("PID: %d\n", getpid());

    int i;
    for (i = 0; i < 50; i++) {
        fprintf(log, "dummy\n");
        sleep(1);
    }
    exit(EXIT_SUCCESS);
}
