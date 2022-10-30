#!/bin/sh

gcc -c error_functions.c get_num.c curr_time.c my.c
ar rcs libtlpi.a error_functions.o get_num.o curr_time.o my.o
mv ./libtlpi.a ../
rm -f ./*.o
