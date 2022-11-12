#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>

/**
 * 端末に対してlseek(2)するとESPIPE(Illegal seek)で失敗する
 * 
 */
int
main(int argc, char *argv[])
{
    if (lseek(1, 10, SEEK_SET) == -1) {
        perror("lseek");
        exit(1);
    }
    write(1, "hoge", 4);

    exit(EXIT_SUCCESS);
}
