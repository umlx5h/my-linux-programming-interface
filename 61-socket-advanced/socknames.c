#include "inet_sockets.h"
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int listenFd, acceptFd, connFd;
    socklen_t len;              /* Size of socket address buffer */
    // void *addr;
    struct sockaddr *addr;      /* Buffer for socket address */
    char addrStr[IS_ADDR_STR_LEN];

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s service\n", argv[0]);
    
    listenFd = inetListen(argv[1], 5, &len);
    if (listenFd == -1)
        errExit("inetListen");
    
    // accept前にconnectしているが特にブロックしない、カーネルが裏でTCPコネクションの確立&キューイングしているため
    connFd = inetConnect(NULL, argv[1], SOCK_STREAM);
    if (connFd == -1)
        errExit("inetConnect");
    
    acceptFd = accept(listenFd, NULL, NULL);
    if (acceptFd == -1)
        errExit("accept");

    // IPv4かIPv6かどちらかわからないので動的に確保する必要がある
    addr = malloc(len);
    if (addr == NULL)
        errExit("malloc");

    if (getsockname(connFd, addr, &len) == -1) 
        errExit("getsockname");
    printf("getsockname(connFd)   %s\n",
        inetAddressStr(addr, len, addrStr, IS_ADDR_STR_LEN));

    if (getsockname(acceptFd, addr, &len) == -1) 
        errExit("getsockname");
    printf("getsockname(acceptFd)   %s\n",
        inetAddressStr(addr, len, addrStr, IS_ADDR_STR_LEN));

    if (getpeername(connFd, addr, &len) == -1) 
        errExit("getpeername");
    printf("getpeername(connFd)   %s\n",
        inetAddressStr(addr, len, addrStr, IS_ADDR_STR_LEN));

    if (getpeername(acceptFd, addr, &len) == -1) 
        errExit("getpeername");
    printf("getpeername(acceptFd)   %s\n",
        inetAddressStr(addr, len, addrStr, IS_ADDR_STR_LEN));
    
    stop("confirm 'ss'");
    exit(EXIT_SUCCESS);
}
