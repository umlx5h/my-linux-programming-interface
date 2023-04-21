#include <stdio.h>
#include <unistd.h>

int
main(void)
{
    printf("ctermid(): %s\n", ctermid(NULL));
    printf("ttyname(): %s\n", ttyname(0));

    pause();
}
