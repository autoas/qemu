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