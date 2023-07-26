#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

int main(int argc, char *argv[])
{
    struct timeval current_time;
    long i;

    for (i = 0; i < 50000000; i++) {
    // for (;;) {
        // straceに出てこない & システムコールだがvdsoによって%userのみあがる
        gettimeofday(&current_time, NULL);
        // straceに出る & vdsoが適用されないので%userと%sysがあがる
        getppid();
        // ライブラリ関数なので%userだけ上がる
        strerror(1);
    }

    exit(EXIT_SUCCESS);
}
