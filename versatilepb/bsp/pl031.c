/* arm pl031
 * Marcos Medeiros
 */
#include "Std_Types.h"
#include "io.h"
#include "irq.h"

#define __iobase 0x101e8000
#define RTC_IRQ_NUM 10
#define RTCDR 0x00   /* data register */
#define RTCMR 0x04   /* match register */
#define RTCLR 0x08   /* load register */
#define RTCCR 0x0c   /* control register */
#define RTCIMSC 0x10 /* int mask  */
#define RTCRIS 0x14  /* raw int status */
#define RTCMIS 0x18  /* masked int status */
#define RTCICR 0x1c  /* int clear status */
#define RTC_ENABLE 0x01

static inline void pl031_enable_irq() {
  writel(__iobase + RTCIMSC, 1);
}

static inline void pl031_disable_irq() {
  writel(__iobase + RTCIMSC, 0);
}

static inline void pl031_clear_irq() {
  writel(__iobase + RTCICR, 1);
}

static inline void pl031_next_event(uint32_t sec) {
  register uint32_t next_tick = readl(__iobase + RTCDR) + sec;
  writel(__iobase + RTCMR, next_tick);
}

static int pl031_irq(void *ctx) {
  (void)ctx;

  pl031_clear_irq();
  pl031_next_event(1);
  return 0;
}

static int pl031_start() {
  writel(__iobase + RTCCR, RTC_ENABLE);
  pl031_enable_irq();
  pl031_next_event(1);
  return 0;
}

static int pl031_stop() {
  pl031_disable_irq();
  return 0;
}

int pl031_setup() {
  irq_install_isr(RTC_IRQ_NUM, pl031_irq);
  irq_enable_line(RTC_IRQ_NUM);

  return 0;
}
