#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void
child_loop()
{
    // child loop
    for (;;) {
        write(1, "child\n", 6);
        sleep(1);
    }
}

int
main(int argc, char *argv[])
{
    pid_t cld_pid;
    cld_pid = fork();

    if (cld_pid == 0) {
        child_loop();
    } else {
        // parent loop
        for (;;) {
            write(1, "parent\n", 7);
            sleep(1);
        }
    }

    exit(EXIT_SUCCESS);
}
