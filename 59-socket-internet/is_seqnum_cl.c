#include "netdb.h"
#include "is_seqnum.h"

int
main(int argc, char *argv[])
{
    char *reqLenStr; /* Requested length of sequence */
    char seqNumStr[INT_LEN]; /* Start of granted sequence */
    int cfd;
    ssize_t numRead;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s server-host [sequence-len]\n", argv[0]);
    
    /* Call getaddrinfo() to obtain a list of addresses that
        we can try connecting to */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    // ここから意味のある設定
    hints.ai_family = AF_UNSPEC; /* Allows IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0) // @1
        errExit("getaddrinfo");
    
    /* Walk through returned list until we find an address structure 
      that can be used to successfully connect a socket */
    
    for (rp = result; rp != NULL; rp = rp->ai_next) { // @2
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); // @3
        if (cfd == -1)
            continue; /* On error, try next address */
        
        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1) // @4
            break; /* Success */
        
        /* Connect failed: close this socket and try next address */
        close(cfd);
    }

    if (rp == NULL)
        fatal("Could not connect socket to any address");
    
    freeaddrinfo(result);

    /* Send requested sequence length, with terminating newline */
    reqLenStr = (argc > 2) ? argv[2] : "1"; // @5
    if (write(cfd, reqLenStr, strlen(reqLenStr)) != strlen(reqLenStr))
        fatal("partial/failed write (reqLenStr)");
    if (write(cfd, "\n", 1) != 1)
        fatal("partial/failed write (newLine)");
    
    /* Read and display sequence number retuend by server */
    numRead = readLine(cfd, seqNumStr, INT_LEN); // @6
    if (numRead == -1)
        errExit("readLine");
    if (numRead == 0)
        fatal("Unexpected EOF from server");
    
    printf("Sequence number: %s", seqNumStr); // @7 /* Includes '\n' */

    exit(EXIT_SUCCESS);
}
