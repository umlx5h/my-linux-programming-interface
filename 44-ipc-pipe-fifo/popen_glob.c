#include <ctype.h>
#include <limits.h>
#include "print_wait_status.h"
#include "tlpi_hdr.h"

#define POPEN_FMT "/bin/ls -d %s 2> /dev/null" // [1]
#define PAT_SIZE 50
#define PCMD_BUF_SIZE (sizeof(POPEN_FMT) + PAT_SIZE)

int
main(int argc, char *argv[])
{
    char pat[PAT_SIZE]; /* Pattern for globbing */
    char popenCmd[PCMD_BUF_SIZE];
    FILE *fp; /* File stream returned by poepn() */
    Boolean badPattern; /* Invalid characters in 'pat'? */
    int len, status, fileCnt, j;
    char pathname[PATH_MAX];

    for (;;) { /* Read pattern, display results of globbing */
        printf("pattern: ");
        fflush(stdout);
        if (fgets(pat, PAT_SIZE, stdin) == NULL) // [2]
            break;
        len = strlen(pat);
        if (len <= 1) /* Empty line */
            continue;

        if (pat[len - 1] == '\n') /* Strip trailing newline */
            pat[len - 1] = '\0';
        
        /* Ensure that the pattern contains only valid characters,
        i.e., letters, digits, underscore, dot, and the shell
        globbing characters. (Our definition of valid is more
        restrictive than the shell, which permits other characters
        to be included in a filename if they are quoted.) */
        for (j = 0, badPattern = FALSE; j < len && !badPattern; j++) // [3]
            if (!isalnum((unsigned char) pat[j]) && strchr("/_*?[^-].", pat[j]) == NULL)
                badPattern = TRUE;
        
        if (badPattern) {
            printf("Bad pattern character: %c\n", pat[j - 1]);
            continue;
        }

        /* Build and execute command to glob 'pat' */
        snprintf(popenCmd, PCMD_BUF_SIZE, POPEN_FMT, pat); // [4]

        fp = popen(popenCmd, "r"); // [5]
        if (fp == NULL) {
            printf("open() failed\n");
            continue;
        }

        /* Read resulting list of pathnames until EOF */
        fileCnt = 0;
        while (fgets(pathname, PATH_MAX, fp) != NULL) {
            printf("%s", pathname);
            fileCnt++;
        }

        /* Close pipe, fetch and display termination status */
        status = pclose(fp);
        printf("      %d matching file%s\n", fileCnt, (fileCnt != 1) ? "s" : "");
        printf("      pclose() status = %#x\n", (unsigned int) status);
        if (status != -1)
            printWaitStatus("\t", status);
    }

    exit(EXIT_SUCCESS);
}
