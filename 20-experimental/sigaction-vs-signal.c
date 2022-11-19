#define _GNU_SOURCE

#include <signal.h>
#include <tlpi_hdr.h>
#include <signal_functions.h>

static void
sigHandler(int sig)
{
    // printf("Ouch");
    write(1, "Ouch", 4);
    sleep(1);
    write(1, "!\n", 2);
    // printf("!\n");
}

int
main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = sigHandler;
    sigemptyset(&act.sa_mask);
    // sigaddset(&act.sa_mask, SIGINT);
    sigaction(SIGINT, &act, NULL);
    for (;;) {
        ;
    }
}

