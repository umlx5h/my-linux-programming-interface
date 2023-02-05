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
2回ファイルをopen(2)してオフセット位置を共有しない場合は
O_APPENDなしでは上書きしてしまう
*/

int
main(int argc, char *argv[])
{
    int i;
    int f1, f2;

    int oflags;
    oflags = O_WRONLY | O_CREAT | O_TRUNC;
    /* O_APPENDをつけないと上書きしてしまう */
    // oflags |= O_APPEND;

    f1 = open(FILENAME, oflags, 0777);
    if (f1 == -1)
        perror("f1");
        
    f2 = open(FILENAME, oflags, 0777);
    if (f2 == -1)
        perror("f2");

    // stop("before dup2");

    if (f1 != STDOUT_FILENO) {
        dup2(f1, STDOUT_FILENO);
        close(f1);
    }

    if (f2 != STDERR_FILENO) {
        dup2(f2, STDERR_FILENO);
        close(f2);
    }


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
