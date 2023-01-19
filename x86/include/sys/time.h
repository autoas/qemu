#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

typedef long time_t;

struct timespec {
  time_t tv_sec; /* seconds */
  long tv_nsec;  /* and nanoseconds */
};

struct timeval {
  time_t tv_sec; /* seconds */
  long tv_usec;  /* and microseconds */
};

typedef	unsigned long useconds_t;	/* microseconds (unsigned) */

#endif