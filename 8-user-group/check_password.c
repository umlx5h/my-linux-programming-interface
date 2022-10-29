/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 8-2 */

/* check_password.c

   Read a user name and password and check if they are valid.

   This program uses the shadow password file. Some UNIX implementations
   don't support this feature.
*/
/* Compile with -lcrypt */
#if ! defined(__sun)
#define _BSD_SOURCE     /* Get getpass() declaration from <unistd.h> */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE   /* Get crypt() declaration from <unistd.h> */
#endif
#endif
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    char *username, *password, *encrypted, *p;
    struct passwd *pwd;
    struct spwd *spwd;
    Boolean authOk;
    size_t len;
    long lnmax;

    /* Determine size of buffer required for a username, and allocate it */

    lnmax = sysconf(_SC_LOGIN_NAME_MAX);
    if (lnmax == -1)                    /* If limit is indeterminate */
        lnmax = 256;                    /* make a guess */

    username = malloc(lnmax);
    if (username == NULL)
        errExit("malloc");

    printf("Username: ");
    fflush(stdout);
    // fgets(3)は改行文字もバッファに入れる
    if (fgets(username, lnmax, stdin) == NULL)
        exit(EXIT_FAILURE);             /* Exit on EOF */

    // 長さに改行文字も含む
    len = strlen(username);
    if (username[len - 1] == '\n')
        username[len - 1] = '\0';       /* Remove trailing '\n' */

    /* Look up password and shadow password records for username */
    // /etc/passwdから読む
    pwd = getpwnam(username);
    if (pwd == NULL)
        fatal("couldn't get password record");
    // etc/shadowから読む
    spwd = getspnam(username);
    if (spwd == NULL && errno == EACCES)
        fatal("no permission to read shadow password file");

    if (spwd != NULL)           /* If there is a shadow password record */
        pwd->pw_passwd = spwd->sp_pwdp;     /* Use the shadow password */

    password = getpass("Password: ");

    // // 暗号パスワードの先頭2 <- 嘘っぽい文字がsaltなので取り出す 2文字+NULL文字=3
    // char salt[22];
    // strncpy(salt, pwd->pw_passwd, 21);
    // salt[21] = '\0';
    /* Encrypt password and erase cleartext version immediately */

    encrypted = crypt(password, pwd->pw_passwd);
    // encrypted = crypt(password, salt);
    // パスワードの領域をヌル文字で上書いて消している
    for (p = password; *p != '\0'; )
        *p++ = '\0';

    if (encrypted == NULL)
        errExit("crypt");

    authOk = strcmp(encrypted, pwd->pw_passwd) == 0;
    if (!authOk) {
        printf("Incorrect password\n");
        exit(EXIT_FAILURE);
    }

    printf("Successfully authenticated: UID=%ld\n", (long) pwd->pw_uid);

    /* Now do authenticated work... */

    exit(EXIT_SUCCESS);
}
