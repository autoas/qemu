/*
 * IRQ handling for versatilepb
 * Marcos Medeiros
 */
#include "Std_Types.h"
#include "Std_Critical.h"
#include "io.h"
#include "irq.h"
#include <string.h>

isr_callback_t isr_handler_table[64] = {NULL};

struct irq_ctrl *__irqctrl = NULL;

void irq_setup_ctrl(struct irq_ctrl *ctrl) {
  __irqctrl = ctrl;
  __irqctrl->init(ctrl);
}

/* IRQ Handler ISR dispatcher
 * called from irq_handler_entry (start.s)
 */
void irq_handler(void *cpu) {
  if (__irqctrl) {
    if (__irqctrl->handler)
      __irqctrl->handler(cpu);
  }
}

int __irq_call_isr(int num, void *cpu) {
  if (num > 63) {
    return -1;
  }
  if (isr_handler_table[num])
    isr_handler_table[num](cpu);
  return 0;
}

void irq_init() {
  memset(isr_handler_table, 0, sizeof(isr_handler_table));
}

int irq_install_isr(int num, isr_callback_t isr) {
  if (num > 63)
    return -1;

  if (isr_handler_table[num])
    return -2;

  isr_handler_table[num] = isr;
  return 0;
}

int irq_uninstall_isr(int num) {
  if (num > 63)
    return -1;

  if (!isr_handler_table[num])
    return -2;

  isr_handler_table[num] = NULL;
  return 0;
}

void irq_enable_line(int num) {
  if (__irqctrl) {
    if (__irqctrl->enable_line)
      __irqctrl->enable_line(num);
  }
}

void irq_disable_line(int num) {
  if (__irqctrl) {
    if (__irqctrl->disable_line)
      __irqctrl->disable_line(num);
  }
}





