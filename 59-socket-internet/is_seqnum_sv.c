#define _BSD_SOURCE /* To get definitions of NI_MAXHOST and
                        NI_MAXSERV from <netdb.h> */

#include <netdb.h>
#include "is_seqnum.h"

#define BACKLOG 50

int
main(int argc, char *argv[])
{
    uint32_t seqNum;
    char reqLenStr[INT_LEN]; /* Length of requested sequence */
    char seqNumStr[INT_LEN]; /* Start of granted sequence */
    struct sockaddr_storage claddr;
    int lfd, cfd, optval, reqLen;
    socklen_t addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [init-seq-num]\n", argv[0]);
    
    seqNum = (argc > 1) ? getInt(argv[1], 0, "init-seq-num") : 0; // @1
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) // @2
        errExit("signal");

    /* Call getaddrinfo() to obtain a list of addresses that
        we can try binding to */

    memset(&hints, 0, sizeof(struct addrinfo));
    /* ここから */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    /* ここまで 使われてない (0初期化でもよい) */

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC; /* Allows IPv4 or IPv6 */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV; // @3 /* Wildcard IP address; service name is numeric */

    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0) // @4
        errExit("getaddrinfo");

    /* Walk through returned list until we find an address structure
        that can be used to successfully create and bind a socket */
    
    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) { // @5
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1)
            continue; /* On error, try next address */

        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optarg, sizeof(optval)) == -1) // @6
            errExit("setsockopt");

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0) // @7
            break; /* Success */
        
        /* bind() failed: close this socket and try next address */

        close(lfd);
    }

    if (rp == NULL)
        fatal("Could not bind socket to any address");
    
    if (listen(lfd, BACKLOG) == -1) // @8
        errExit("listen");

    freeaddrinfo(result);

    for (;;) { // @9 /* Handle clients iteratively */
        /* Accept a client connection, obtaining client's address */

        addrlen = sizeof(struct sockaddr_storage); // sockaddr_storageはipv4, v6両対応するためのもの
        cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen); // @10
        if (cfd == -1) {
            errMsg("accept");
            continue;
        }

        if (getnameinfo((struct sockaddr *) &claddr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) // @11
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
        printf("Connection from %s\n", addrStr);

        /* Read client request, send sequence number back */
        if (readLine(cfd, reqLenStr, INT_LEN) <= 0) { // @12
            close(cfd);
            continue; /* Failed read; skip request */
        }

        reqLen = atoi(reqLenStr); // @13
        if (reqLen <= 0) { /* Watch for misbehaving clients */
            close(cfd);
            continue; /* Bad request; skip it */
        }

        snprintf(seqNumStr, INT_LEN, "%d\n", seqNum); // @14
        if (write(cfd, seqNumStr, strlen(seqNumStr)) != strlen(seqNumStr))
            fprintf(stderr, "Error on write");

        seqNum += reqLen; // @15 /* Update sequence number */
        if (close(cfd) == -1) /* Close connection */
            errMsg("close");
    }
}
