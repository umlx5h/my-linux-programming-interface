#include <tlpi_hdr.h>
#include <sys/wait.h>


/**
 * stty -tostop を実行してからこのプログラムを実行するとSIGTTOUでTになるのが再現できる
 * fgetc(stdin)で標準入力から読んだ場合はSIGTTINが必ず発生
 */

int
main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    // 親プロセスはプロセスグループリーダとなっているので、setpgidしても意味がない
    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        // ここでsetpgidを呼び、フォアグラウンドプロセスグループから抜ける
        // -> ほぼバックグラウンドプロセスグループと同じ (シェルはこのプロセスを認識していない)
        setpgid(0, 0);
        // fgetc(stdin); // SIGTTINを再現したい場合
        printf("write from child!\n");
        // stop("child");
        exit(EXIT_SUCCESS);
    }

    // system("stty -tostop");
    printf("write from parent!\n");
    wait(NULL);
    // stop("parent");

    exit(EXIT_SUCCESS);
}
