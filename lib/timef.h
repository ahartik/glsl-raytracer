#ifndef TIMEF_H
#define TIMEF_H
#include<sys/time.h>

static double timef() {
  static bool init = false;
  static struct timeval start = {0};

  if (!init) {
    gettimeofday(&start, 0);
    init = true;
  }

  struct timeval tv = {0};
  gettimeofday(&tv, NULL);

  return (tv.tv_sec - start.tv_sec) + (tv.tv_usec - start.tv_usec) / 1000000.;
}

#endif
