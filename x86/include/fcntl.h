#ifndef __FCNTL_H__
#define __FCNTL_H__
#include <stdint.h>
typedef uint32_t mode_t;

#define O_CREAT       0x0100  // create and open file
#define O_TRUNC       0x0200  // open and truncate
#define O_EXCL        0x0400  // open only if file doesn't already exist

#endif