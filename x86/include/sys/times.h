#ifndef __SYS_TIMES_H__
#define __SYS_TIMES_H__

#define	_CLOCK_T_	unsigned long	/* clock() */
typedef	_CLOCK_T_	clock_t;

/*  Get Process Times, P1003.1b-1993, p. 92 */
struct tms {
  clock_t tms_utime;  /* user time */
  clock_t tms_stime;  /* system time */
  clock_t tms_cutime; /* user time, children */
  clock_t tms_cstime; /* system time, children */
};
#endif