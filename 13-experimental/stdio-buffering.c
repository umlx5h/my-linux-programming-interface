#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*
# stdin端末、stdout&stderr端末の場合: read, writeは改行文字ごとに呼ばれる
gcc -g stdio-buffering.c && strace -e trace=write,read ./a.out

# stdin端末、stdoutファイルの場合: readは改行文字ごとに呼ばれて、writeはまとめて呼ばれる
# stdin端末、stderrファイルの場合: read, writeは改行文字ごとに呼ばれる
gcc -g stdio-buffering.c && strace -e trace=write,read ./a.out | tee /dev/null

# stdinファイル、stdout端末の場合: readはまとめて呼ばれてwriteは改行文字ごとに呼ばれる
gcc -g stdio-buffering.c && strace -e trace=write,read ./a.out < test.txt

# stdinファイル、stdoutファイルの場合: readはまとめて呼ばれてwriteもまとめて呼ばれる
# stdinファイル、stderrファイルの場合: readはまとめて呼ばれてwriteは改行文字ごとに呼ばれる
gcc -g stdio-buffering.c && strace -e trace=write,read ./a.out < test.txt | tee /dev/null


### 結論
stdin, stdoutは端末の場合はラインバッファリング、ファイルの場合はフルバッファリングになるが
stderrの場合は端末でもファイルでも完全にノーバッファリングである。
*/

int
main(int argc, char *argv[])
{
    fprintf(stderr, "%d\n", getpid());
    char buf[1024];
    while (fgets(buf, sizeof buf, stdin) != NULL) {
        // delete line break
        // buf[strcspn(buf, "\n")] = 0;
        // fprintf(stderr, "%s", buf);
        fprintf(stdout, "%s", buf);
    }

    pause();
    exit(EXIT_SUCCESS);
}
