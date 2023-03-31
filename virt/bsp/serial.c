/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */

/* ================================ [ INCLUDES  ] ============================================== */
#include "io.h"
#include "smp.h"
/* ============================ [ MACROS] ====================================================== */
/* ref qemu/hw/arm/virt.c */
#define UART0DR 0x09000000
#define UART0FR 0x09000018
#define UART0CR 0x09000030
#define UART0IMSC 0x09000038
#define UART0ICR 0x09000044

#define UART0_IRQNUM (32 + 1)

#define UARTIMSC_RXIM 0x10

/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
extern void Irq_Install(int irqno, void (*handler)(void), int oncpu);
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void uart_putc(char byte) {
  while (readl(UART0FR) & (1 << 5)) {
  }
  writel(UART0DR, byte);
}

int uart_getc(void) {
  if ((readl(UART0FR) & (1 << 6))) {
    return readl(UART0DR);
  }

  return -1;
}

void __putchar(char ch) {
  uart_putc(ch);
}

void uart_isr_handler(void) {
  int ch = uart_getc();
  if (ch != -1) {
#ifdef USE_SHELL
    void Shell_Input(uint8_t ch);
    if (ch == '\t') {
      ch = '\n';
    }
    Shell_Input(ch);
#endif
  }
}

void uart_init(void) {
  uint32_t u32v;
  /* enable Rx and Tx of UART */
  u32v = readl(UART0CR);
  writel(UART0CR, u32v | (1 << 0) | (1 << 8) | (1 << 9));
  /* enable rx irq */
  u32v = readl(UART0IMSC);
  writel(UART0IMSC, u32v | UARTIMSC_RXIM);
  Irq_Install(UART0_IRQNUM, uart_isr_handler, smp_processor_id());
}
