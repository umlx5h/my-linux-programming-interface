#include <sys/wait.h>
#include <tlpi_hdr.h>

#define BUF_SIZE 10

int
main(int argc, char *argv[])
{
    int pfd[2]; /* Pipe file descriptors */
    char buf[BUF_SIZE];
    ssize_t numRead;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s string\n", argv[0]);
    
    if (pipe(pfd) == -1) /* Create the pipe*/ // [1]
        errExit("pipe");
    
    switch (fork()) { // [2]
    case -1:
        errExit("fork");
    case 0: /* Child - reads from pipe */
        if (close(pfd[1]) == -1) /* Write end is unused */ // [3]
            errExit("close - child");
        
        for (;;) { /* Read data from pipe, echo on stdout */
            numRead = read(pfd[0], buf, BUF_SIZE); // [4]
            if (numRead == -1)
                errExit("read");
            if (numRead == 0) // [5]
                break; /* End of file */
            if (write(STDOUT_FILENO, buf, numRead) != numRead) // [6]
                fatal("child - partial/failed write");
        }

        write(STDOUT_FILENO, "\n", 1); // [7]
        if (close(pfd[0]) == -1)
            errExit("close");
        _exit(EXIT_SUCCESS);


    default: /* Parent - writes to pipe */
        if (close(pfd[0]) == -1) /* Read end is unused */ // [8]
            errExit("close - parent");
        
        if (write(pfd[1], argv[1], strlen(argv[1])) != strlen(argv[1])) // [9]
            fatal("parent - partial/failed write");
        
        if (close(pfd[1]) == -1) /* Child will see EOF*/ // [10]
            errExit("close");
        
        wait(NULL); /* Wait for child to finish */ // [11]
        exit(EXIT_SUCCESS);
    }
}
