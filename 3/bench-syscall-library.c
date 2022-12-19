#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


long add(long a, long b) {
    return a + b;
}


int main() {
    long i;
    for (i = 0; i < (long)10000 * 1000000; i++) {
    // for (;;) {
        add(i, 1000);
    }

    return 0;
}
