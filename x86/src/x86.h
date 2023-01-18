/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2017-2023 Parai Wang <parai@foxmail.com>
 */
#ifndef __X86_H__
#define __X86_H__
/* ================================ [ INCLUDES  ] ============================================== */
#include <stdint.h>
#include "mmu.h"
/* ================================ [ MACROS    ] ============================================== */

#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

#define	RPL_KRNL	SA_RPL0
#define	RPL_TASK	SA_RPL1
#define	RPL_USER	SA_RPL3
/* ================================ [ TYPES     ] ============================================== */
typedef struct {
  uint32_t backlink;
  uint32_t tesp0; /* stack pointer to use during interrupt */
  uint32_t ss0;   /* segment */
  uint32_t esp1;
  uint32_t ss1;
  uint32_t esp2;
  uint32_t ss2;
  uint32_t cr3;
  uint32_t eip;
  uint32_t flags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;
  uint32_t es;
  uint32_t cs;
  uint32_t ss;
  uint32_t ds;
  uint32_t fs;
  uint32_t gs;
  uint32_t ldt;
  uint16_t trap;
  uint16_t iobase; /* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
                   /*t_8iomap[2];*/
} tss_t;
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
#endif /* __X86_H__ */
