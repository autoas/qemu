/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */

/* ================================ [ INCLUDES  ] ============================================== */
#include "Mcu.h"
#include "Std_Timer.h"
#include "Std_Critical.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
extern void timer_init(void (*cbk)(void));
extern void vic_setup(void);
extern void irq_init(void);
extern void serial_init(void);
extern void EnableInterrupt(void);
#ifdef USE_PCI
void virtio_net_init(void);
#endif
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void Mcu_Init(const Mcu_ConfigType *ConfigPtr) {
  vic_setup();
  irq_init();
  serial_init();
#ifdef USE_PCI
  virtio_net_init();
#endif
  timer_init(NULL);
  EnableInterrupt();
}
