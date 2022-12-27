#define _GNU_SOURCE /* Get '_sys_nerr' and '_sys_errlist' declarations from stdio.h */

#include <stdio.h>
#include <string.h> /* Get declaration of strerror() */

/* Maximum length of string returned by strerror() */
#define MAX_ERROR_LEN 256

static __thread char buf[MAX_ERROR_LEN]; /* Thread-local return buffer */

char *
strerror(int err)
{
    printf("custom strerror\n");
    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0'; /* Ensure null termination */
    }

    return buf;
}
