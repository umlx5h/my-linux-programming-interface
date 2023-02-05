#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
1 > file 2 > file だとオフセット位置を共有しないので上書きするが
1 >> file 2 >> file でO_APPENDフラグを付けると問題ない
*/

int
main(int argc, char *argv[])
{
    int i;
    for (i = 0; i < 1000; i++) {
        write(1, "stdout\n", 7);
        // if (i > 10) {
        //     sleep(1000);
        // }
        write(2, "stderr\n", 7);
    }    
    
    exit(EXIT_SUCCESS);
}
