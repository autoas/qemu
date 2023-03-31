/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "smp.h"
#include "psci.h"
#include "spinlock.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
static spinlock_t lock;
/* ================================ [ FUNCTIONS ] ============================================== */
void smp_boot_secondary(int cpu, secondary_entry_fn entry) {
  spin_lock(&lock);
  psci_cpu_on((unsigned long)cpu, (unsigned long)entry);
  spin_unlock(&lock);
}
