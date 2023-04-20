/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */

/* ================================ [ INCLUDES  ] ============================================== */
#include "Mcu.h"
#include "Std_Timer.h"
#include "interrupt.h"
#include "serial.h"
#include "timer.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
extern void application_main(void);
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void Mcu_Init(const Mcu_ConfigType *ConfigPtr) {
  DisableInterrupt();
  Irq_Init();
  uart_init();
#ifdef USE_LATE_MCU_INIT
  Os_PortStartSysTick();
  EnableInterrupt();
#endif
}

#ifdef USE_BL
Std_ReturnType BL_IsAppValid(void) {
  Std_ReturnType ret = E_NOT_OK;
  uint32_t u32Code = *(uint32_t *)application_main;
  if (u32Code == 0xD2800024) {
    ret = E_OK;
  }
  return ret;
}
#endif