#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


int add(int a, int b) {
    return + b;
}


int main() {

    int i;
    for (i = 0; i < 10000 * 10000; i++) {
        add(i, 1000);
    }

    return 0;
}
