#include "tlpi_hdr.h"
#define MAX_READ 20

int main(int argc, char *argv[])
{
    char buffer[MAX_READ];

    if (read(STDIN_FILENO, buffer, MAX_READ) == -1)
        errExit("read");
    
    printf("The input data was: %s\n", buffer);

    exit(EXIT_SUCCESS);
}
