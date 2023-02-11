/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2017-2023 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "mmu.h"
#include "x86.h"
#include <stdio.h>
#include "Std_Critical.h"
#include "Std_Debug.h"
/* ================================ [ MACROS    ] ============================================== */
/* Hardware interrupts */
#define NR_IRQ 16 /* Number of IRQs */
#define CLOCK_IRQ 0
#define KEYBOARD_IRQ 1
#define CASCADE_IRQ 2   /* cascade enable for 2nd AT controller */
#define ETHER_IRQ 3     /* default ethernet interrupt vector */
#define SECONDARY_IRQ 3 /* RS232 interrupt vector for port 2 */
#define RS232_IRQ 4     /* RS232 interrupt vector for port 1 */
#define XT_WINI_IRQ 5   /* xt winchester */
#define FLOPPY_IRQ 6    /* floppy disk */
#define PRINTER_IRQ 7
#define AT_WINI_IRQ 14 /* at winchester */

/* 8253/8254 PIT (Programmable Interval Timer) */
#define TIMER0 0x40         /* I/O port for timer channel 0 */
#define TIMER_MODE 0x43     /* I/O port for timer mode control */
#define RATE_GENERATOR 0x34 /* 00-11-010-0 : Counter0 - LSB then MSB - rate generator - binary */
#define TIMER_FREQ 1193182L /* clock frequency for timer in PC and AT */
#define HZ 1000             /* clock freq (software settable on IBM-PC) */
/* ================================ [ TYPES     ] ============================================== */
typedef void (*t_pf_irq_handler)(int irq);
/* ================================ [ DECLARES  ] ============================================== */
extern void serial_isr(void);
extern void Os_PortSysTick(void);
/* ================================ [ DATAS     ] ============================================== */
t_pf_irq_handler g_irq_table[NR_IRQ];
/* ================================ [ LOCALS    ] ============================================== */
void spurious_irq(int irq) {
  printf("spurious_irq: %d\n", irq);
}
/* ================================ [ FUNCTIONS ] ============================================== */
void enable_irq(unsigned int irq) {
  uint8_t mask;
  if (irq < 8) {
    mask = inb(INT_M_CTLMASK);
    mask &= ~(1 << irq);
    outb(INT_M_CTLMASK, mask);
  } else {
    mask = inb(INT_S_CTLMASK);
    mask &= ~(1 << (irq - 8));
    outb(INT_S_CTLMASK, mask);
  }
}

void disable_irq(unsigned int irq) {
  uint8_t mask;
  if (irq < 8) {
    mask = inb(INT_M_CTLMASK);
    mask |= (1 << irq);
    outb(INT_M_CTLMASK, mask);
  } else {
    mask = inb(INT_S_CTLMASK);
    mask |= (1 << (irq - 8));
    outb(INT_S_CTLMASK, mask);
  }
}

imask_t Std_EnterCritical(void) {
  imask_t imask;

  __asm__ __volatile__("pushfl ; popl %0 ; cli" : "=g"(imask) : : "memory");

  return imask;
}

void Std_ExitCritical(imask_t imask) {
  __asm__ __volatile__("pushl %0 ; popfl" : : "g"(imask) : "memory", "cc");
}

void init_8259A(void) {
  outb(INT_M_CTL, 0x11); // Master 8259, ICW1.
  outb(INT_S_CTL, 0x11); // Slave  8259, ICW1.
  outb(INT_M_CTLMASK,
       INT_VECTOR_IRQ0); // Master 8259, ICW2. 设置 '主8259' 的中断入口地址为 0x20.
  outb(INT_S_CTLMASK,
       INT_VECTOR_IRQ8);    // Slave  8259, ICW2. 设置 '从8259' 的中断入口地址为 0x28
  outb(INT_M_CTLMASK, 0x4); // Master 8259, ICW3. IR2 对应 '从8259'.
  outb(INT_S_CTLMASK, 0x2); // Slave  8259, ICW3. 对应 '主8259' 的 IR2.
  outb(INT_M_CTLMASK, 0x1); // Master 8259, ICW4.
  outb(INT_S_CTLMASK, 0x1); // Slave  8259, ICW4.

  outb(INT_M_CTLMASK, 0xFF); // Master 8259, OCW1.
  outb(INT_S_CTLMASK, 0xFF); // Slave  8259, OCW1.

  int i;
  for (i = 0; i < NR_IRQ; i++) {
    g_irq_table[i] = spurious_irq;
  }
}

void put_irq_handler(int irq, t_pf_irq_handler handler) {
  disable_irq(irq);
  g_irq_table[irq] = handler;
}

void serial_enable_rx(void) {
  put_irq_handler(RS232_IRQ, (t_pf_irq_handler)serial_isr);
  enable_irq(RS232_IRQ);
}

void init_clock(void) {
  /* 初始化 8253 PIT */
  outb(TIMER_MODE, RATE_GENERATOR);
  outb(TIMER0, (uint8_t)(TIMER_FREQ / HZ));
  outb(TIMER0, (uint8_t)((TIMER_FREQ / HZ) >> 8));

  put_irq_handler(CLOCK_IRQ, (t_pf_irq_handler)Os_PortSysTick); /* 设定时钟中断处理程序 */
  enable_irq(CLOCK_IRQ); /* 让8259A可以接收时钟中断 */

  serial_enable_rx();
}

void handle_irq(int irqno) {
  if ((irqno < NR_IRQ) && (NULL != g_irq_table[irqno])) {
    g_irq_table[irqno](irqno);
  } else {
    asAssert(0);
  }
}