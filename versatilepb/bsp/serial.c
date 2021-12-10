/* VersatilePB uart0 serial
 * Marcos Medeiros
 */
#include "Std_Types.h"
#include "io.h"
#include "irq.h"

#define __iobase 0x101f1000
#define IRQ_UART_NUM 12
#define UARTDR 0x00
#define UARTFR 0x18
#define UARTCR 0x30
#define UARTIMSC 0x38
#define UARTICR 0x44

static inline void serial_enable_rxe(uint32_t v) {
  uint32_t curr = readl(__iobase + UARTCR);
  if (v)
    writel(__iobase + UARTCR, curr | (1 << 9));
  else
    writel(__iobase + UARTCR, curr & ~(1 << 9));
}

static inline void serial_enable_txe(uint32_t v) {
  uint32_t curr = readl(__iobase + UARTCR);
  if (v)
    writel(__iobase + UARTCR, curr | (1 << 8));
  else
    writel(__iobase + UARTCR, curr & ~(1 << 8));
}

static inline void enable_txe_irq(uint32_t v) {
  uint32_t curr = readl(__iobase + UARTIMSC);
  if (v)
    writel(__iobase + UARTIMSC, curr | (1 << 5));
  else
    writel(__iobase + UARTIMSC, curr & ~(1 << 5));
}

static inline void enable_rxe_irq(uint32_t v) {
  uint32_t curr = readl(__iobase + UARTIMSC);
  if (v)
    writel(__iobase + UARTIMSC, curr | (1 << 4));
  else
    writel(__iobase + UARTIMSC, curr & ~(1 << 4));
}

static inline void clear_txe_irq() {
  writel(__iobase + UARTICR, (1 << 5));
}

static inline void clear_rxe_irq() {
  writel(__iobase + UARTICR, (1 << 6));
}

static inline void serial_wait() {
  do {
    (void)0;
  } while (readl(__iobase + UARTFR) & (1 << 5));
}

int uart_rxed() {
  return ((readl(__iobase + UARTFR) & (1 << 6)) != 0);
}

char uart_rxdata() {
  char ch = (readl(__iobase + UARTDR) & 0xFF);

  return ch;
}

static int serial_irq_handler(void *ctx) {
  char ch = uart_rxdata();
  (void)ctx;
  clear_rxe_irq();
  return 0;
}

void serial_init() {
  /* enable receive irq and disable transmit */
  enable_rxe_irq(1);
  enable_txe_irq(0);

  /* install interrupt service routine */
  irq_install_isr(IRQ_UART_NUM, serial_irq_handler);
  irq_enable_line(IRQ_UART_NUM);
}

void serial_send_char(uint32_t chr) {
  serial_wait();
  writeb(__iobase + UARTDR, chr & 0xff);
}

void __putchar(char ch) {
  serial_send_char(ch);
}
