#include <signal.h>
#include <tlpi_hdr.h>
#include <signal_functions.h>

static void
sigHandler(int sig)
{
    printf("Ouch!\n");
}

int
main(int argc, char *argv[])
{
    signal(SIGINT, sigHandler);
    pause();

    printf("Hello world after pause()!");
}
