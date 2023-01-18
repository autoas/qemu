/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2017-2023 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "mmu.h"
#include <stdio.h>
/* ================================ [ MACROS    ] ============================================== */
/* 8259A interrupt controller ports. */
#define INT_M_CTL 0x20     /* I/O port for interrupt controller         <Master> */
#define INT_M_CTLMASK 0x21 /* setting bits in this port disables ints   <Master> */
#define INT_S_CTL 0xA0     /* I/O port for second interrupt controller  <Slave>  */
#define INT_S_CTLMASK 0xA1 /* setting bits in this port disables ints   <Slave>  */

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
extern void enable_irq(unsigned int irq);
extern void disable_irq(unsigned int irq);
extern void serial_isr(void);
/* ================================ [ DATAS     ] ============================================== */
t_pf_irq_handler g_irq_table[NR_IRQ];
/* ================================ [ LOCALS    ] ============================================== */
void spurious_irq(int irq) {
  printf("spurious_irq: %d\n", irq);
}
/* ================================ [ FUNCTIONS ] ============================================== */
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