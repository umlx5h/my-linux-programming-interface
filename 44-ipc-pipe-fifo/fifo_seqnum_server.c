#include <signal.h>
#include "fifo_seqnum.h"

int
main(int argc, char *argv[])
{
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0; /* This is our "service" */

    /* Create well-known FIFO, and open it for reading */

    umask(0); /* So we get the permissions we want */

    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 // @1
            && errno != EEXIST)
        errExit("mkfifo %s", SERVER_FIFO);
    
    // 最初のクライアントがこの書き込み用FIFOをオープンするまでこのopenはブロックする
    serverFd = open(SERVER_FIFO, O_RDONLY); // @2
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);
    
    /* Open an extra write descriptor, so that we never see EOF */
    dummyFd = open(SERVER_FIFO, O_WRONLY); // @3
    if (dummyFd == -1)
        errExit("open %s", SERVER_FIFO);
    
    // write()側で読み取り側がいなかったときにSIGPIPEを発生させずEPIPEエラーにするために必要
    // サーバー -> クライアント書き込みでの話
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) // @4
        errExit("signal");
    
    for (;;) { /* Read requests and send responses */  // @5
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue; /* Either partial read or error */
        } 

        /* Open client FIFO (previously created by client)*/
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid); // @6
        clientFd = open(clientFifo, O_WRONLY); // @7
        if (clientFd == -1) { /* Open failed, give up on client */
            errExit("open %s", clientFifo);
            continue; // @8
        }

        /* Send response and close FIFO */
        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        if (close(clientFd) == -1)
            errMsg("close");
        
        seqNum += req.seqLen; /* Update our sequence number */
    }
}
