#include <signal.h>
#include <tlpi_hdr.h>
#include <signal_functions.h>

static void
sigHandler(int sig)
{
    printf("Ouch!\n");
}

int
main(int argc, char *argv[])
{
    int j;

    sigset_t blockSet, prevSet;

    sigemptyset(&blockSet);
    sigaddset(&blockSet, SIGINT);
    if (sigprocmask(SIG_BLOCK, &blockSet, &prevSet) == -1)
        errExit("sigprocmask1");

    for (j = 0; ; j++) {
        printf("%d\n", j);
        sleep(1);

        if (j >= 5) {
            // ループ実行中にSIGINTを送ったときに無視されるのではなくブロックされた結果保留されている
            // なのでこの関数がリターンする前にSIGINTのデフォルト動作でプロセス終了となる
            printPendingSigs(stderr, "pending signal:");
            if (sigprocmask(SIG_SETMASK, &prevSet, NULL) == -1)
                errExit("sigprocmask2");

            printf("never reached!\n");
        }
    }
}
