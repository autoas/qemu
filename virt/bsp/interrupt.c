/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
/*
 * https://developer.arm.com/docs/100336/latest/programmers-model/distributor-registers-gicdgicda-summary
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"
#include "Std_Critical.h"
#include "Std_Debug.h"
#include "smp.h"
#include "io.h"
#include <rthw.h>
#include <rtthread.h>
#include "gicv2.h"
#include "gicv3.h"
/* ================================ [ MACROS    ] ============================================== */
#define isb() asm volatile("isb" : : : "memory")

/* DAIF, Interrupt Mask Bits */
#define DAIF_DBG_BIT (1 << 3) /* Debug mask bit */
#define DAIF_ABT_BIT (1 << 2) /* Asynchronous abort mask bit */
#define DAIF_IRQ_BIT (1 << 1) /* IRQ mask bit */
#define DAIF_FIQ_BIT (1 << 0) /* FIQ mask bit */

/*
 * Interrupt flags
 */

/*
 * GIC on QEMU Virt
 */
#define QEMU_VIRT_GIC_BASE (0x08000000)
#define GIC_BASE (QEMU_VIRT_GIC_BASE)
#define GIC_GICD_BASE (GIC_BASE)           /* GICD MMIO base address */
#define GIC_GICC_BASE (GIC_BASE + 0x10000) /* GICC MMIO base address */

#define GIC_GICD_SGIR (GIC_GICD_BASE + 0xf00) /* Software Generated Interrupt Register */

/* 8.9.7 GICD_ICFGR<n>, Interrupt Configuration Registers */
#define GIC_GICD_ICFGR_LEVEL (0x0) /* level-sensitive */
#define GIC_GICD_ICFGR_EDGE (0x2)  /* edge-triggered */

/* ================================ [ TYPES     ] ============================================== */
typedef int32_t irq_no; /* IRQ no */

void (*isr_pc[256])(void);
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
static void gicv3_write_sgi1r(uint64_t val) {
#if 0
	asm volatile("msr ICC_SGI1R_EL1, %0" : : "r" (val));
#else
  writel(GIC_GICD_SGIR, val);
#endif
}
/* ================================ [ FUNCTIONS ] ============================================== */

void EnableInterrupt(void) {
  __asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i"(DAIF_FIQ_BIT | DAIF_IRQ_BIT) : "memory");
}

void DisableInterrupt(void) {
  __asm__ __volatile__("msr DAIFSet, %0\n\t" : : "i"(DAIF_FIQ_BIT | DAIF_IRQ_BIT) : "memory");
}

imask_t Std_EnterCritical(void) {
  imask_t imask;

  __asm__ __volatile__("mrs %0, DAIF\n\t" : "=r"(imask) : : "memory");

  DisableInterrupt();

  return imask;
}
void Std_ExitCritical(imask_t imask) {
  __asm__ __volatile__("msr DAIF, %0\n\t" : : "r"(imask) : "memory");
}

void Irq_Init(void) {
  rt_uint64_t gic_cpu_base;
  rt_uint64_t gic_dist_base;
#ifdef BSP_USING_GICV3
  rt_uint64_t gic_rdist_base;
#endif
  rt_uint64_t gic_irq_start;

  /* initialize ARM GIC */
  gic_dist_base = platform_get_gic_dist_base();
  gic_cpu_base = platform_get_gic_cpu_base();
#ifdef BSP_USING_GICV3
  gic_rdist_base = platform_get_gic_redist_base();
#endif

  gic_irq_start = GIC_IRQ_START;

  arm_gic_dist_init(0, gic_dist_base, gic_irq_start);
  arm_gic_cpu_init(0, gic_cpu_base);
#ifdef BSP_USING_GICV3
  arm_gic_redist_init(0, gic_rdist_base);
#endif
  arm_gic_dump_type(0);
}

void Irq_Install(int irqno, void (*handler)(void), int oncpu) {
  isr_pc[irqno] = handler;
  arm_gic_set_configuration(0, irqno, GIC_GICD_ICFGR_EDGE);
  arm_gic_set_priority(0, irqno, 0);
  arm_gic_set_cpu(0, irqno, (1 << oncpu));
  arm_gic_clear_pending_irq(0, irqno);
  arm_gic_umask(0, irqno);
}

void Irq_UnInstall(int irqno) {
  arm_gic_clear_pending_irq(0, irqno);
  arm_gic_mask(0, irqno);
  isr_pc[irqno] = NULL;
}

void Os_PortIsrHandler(void) {
  irq_no irq;

  EnterCritical();
  irq = arm_gic_get_active_irq(0);
  arm_gic_mask(0, irq);
  arm_gic_ack(0, irq);
  if (isr_pc[irq] != NULL) {
    isr_pc[irq]();
    arm_gic_umask(0, irq);
  } else {
    ASLOG(ERROR, ("GIC: uninstalled IRQ %d on CPU%d\n", irq, smp_processor_id()));
  }
  ExitCritical();
}

void __attribute__((weak)) Os_PortException(long exception, void *sp, long esr) {
  if (5 == exception) {
    Os_PortIsrHandler();
  } else {
    ASLOG(INFO, ("Exception %d happened!\n", (int)exception));
  }
}

/* https://static.docs.arm.com/dai0492/a/GICv3_Software_Overview_Official_Release_A.pdf */
void Ipc_KickTo(int cpu, int irqno) {
  asAssert(irqno < 16);

  gicv3_write_sgi1r((1 << (cpu + 24)) | irqno);
  isb();
}
