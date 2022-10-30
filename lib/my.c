#include <stdio.h>
#include "my.h"

void
stop(const char *message)
{
    printf("# %s\n", message);
    getchar();
}
