#ifndef INET_SOCKETS_H
#define INET_SOCKETS_H /* Prevent accidental double inclusion */

#include <sys/socket.h>
#include <netdb.h>

// TCP, UDPクライアント用 socket(2), connect(2)を実行してファイルディスクリプタを返す
// 成功時はファイルディスクリプタを、エラー時は −1 を返す
int inetConnect(const char *host, const char *service, int type);

// TCPサーバ用 socket(2), bind(2), listen(2)を実行してファイルディスクリプタを返す
// 成功時はファイルディスクリプタを、エラー時は −1 を返す
int inetListen(const char *service, int backlog, socklen_t *addrlen);

// UDPサーバ, クライアント用 socket(2), bind(2)を実施
// 成功時はファイルディスクリプタを、エラー時は −1 を返す
int inetBind(const char *service, int type, socklen_t *addrlen);

// インターネットソケットアドレスを可読形式へ変換 形式: (hostname, port-number)
// ホスト名、サービス名の文字列を addrStr に生成し、addrStr を返す
char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen);

#define IS_ADDR_STR_LEN 4096
                    /* Suggested length for string buffer that caller
                        should pass to inetAddressStr(). Must be greater
                        than (NI_MAXHOST + NI_MAXSERV + 4) */

#endif
