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

/**
 *	Enables all IRQ's in the CPU's CPSR register.
 **/
static void irqEnable() {
  __asm volatile("mrs 	r0,cpsr");     // Read in the cpsr register.
  __asm volatile("bic		r0,r0,#0x80"); // Clear bit 8, (0x80) -- Causes IRQs to be enabled.
  __asm volatile("msr		cpsr_c, r0");  // Write it back to the CPSR register
}

static void irqDisable() {
  __asm volatile("mrs		r0,cpsr");     // Read in the cpsr register.
  __asm volatile("orr		r0,r0,#0x80"); // Set bit 8, (0x80) -- Causes IRQs to be disabled.
  __asm volatile("msr		cpsr_c, r0");  // Write it back to the CPSR register.
}

void tpl_enable_os_interrupts(void) {
  irqEnable();
}

void tpl_disable_os_interrupts(void) {
  irqDisable();
}

int EnableInterrupts() {
  irqEnable();
  return 0;
}

int DisableInterrupts() {
  irqDisable();
  return 0;
}

static unsigned long isrDisableCounter = 0;
imask_t __attribute__((weak)) __Irq_Save(void) {
  isrDisableCounter++;
  if (1u == isrDisableCounter) {
    irqDisable();
  }
  return 0;
}

void __attribute__((weak)) Irq_Restore(imask_t irq_state) {

  isrDisableCounter--;
  if (0u == isrDisableCounter) {
    irqEnable();
  }

  (void)irq_state;
}

void __attribute__((weak)) Irq_Enable(void) {
  irqEnable();
}
