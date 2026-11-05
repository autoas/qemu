/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 *
 * lwIP sys_arch types for the bare metal ARM Versatile/PB target. There is no scheduler on
 * this target: the objects below only store their state, see sys_arch.c.
 */
#ifndef __ARCH_SYS_ARCH_H
#define __ARCH_SYS_ARCH_H
/* ================================ [ INCLUDES  ] ============================================== */
#include <stdint.h>
/* ================================ [ MACROS    ] ============================================== */
#define SYS_MBOX_SIZE 16
/* ================================ [ TYPES     ] ============================================== */
typedef struct {
  uint32_t count;
  uint8_t valid;
} sys_sem_t;

typedef struct {
  uint32_t count;
  uint8_t valid;
} sys_mutex_t;

typedef struct {
  void *msg[SYS_MBOX_SIZE];
  uint16_t read;
  uint16_t write;
  uint16_t count;
  uint8_t valid;
} sys_mbox_t;

typedef void *sys_thread_t;
typedef uint32_t sys_prot_t;
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
#endif /* __ARCH_SYS_ARCH_H */
