#include <tlpi_hdr.h>

#define COMMAND_SIZE 4096
static char command[COMMAND_SIZE];

int main(void) {
    pid_t pid;

    write(0, "INIT_DONE\n", 10);

    printf("first: %p\n", sbrk(0));
    // 小さいサイズを指定すると内部でbrk(2)が利用されてprogram breakが移動する
    char *small = malloc(10000);
    printf("after small malloc: program break=%p, ptr=%p\n", sbrk(0), small);
    // 大きいサイズを指定すると内部でmmap(2)が利用されてprogram breakは移動しない (ファイルマッピングはヒープとスタックの間の領域に確保される)
    char *big = malloc(100 * 1024 * 1024);
    printf("after big malloc: program break=%p, ptr=%p\n", sbrk(0), big);

    pid = getpid();
    snprintf(command, COMMAND_SIZE, "cat /proc/%d/maps", pid);
    system(command);

    free(small);
    free(big);
    exit(0);
}
