#define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from stdio.h */

#include <stdio.h>
#include <string.h> /* Get declaration of strerror() */
#include <pthread.h>
#include "tlpi_hdr.h"

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorKey;

/* Maximum length of string returned by strerror() */
#define MAX_ERROR_LEN 256

int DBG = 1;

static void         /* Free thread-specific data buffer */
destructor(void *buf) // [1]
{
    DBG && printf("called destructor(): %p\n", buf);
    free(buf);
}

static void         /* Once-time key creation function */
createKey(void) // [2]
{
    int s;

    /* Allocate a unique thread-specific data key and save the address
       of the destructor for thread-specific data buffers */
    s = pthread_key_create(&strerrorKey, destructor); // [3]
    if (s != 0)
        errExitEN(s, "pthread_key_create");
}


char *
strerror(int err)
{
    int s;
    char *buf;

    // DBG && printf("custom strerror\n");

    /* Make first caller allocate key for thread-specific data */
    s = pthread_once(&once, createKey); // [4]
    if (s != 0)
        errExitEN(s, "pthread_once");
    
    buf = pthread_getspecific(strerrorKey); // [5]
    if (buf == NULL) {/* If first call from this thread, allocate buffer for thread, and save its location */
        buf = malloc(MAX_ERROR_LEN); // [6]
        if (buf == NULL) {
            errExit("malloc");
        }
        DBG && printf("malloc called: %p\n", buf);

        s = pthread_setspecific(strerrorKey, buf); // [7]
        if (s != 0)
            errExitEN(s, "pthread_setspecific");
    }

    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';
    }

    return buf;
}
