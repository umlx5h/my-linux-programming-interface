// #include "tlpi_hdr.h"
#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int
main(int argc, char *argv[])
{
    char **ep;

    for (ep = environ; *ep; ep++) {
        puts(*ep);
        // printf("%s\n", *ep);
    }

    exit(EXIT_SUCCESS);
}
