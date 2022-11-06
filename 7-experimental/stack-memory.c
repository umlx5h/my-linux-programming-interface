#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/**
 * PID=XX
 * ps -o pid,rss,vsz,sz,command -p $PID
 * cat /proc/$PID/status |  grep -e VmHWM -e VmRSS -e VmSize -e VmStk -e VmData -e VmStk -e VmExe
 */


int
array_init(char *buf, int size)
{
    int i;
    for (i = 0; i < size; i++) {
        buf[i] = (char) 60;
    }
}

int
main(int argc, char *argv[])
{
    printf("pid: %ld\n", (long) getpid());

#define BUFSIZE 10

    char buf[BUFSIZE];
    // array_init(buf, BUFSIZE);

    getchar();
    exit(EXIT_SUCCESS);
}
