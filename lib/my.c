#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "my.h"

void
stop(const char *message)
{
    printf("# [PID:%d] %s\n", (int) getpid(), message);
    getchar();
}
