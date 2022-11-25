#!/bin/sh

gcc -c error_functions.c get_num.c curr_time.c signal.c signal_functions.c my.c
ar rcs libtlpi.a error_functions.o get_num.o curr_time.o signal.o signal_functions.o my.o
mv ./libtlpi.a ../
rm -f ./*.o
