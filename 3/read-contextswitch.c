#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int
main(void)
{
    char c;
    int i;
    int fd;

    fd = open("./out", O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }

    for (i = 0; i < 1000; i++) {
        // 以下のようにread(2)を発行するが、/usr/bin/time -vで見てもコンテキストスイッチは一切発生しない
        // これはページキャッシュにヒットしたときのreadはオンメモリ処理であり遅いシステムコールになっていないため

        // 以下のコマンドでページキャッシュを破棄してから再度実行するとinvoluntary context switchesが1->2になることがわかった
        // やはりページキャッシュにヒットしない初回だけ'遅い'システムコールになるがこの時にコンテキストスイッチが発生することがわかった
        // sudo sync && echo 3 | sudo tee /proc/sys/vm/drop_caches
        if (read(fd, &c, 1) == -1) {
            perror("read failed");
            exit(1);
        } 
    }

    return 0;
}
