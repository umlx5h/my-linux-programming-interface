#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


int main() {

    int i;
    for (i = 0; i < 10000 * 10000; i++) {
        getppid();
    }

    return 0;
}
