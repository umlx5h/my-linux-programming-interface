#include <tlpi_hdr.h>
#include <signal.h>

static void
sigHupHandler(int signum)
{
    write(1, "call sighup!\n", 13);
}


int
main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    signal(SIGHUP, sigHupHandler);

    printf("install signal handler\n");
    pause();

    printf("exit\n");
    exit(EXIT_SUCCESS);
}
