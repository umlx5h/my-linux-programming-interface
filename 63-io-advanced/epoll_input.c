#include <sys/epoll.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define MAX_BUF     1000 /* Maximum bytes fetched by a single read() */
#define MAX_EVENTS     5 /* Maximum number of events to be returned from
                                a single epoll_wait() call */

int main(int argc, char *argv[])
{

}
