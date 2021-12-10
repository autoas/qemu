/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
#ifndef _ARM_SMP_H_
#define _ARM_SMP_H_
/* ================================ [ INCLUDES  ] ============================================== */
/* ================================ [ MACROS    ] ============================================== */
#define dmb(opt) asm volatile("dmb " #opt : : : "memory")
#define smp_mb() dmb(ish)
/* ================================ [ TYPES     ] ============================================== */
typedef void (*secondary_entry_fn)(void);
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
int smp_processor_id(void);
void smp_boot_secondary(int cpu, secondary_entry_fn entry);
#endif /* _ARM_SMP_H_ */
