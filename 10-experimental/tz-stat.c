#include <time.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
  int i = 0;
  time_t timep;

  printf("Greetings!\n");

  for (;;) {
    time(&timep);
    localtime(&timep);
  }

  printf("Godspeed, dear friend!\n");
  return 0;
}

