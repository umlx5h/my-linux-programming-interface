#define _GNU_SOURCE

#include <tlpi_hdr.h>
#include <signal.h>
#include <signal_functions.h>

typedef void (*sighandler_t)(int);

sighandler_t
signal_using_sigaction(int sig, sighandler_t handler)
{
    struct sigaction newDisp, prevDisp;
    newDisp.sa_handler = handler;
#ifdef OLD_SIGNAL
    // 古いsignal() = sysv_signal()相当の挙動にするフラグを設定
    newDisp.sa_flags = SA_RESETHAND | SA_NODEFER;
#else
    newDisp.sa_flags = SA_RESTART;
#endif

    if (sigaction(sig, &newDisp, &prevDisp) == -1)
        return SIG_ERR;
    else
        return prevDisp.sa_handler;
}

static void
sigHandler(int sig)
{
    // printf("Ouch");
    write(1, "Ouch", 4);
    sleep(1);
    write(1, "!!!!!!\n", 7);
    // printf("!\n");

    // 古いsignal()の実装では (sysv_signal) シグナルハンドラ実行前にデフォルト動作に戻すので再度登録する必要がある
    // signal_using_sigaction(SIGINT, sigHandler);
}

int
main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = sigHandler;
    sigemptyset(&act.sa_mask);
    // WARNING: 初期化しないとact構造体はauto変数なので不定値になっている、おかしな挙動となるので必ず初期化すること！
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    // signal(SIGINT, sigHandler);
    // signal_using_sigaction(SIGINT, sigHandler);
    // sysv_signal(SIGINT, sigHandler);
    for (;;) {
        printf("waiting signal\n");
        pause();
        // ;
    }
}

