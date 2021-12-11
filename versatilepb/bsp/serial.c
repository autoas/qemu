/* VersatilePB uart0 serial
 * Marcos Medeiros
 */
#include "Std_Types.h"
#include "io.h"
#include "serial.h"
#include "OsekNm.h"
#include "CanNm.h"
#include "Std_Debug.h"

#define UARTDR 0x00
#define UARTFR 0x18
#define UARTCR 0x30
#define UARTIMSC 0x38
#define UARTICR 0x44

static inline void serial_enable_rxe(uint32_t __iobase, uint32_t v) {
  uint32_t curr = readl(__iobase + UARTCR);
  if (v)
    writel(__iobase + UARTCR, curr | (1 << 9));
  else
    writel(__iobase + UARTCR, curr & ~(1 << 9));
}

static inline void serial_enable_txe(uint32_t __iobase, uint32_t v) {
  uint32_t curr = readl(__iobase + UARTCR);
  if (v)
    writel(__iobase + UARTCR, curr | (1 << 8));
  else
    writel(__iobase + UARTCR, curr & ~(1 << 8));
}

static inline void enable_txe_irq(uint32_t __iobase, uint32_t v) {
  uint32_t curr = readl(__iobase + UARTIMSC);
  if (v)
    writel(__iobase + UARTIMSC, curr | (1 << 5));
  else
    writel(__iobase + UARTIMSC, curr & ~(1 << 5));
}

static inline void enable_rxe_irq(uint32_t __iobase, uint32_t v) {
  uint32_t curr = readl(__iobase + UARTIMSC);
  if (v)
    writel(__iobase + UARTIMSC, curr | (1 << 4));
  else
    writel(__iobase + UARTIMSC, curr & ~(1 << 4));
}

static inline void clear_txe_irq(uint32_t __iobase) {
  writel(__iobase + UARTICR, (1 << 5));
}

static inline void clear_rxe_irq(uint32_t __iobase) {
  writel(__iobase + UARTICR, (1 << 6));
}

static inline void serial_wait(uint32_t __iobase) {
  do {
    (void)0;
  } while (readl(__iobase + UARTFR) & (1 << 5));
}

int Uart_IsRxIsr(uint32_t __iobase) {
  return ((readl(__iobase + UARTFR) & (1 << 6)) != 0);
}

uint8_t Uart_Read(uint32_t __iobase) {
  uint8_t ch = (readl(__iobase + UARTDR) & 0xFF);

  return ch;
}

static int serial0_irq_handler(void *ctx) {
  uint8_t ch = Uart_Read(UART0);
  (void)ctx;
  (void)ch;
  clear_rxe_irq(UART0);

#ifdef USE_OSEKNM
  if (ch == 'x') {
    static int sleeped = TRUE;
    if (FALSE == sleeped) {
      ASLOG(INFO, ("OSEKNM goto sleep\n"));
      GotoMode(0, NM_BusSleep);
      sleeped = TRUE;
    } else {
      ASLOG(INFO, ("OSEKNM goto wakeup\n"));
      GotoMode(0, NM_Awake);
      sleeped = FALSE;
    }
  }
#endif

#ifdef USE_CANNM
  if (ch == 'x') {
    static int requested = FALSE;
    if (FALSE == requested) {
      ASLOG(INFO, ("CanNm request\n"));
      CanNm_NetworkRequest(0);
      requested = TRUE;
    } else {
      ASLOG(INFO, ("CanNm release\n"));
      CanNm_NetworkRelease(0);
      requested = FALSE;
    }
  }
#endif
  return 0;
}

void Uart_Init(uint32_t ioBase, uint32_t irqNo, isr_callback_t callback) {
  /* enable receive irq and disable transmit */
  enable_rxe_irq(ioBase, 1);
  enable_txe_irq(ioBase, 0);

  /* install interrupt service routine */
  irq_install_isr(irqNo, callback);
  irq_enable_line(irqNo);
}

void Uart_Send(uint32_t ioBase, uint8_t chr) {
  serial_wait(ioBase);
  writeb(ioBase + UARTDR, chr & 0xff);
}

void Uart_RxIsrAck(uint32_t ioBase) {
  clear_rxe_irq(ioBase);
}

void serial_init() {
  Uart_Init(UART0, IRQ_UART0_NUM, serial0_irq_handler);
}

void __putchar(char ch) {
  Uart_Send(UART0, ch);
}
