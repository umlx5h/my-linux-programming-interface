#include <pthread.h>
#include "tlpi_hdr.h"

static void *
threadFunc(void *arg)
{
    char *s = (char *) arg;
    printf("[%d:%d:%ld] %s", getpid(), gettid(), (long) pthread_self(), s);

    // stop("sub");
    // equivelent
    pthread_exit((void *) strlen(s));
    // return (void *) strlen(s);
}

int
main(int argc, char *argv[])
{
    pthread_t t1;
    void *res;
    int s;

    // stop("before thread");
    s = pthread_create(&t1, NULL, threadFunc, "Hello world\n");
    if (s != 0)
        errExitEN(s, "pthread_create");
    
    printf("[%d:%d:%ld] Message from main()\n", getpid(), gettid(), (long) pthread_self());
    // stop("main");
    s = pthread_join(t1, &res);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("Thread returned %ld\n", (long) res);

    stop("after join");
    exit(EXIT_SUCCESS);
}
