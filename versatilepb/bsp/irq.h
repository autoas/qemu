#ifndef __IRQ_H
#define __IRQ_H

typedef int (*isr_callback_t)(void *cpu);

struct irq_ctrl {
	int (*init)();
	int (*enable_line)(int num);
	int (*disable_line)(int num);
	int (*handler)(void*cpu);
	const char *name;
};

int __irq_call_isr(int num, void *cpu);

extern void irq_enable();
extern void irq_disable();
extern void fiq_enable();
extern void fiq_disable();

void irq_setup_ctrl(struct irq_ctrl *ctrl);
void irq_enable_line(int num);
void irq_disable_line(int num);
void irq_init();
int irq_install_isr(int num, isr_callback_t isr);
int irq_uninstall_isr(int num);

#endif
