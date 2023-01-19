#include <semaphore.h>
#include <pthread.h>
#include <tlpi_hdr.h>

// volatile prevents compiler optimizations of arithmetic operations on glob
static volatile int glob = 0;
static sem_t sem;

// Loop 'arg' times incrementing 'glob'
static void *
threadFunc(void *arg)
{
    int loops = *((int *) arg);
    int loc, j, s;

    for (j = 0; j < loops; j++) {
        s = sem_wait(&sem);
        if (s != 0)
            errExit("sem_wait");

        loc = glob;
        loc++;
        glob = loc;

        // glob++;

        s = sem_post(&sem);
        if (s != 0)
            errExit("sem_post");
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int loops, s;

    loops = (argc > 1) ? getInt(argv[1], GN_GT_0, "num_loops") : 10000000;

    /* Initialize a semaphore with the value 1 */
    s = sem_init(&sem, 0, 1);
    if (s != 0)
        errExit("sem_init");

    /* Create tho threads that increment 'glob' */
    s = pthread_create(&t1, NULL, threadFunc, &loops);
    if (s != 0)
        errExitEN(s, "pthread_create");
    s = pthread_create(&t2, NULL, threadFunc, &loops);
    if (s != 0)
        errExitEN(s, "pthread_create");
    
    /* Wait for threads to terminate */
    s = pthread_join(t1, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        errExitEN(s, "pthread_join");
    
    printf("glob = %d\n", glob);

    s = sem_destroy(&sem);
    if (s != 0)
        errExit("sem_destroy");

    exit(EXIT_SUCCESS);
}
