#include <stdio.h>
#include <string.h> /* Get declaration of strerror() */
#include <pthread.h>
#include "tlpi_hdr.h"

static void *
threadFunc(void *arg)
{
    char *str;

    printf("Other thread 1 about to call strerror()\n");
    str = strerror(EPERM);
    printf("Other thread 1: str (%p) = %s\n", str, str);

    sleep(1);
    return NULL;
}

static void *
threadFunc2(void *arg)
{
    char *str;

    printf("Other thread 2 about to call strerror()\n");
    str = strerror(EINTR);
    printf("Other thread 2: str (%p) = %s\n", str, str);

    sleep(1);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t, t2;
    int s;
    char *str;

    str = strerror(EINVAL);
    printf("Main thread has called strerror()\n");

    s = pthread_create(&t, NULL, threadFunc, NULL);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_create(&t2, NULL, threadFunc2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_create");

    s = pthread_join(t, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");

    printf("Main thread: str (%p) = %s\n", str, str);

    // pthread_exit(NULL);
    exit(EXIT_SUCCESS);
}
