/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 41 */

/* prog.c */

#include <stdio.h>
#include <stdlib.h>

#include "mod1.h"
#include "mod2.h"

// void x1(void);
// void x2(void);

int
main(int argc, char *argv[])
{
    x1();
    x2();
    // x3();
    exit(EXIT_SUCCESS);
}
