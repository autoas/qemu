/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include <stdint.h>
#include "spinlock.h"
#include "smp.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void spin_lock(spinlock_t *lock) {
  uint32_t val, fail;

#ifdef __AARCH64__
  do {
    asm volatile("1:	ldaxr	%w0, [%2]\n"
                 "	cbnz	%w0, 1b\n"
                 "	mov	%0, #1\n"
                 "	stxr	%w1, %w0, [%2]\n"
                 : "=&r"(val), "=&r"(fail)
                 : "r"(&lock->v)
                 : "cc");
  } while (fail);
#else
  do {
    asm volatile("1:	ldrex	%0, [%2]\n"
                 "	teq	%0, #0\n"
                 "	bne	1b\n"
                 "	mov	%0, #1\n"
                 "	strex	%1, %0, [%2]\n"
                 : "=&r"(val), "=&r"(fail)
                 : "r"(&lock->v)
                 : "cc");
  } while (fail);
#endif
  smp_mb();
}

void spin_unlock(spinlock_t *lock) {
  smp_mb();
  lock->v = 0;
}
