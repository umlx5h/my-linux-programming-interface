#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <my.h>

#define FILENAME "./logging"

/*
dup2でファイルディスクリプタを1番から2番にコピーすれば上書きする問題は発生しない
O_APPENDは不要
*/

int
main(int argc, char *argv[])
{
    int i;
    int f1, f2;

    int oflags;
    oflags = O_WRONLY | O_CREAT | O_TRUNC;
    /* オフセット位置を共有しているのでO_APPENDは不要になる */
    // oflags |= O_APPEND;

    f1 = open(FILENAME, oflags, 0777);
    if (f1 == -1)
        perror("f1");
        
    if (f1 != STDOUT_FILENO) {
        dup2(f1, STDOUT_FILENO);
        close(f1);
    }

    dup2(STDOUT_FILENO, STDERR_FILENO);

    // stop("after dup2");
    switch (fork()) {
    case 0:
        for (i = 0; i < 1000; i++) {
            write(STDOUT_FILENO, "stdout\n", 7);
            // sleep(1000);
            write(STDERR_FILENO, "stderr\n", 7);
        }

        _exit(0);
    }

    wait(NULL);
    
    exit(EXIT_SUCCESS);
}
