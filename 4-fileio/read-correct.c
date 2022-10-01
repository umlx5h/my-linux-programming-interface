#include "tlpi_hdr.h"
#define MAX_READ 20

int main(int argc, char *argv[])
{
    char buffer[MAX_READ + 1];
    ssize_t numRead;

    numRead = read(STDIN_FILENO, buffer, MAX_READ);
    if (numRead == -1)
        errExit("read");
    
    // 最後にNULL文字を格納
    buffer[numRead] = '\0';
    printf("The input data was: %s\n", buffer);

    exit(EXIT_SUCCESS);
}
