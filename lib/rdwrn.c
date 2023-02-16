#include <unistd.h>
#include <errno.h>
#include "rdwrn.h" /* Declares readn() and writen() */

/* Read 'n' bytes from 'fd' into 'buf', restarting after partial
   reads or interruptions by a signal handlers */
ssize_t
readn(int fd, void *buffer, size_t n)
{
    ssize_t numRead; /* # of bytes fetched by last read() */
    ssize_t totRead; /* Total # of bytes read so far */
    char *buf;

    buf = buffer;    /* No pointer arithmetic on "void *" */
    for (totRead = 0; totRead < n; ) {
        numRead = read(fd, buf, n - totRead);

        if (numRead == 0)   /* EOF*/
            return totRead; /* May be 0 if this is first read() */
        if (numRead == -1) {
            if (errno == EINTR)
                continue;   /* Interrupted --> restart read() */
            else
                return -1;  /* Some other error */
        }
        totRead += numRead;
        buf += numRead;
    }

    return totRead;         /* Must be ’n’ bytes if we get here */
}

/* Write 'n' bytes to 'fd' from 'buf', restarting after partial
   write or interruptions by a signal handlers */
ssize_t
writen(int fd, const void *buffer, size_t n)
{
    ssize_t numWritten; /* # of bytes written by last write() */
    size_t  totWritten; /* Total # of bytes written so far */
    const char *buf;

    buf = buffer;          /* No pointer arithmetic on "void *" */
    for (totWritten = 0; totWritten < n; ) {
        numWritten = write(fd, buf, n - totWritten);

        if (numWritten <= 0) {
            if (numWritten == -1 && errno == EINTR)
                continue;  /* Interrupted --> restart write() */
            else
                return -1; /* Some other error */
        }
        totWritten += numWritten;
        buf += numWritten;
    }

    return totWritten;     /* Must be ’n’ bytes if we get here */
}
