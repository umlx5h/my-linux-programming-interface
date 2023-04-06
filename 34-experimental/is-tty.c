#include <tlpi_hdr.h>

int
main(int argc, char *argv[])
{
    write(2, "start\n", 6);
    fprintf(stderr, "stdin is %s\n", isatty(0) ? "t" : "n");
    write(2, "end\n", 4);
    exit(EXIT_SUCCESS);
}
