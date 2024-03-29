// socketpair() can be used to create a unix socket for two-way (i.e.,
// full-duplex) communication. You could use two pipe()'s for two way
// communication, or you could use a socket (but then you have to deal
// with listen(), connect(), etc on both ends). socketpair() allows
// you to use sockets, but simplifies things to make them easier for
// you to use!

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "my.h"

int
main(void)
{
	int sv[2]; /* the pair of socket descriptors */
	char buf; /* for data exchange between processes */

    setbuf(stdout, NULL);

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
		perror("socketpair");
		exit(EXIT_FAILURE);
	}

	if (fork() == 0) {  /* child */
        close(sv[0]);
		sleep(3);
		read(sv[1], &buf, 1);
		printf("child: read '%c'\n", buf);
		buf = toupper(buf);  /* make it uppercase */
		write(sv[1], &buf, 1);
		printf("child: sent '%c'\n", buf);

        stop("child");

        _exit(0);
	}
    close(sv[1]);
    write(sv[0], "b", 1);
    printf("parent: sent 'b'\n");
    read(sv[0], &buf, 1);
    printf("parent: read '%c'\n", buf);
    wait(NULL); /* wait for child to die */

	return 0;
}
