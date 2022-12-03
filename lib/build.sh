#!/bin/sh

gcc -c error_functions.c get_num.c curr_time.c signal.c signal_functions.c itimerspec_from_str.c print_wait_status.c my.c
ar rcs libtlpi.a ./*.o
mv ./libtlpi.a ../
rm -f ./*.o
