/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 *
 * lwIP sys_arch port of the bare metal ARM Versatile/PB target.
 *
 * lwIP is built with NO_SYS=0 + LWIP_SOCKET so that the unmodified TcpIp module of infras
 * (which is written against <lwip/sockets.h>) can be reused, but with
 * LWIP_TCPIP_CORE_LOCKING=1: in that mode tcpip_send_msg_wait_sem() calls the API function
 * directly in the calling context, so no tcpip thread is needed to serve the socket API.
 *
 * Consequently this port does not implement a scheduler at all:
 *  - the tcpip thread is never started, sys_thread_new() only returns a dummy handle;
 *  - ethernet frames are handed to the stack inline by TcpIp_MainFunction and the timers are
 *    served by sys_check_timeouts() from the very same place;
 *  - semaphores/mailboxes/mutexes only keep their state. If a blocking operation is used
 *    anyway, the wait pumps the network through Lwip_PortPump() so that the stack can still
 *    make progress, and it always returns after at most one second instead of hanging the
 *    bootloader forever.
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include <string.h>

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/err.h"

#include "Lwip_Port.h"
/* ================================ [ MACROS    ] ============================================== */
/* upper bound of a blocking wait, the stack of a bootloader must never hang */
#define SYS_WAIT_MAX_MS 1000
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void sys_init(void) {
}

/* ================================ [ MUTEX     ] ============================================== */
/* the whole stack is single threaded: locking is only counted, the core lock never waits */
err_t sys_mutex_new(sys_mutex_t *mutex) {
  if (NULL == mutex) {
    return ERR_ARG;
  }
  mutex->count = 0;
  mutex->valid = 1;
  return ERR_OK;
}

void sys_mutex_lock(sys_mutex_t *mutex) {
  if ((NULL != mutex) && (0 != mutex->valid)) {
    mutex->count++;
  }
}

void sys_mutex_unlock(sys_mutex_t *mutex) {
  if ((NULL != mutex) && (mutex->count > 0)) {
    mutex->count--;
  }
}

void sys_mutex_free(sys_mutex_t *mutex) {
  if (NULL != mutex) {
    mutex->valid = 0;
  }
}

int sys_mutex_valid(sys_mutex_t *mutex) {
  return ((NULL != mutex) && (0 != mutex->valid)) ? 1 : 0;
}

void sys_mutex_set_invalid(sys_mutex_t *mutex) {
  if (NULL != mutex) {
    mutex->valid = 0;
  }
}

/* ================================ [ SEMAPHORE ] ============================================== */
err_t sys_sem_new(sys_sem_t *sem, u8_t count) {
  if (NULL == sem) {
    return ERR_ARG;
  }
  sem->count = count;
  sem->valid = 1;
  return ERR_OK;
}

void sys_sem_signal(sys_sem_t *sem) {
  if ((NULL != sem) && (0 != sem->valid)) {
    sem->count++;
  }
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout) {
  u32_t start;
  u32_t elapsed;

  if ((NULL == sem) || (0 == sem->valid)) {
    return SYS_ARCH_TIMEOUT;
  }

  start = sys_now();

  do {
    if (sem->count > 0) {
      sem->count--;
      return sys_now() - start;
    }
    elapsed = sys_now() - start;
    if ((0 != timeout) && (elapsed >= timeout)) {
      return SYS_ARCH_TIMEOUT;
    }
    Lwip_PortPump();
  } while (elapsed < SYS_WAIT_MAX_MS);

  return SYS_ARCH_TIMEOUT;
}

void sys_sem_free(sys_sem_t *sem) {
  if (NULL != sem) {
    sem->valid = 0;
  }
}

int sys_sem_valid(sys_sem_t *sem) {
  return ((NULL != sem) && (0 != sem->valid)) ? 1 : 0;
}

void sys_sem_set_invalid(sys_sem_t *sem) {
  if (NULL != sem) {
    sem->valid = 0;
  }
}

/* ================================ [ MAILBOX   ] ============================================== */
err_t sys_mbox_new(sys_mbox_t *mbox, int size) {
  (void)size; /* the queue is always SYS_MBOX_SIZE deep, bigger than any lwIP default */

  if (NULL == mbox) {
    return ERR_ARG;
  }
  memset(mbox, 0, sizeof(sys_mbox_t));
  mbox->valid = 1;
  return ERR_OK;
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg) {
  if ((NULL == mbox) || (0 == mbox->valid)) {
    return ERR_ARG;
  }
  if (mbox->count >= SYS_MBOX_SIZE) {
    return ERR_MEM;
  }
  mbox->msg[mbox->write] = msg;
  mbox->write = (uint16_t)((mbox->write + 1) % SYS_MBOX_SIZE);
  mbox->count++;
  return ERR_OK;
}

err_t sys_mbox_trypost_fromisr(sys_mbox_t *mbox, void *msg) {
  return sys_mbox_trypost(mbox, msg);
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg) {
  while (ERR_OK != sys_mbox_trypost(mbox, msg)) {
    Lwip_PortPump();
  }
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout) {
  u32_t start;
  u32_t elapsed;

  if ((NULL == mbox) || (0 == mbox->valid)) {
    return SYS_ARCH_TIMEOUT;
  }

  start = sys_now();

  do {
    if (mbox->count > 0) {
      if (NULL != msg) {
        *msg = mbox->msg[mbox->read];
      }
      mbox->read = (uint16_t)((mbox->read + 1) % SYS_MBOX_SIZE);
      mbox->count--;
      return sys_now() - start;
    }
    elapsed = sys_now() - start;
    if ((0 != timeout) && (elapsed >= timeout)) {
      return SYS_ARCH_TIMEOUT;
    }
    Lwip_PortPump();
  } while (elapsed < SYS_WAIT_MAX_MS);

  return SYS_ARCH_TIMEOUT;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg) {
  if ((NULL == mbox) || (0 == mbox->valid) || (0 == mbox->count)) {
    return SYS_MBOX_EMPTY;
  }
  return sys_arch_mbox_fetch(mbox, msg, 1);
}

void sys_mbox_free(sys_mbox_t *mbox) {
  if (NULL != mbox) {
    mbox->valid = 0;
  }
}

int sys_mbox_valid(sys_mbox_t *mbox) {
  return ((NULL != mbox) && (0 != mbox->valid)) ? 1 : 0;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox) {
  if (NULL != mbox) {
    mbox->valid = 0;
  }
}

/* ================================ [ THREAD    ] ============================================== */
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize,
                            int prio) {
  (void)name;
  (void)thread;
  (void)arg;
  (void)stacksize;
  (void)prio;

  /* no thread is created: with LWIP_TCPIP_CORE_LOCKING the socket API is executed inline and
   * the input/timer processing is done by TcpIp_MainFunction. */
  return (sys_thread_t)1;
}

/* ================================ [ PROTECT   ] ============================================== */
/* SYS_LIGHTWEIGHT_PROT is 0, these are only here in case a component asks for them */
sys_prot_t sys_arch_protect(void) {
  return 0;
}

void sys_arch_unprotect(sys_prot_t pval) {
  (void)pval;
}
