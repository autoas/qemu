/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2017-2023 Parai Wang <parai@foxmail.com>
 */
#ifndef __X86_H__
#define __X86_H__
/* ================================ [ INCLUDES  ] ============================================== */
#ifndef MACROS_ONLY
#include <stdint.h>
#include "mmu.h"
#endif
/* ================================ [ MACROS    ] ============================================== */
#define PRIVILEGE_KRNL 0
#define PRIVILEGE_TASK 1
#define PRIVILEGE_USER 3

#define RPL_KRNL SA_RPL0
#define RPL_TASK SA_RPL1
#define RPL_USER SA_RPL3

/* 8259A interrupt controller ports. */
#define INT_M_CTL 0x20     /* I/O port for interrupt controller         <Master> */
#define INT_M_CTLMASK 0x21 /* setting bits in this port disables ints   <Master> */
#define INT_S_CTL 0xA0     /* I/O port for second interrupt controller  <Slave>  */
#define INT_S_CTLMASK 0xA1 /* setting bits in this port disables ints   <Slave>  */

#define EOI 0x20

#define P_STACKBASE 0
#define GSREG P_STACKBASE
#define FSREG 4         // (GSREG + 4)
#define ESREG 8         // (FSREG + 4)
#define DSREG 12        // (ESREG + 4)
#define EDIREG 16       // (DSREG + 4)
#define ESIREG 20       // (EDIREG + 4)
#define EBPREG 24       //(ESIREG + 4)
#define KERNELESPREG 28 // (EBPREG + 4)
#define EBXREG 32       //(KERNELESPREG + 4)
#define EDXREG 36       //(EBXREG + 4)
#define ECXREG 40       //(EDXREG + 4)
#define EAXREG 44       //(ECXREG + 4)
#define RETADR 48       //(EAXREG + 4)
#define EIPREG 52       //(RETADR + 4)
#define CSREG 56        //(EIPREG + 4)
#define EFLAGSREG 60    //(CSREG + 4)
#define ESPREG 64       // (EFLAGSREG + 4)
#define SSREG 68        // (ESPREG + 4)
#define P_STACKTOP 72   // (SSREG + 4)
#define P_LDT_SEL P_STACKTOP
#define P_LDT 76 // (P_LDT_SEL + 4)

#define TSS3_S_SP0 4
/* ================================ [ TYPES     ] ============================================== */
#ifndef MACROS_ONLY
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

typedef struct {
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t kernel_esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t retaddr;
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags; /* these are pushed by CPU during interrupt */
  uint32_t esp;
  uint32_t ss;
} cpu_context_t;
#endif
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
#endif /* __X86_H__ */
