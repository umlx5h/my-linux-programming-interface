#include <pthread.h>
#include "tlpi_hdr.h"

static pthread_cond_t threadDied = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;
/* Protects all of the following global variables */

static int totThreads = 0;  /* Total number of threads created */
static int numLive = 0;     /* Total number of threads still alive or
                               terminated but not yet joined */
static int numUnjoined = 0; /* Number of terminated threads that
                               have not yet been joined */

enum tstate {               /* Threads states */
    TS_ALIVE,               /* Thread is alive */
    TS_TERMINATED,          /* Thread terminated, not yet joined */
    TS_JOINED,              /* Thread terminated, and joined */
};

static struct {             /* Info about each thread */
    pthread_t tid;          /* ID of this thread */
    enum tstate state;      /* Thread state (TS_* constants above) */
    int sleepTime;          /* Number seconds to live before terminating */
} *thread;

static void *
threadFunc(void *arg)
{
    int idx = (int) arg;
    int s;

    sleep(thread[idx].sleepTime); /* Simulate doing some work */
    printf("Thread %d terminating\n", idx);

    s = pthread_mutex_lock(&threadMutex);
    if (s != 0)
        errExitEN(s, "pthread_mutex_lock");
    numUnjoined++;
    thread[idx].state = TS_TERMINATED;

    s = pthread_mutex_unlock(&threadMutex);
    if (s != 0)
        errExitEN(s, "pthread_mutex_unlock");
    s = pthread_cond_signal(&threadDied);
    if (s != 0)
        errExitEN(s, "pthread_cond_signal");
    
    return NULL;
}

int
main(int argc, char *argv[])
{
    int s, idx;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s nsecs...\n", argv[0]);
    
    thread = calloc(argc - 1, sizeof(*thread));
    if (thread == NULL)
        errExit("calloc");

    totThreads = argc - 1;
    numLive = totThreads;
    
    /* Create all threads */
    for (idx = 0; idx < totThreads; idx++) {
        thread[idx].sleepTime = getInt(argv[idx + 1], GN_NONNEG, NULL);
        thread[idx].state = TS_ALIVE;
        s = pthread_create(&thread[idx].tid, NULL, threadFunc, (void *) idx);
        if (s != 0)
            errExitEN(s, "pthread_create");
    }

    /* Join with terminated threads */
    while (numLive > 0) {
        printf("main loop\n");
        s = pthread_mutex_lock(&threadMutex);
        if (s != 0)
            errExitEN(s, "pthread_mutex_lock");
        
        while (numUnjoined == 0) { // whileにする必要がある条件 (condition)
            // 目的の状態になるまで (numUnjoined > 0になる)待機する (CPUを使わない)
            s = pthread_cond_wait(&threadDied, &threadMutex);
            if (s != 0)
                errExitEN(s, "pthread_cond_wait");
        }

        // 目的の状態になった
        for (idx = 0; idx < totThreads; idx++) {
            if (thread[idx].state == TS_TERMINATED) {
                s = pthread_join(thread[idx].tid, NULL);
                if (s != 0)
                    errExitEN(s, "pthread_join");
                
                thread[idx].state = TS_JOINED;
                numLive--;
                numUnjoined--;

                printf("Readped thread %d (numLive=%d)\n", idx, numLive);
            }
        }

        s = pthread_mutex_unlock(&threadMutex);
        if (s != 0)
            errExitEN(s, "pthread_mutex_unlock");
    }

    exit(EXIT_SUCCESS);
}
