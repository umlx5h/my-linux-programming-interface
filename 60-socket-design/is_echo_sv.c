#include <signal.h>
#include <syslog.h>
#include <sys/wait.h>
#include "become_daemon.h"
#include "inet_sockets.h" /* Declarations of inet*() socket functions */
#include "tlpi_hdr.h"

#include "syslog_printf.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

// #define SERVICE "echo"
#define SERVICE "12345"
#define BUF_SIZE 4096

static void
grimReaper(int sig) /* SIGCHLD handler to reap dead child processes */
{
    int savedErrno; /* Save ’errno’ in case changed here */

    savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

/* Handle a client request: copy socket input back to socket */
static void
handleRequest(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t numRead;

    while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
        if (write(cfd, buf, numRead) != numRead) {
            syslog(LOG_ERR, "write() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (numRead == -1) {
        syslog(LOG_ERR, "Error from read(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // success
}


int
main(int argc, char *argv[])
{
    int lfd, cfd; /* Listening and connected sockets */
    struct sigaction sa;

    // if (becomeDaemon(0) == -1)
    //     errExit("becomeDaemon");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    lfd = inetListen(SERVICE, 10, NULL);
    if (lfd == -1) {
        syslog(LOG_ERR, "Could not create server socket (%s)",
            strerror(errno));
        exit(EXIT_FAILURE);
    }

    // MEMO: TCP_DEFER_ACCEPTを設定すると、1st byteが来るまでaccept(2)がリターンせずコネクションがSYN_RECVの状態で止まる
    // クライアント側でconnect(2)を実行しても、write(2)でソケットにデータを1バイトでも書き込まない限りaccept(2)がリターンしない
    // apacheで使われていた！！！
    int timeout = 20;
    if (setsockopt(lfd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &timeout, sizeof(int)) == -1) {
        errExit("setsockopt");
    }

    for (;;) {
        cfd = accept(lfd, NULL, NULL); /* Wait for connection */
        if (cfd == -1) {
            syslog(LOG_ERR, "Failure in accept(): %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        stop("accept success");

        /* Handle each client request in a new child process */
        switch (fork()) {
        case -1:
            syslog(LOG_ERR, "Can’t create child (%s)", strerror(errno));
            close(cfd);   /* Give up on this client */

            break;        /* May be temporary; try next client */
        
        case 0: /* Child */
            close(lfd); /* Unneeded copy of listening socket */
            handleRequest(cfd);
            // exit()を実行すると、暗黙的にファイルディスクリプタがクローズされるので
            // TCP FINがクライアントに送信されコネクションが自動的に切断される
            _exit(EXIT_SUCCESS);

        default: /* Parent */
            close(cfd); /* Unneeded copy of connected socket */
            break;
        }
    }
}
