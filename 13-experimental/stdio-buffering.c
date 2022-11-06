#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*
# stdin端末、stdout端末の場合: read, writeは改行文字ごとに呼ばれる
gcc -g stdio-buffering.c && strace -e trace=write,read ./a.out

# stdin端末、stdoutファイルの場合: readは改行文字ごとに呼ばれて、writeはまとめて呼ばれる
gcc -g stdio-buffering.c && strace -e trace=write,read ./a.out | tee /dev/null

# stdinファイル、stdout端末の場合: readはまとめて呼ばれてwriteは改行文字ごとに呼ばれる

# stdinファイル、stdoutファイルの場合: readはまとめて呼ばれてwriteもまとめて呼ばれる
gcc -g stdio-buffering.c && strace -e trace=write,read ./a.out < test.txt | tee /dev/null
*/

int
main(int argc, char *argv[])
{
    char buf[1024];
    while (fgets(buf, sizeof buf, stdin) != NULL) {
        fprintf(stdout, "%s", buf);
    }

    exit(EXIT_SUCCESS);
}
