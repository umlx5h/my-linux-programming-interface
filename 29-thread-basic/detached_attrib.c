#include <pthread.h>
#include "tlpi_hdr.h"

static void *
threadFunc(void *arg)
{
    return arg;
}

int
main(int argc, char *argv[])
{
    pthread_t thr;
    pthread_attr_t attr;
    int s;

    s = pthread_attr_init(&attr); /* Alligns default values */
    if (s != 0)
        errExitEN(s, "pthread_attr_init");
    
    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (s != 0)
        errExitEN(s, "pthread_attr_setdetachstate");
    
    s = pthread_create(&thr, &attr, threadFunc, (void *) 0);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_attr_destroy(&attr); /* No longer needed */
    if (s != 0)
        errExitEN(s, "pthread_attr_setdetachstate");

    void *res;
    s = pthread_join(thr, &res);
    if (s != 0)
        // PTHREAD_CREATE_DETACHEDでjoinできなくしているのでエラーになる
        errExitEN(s, "pthread_join failed as expected");

    exit(EXIT_SUCCESS);
}
