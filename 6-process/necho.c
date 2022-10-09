#include "tlpi_hdr.h"

int
main(int argc, char *argv[]) {
    int j;
    for (j = 0; j < argc; j++)
        printf("argv[%d] = %s\n", j, argv[j]);
    
    puts("");

    char **p;
    int i = 0;
    for (p = argv; *p != NULL; p++)
        printf("argv[%d] = %s\n", i++, *p);

    exit(EXIT_SUCCESS);
}
