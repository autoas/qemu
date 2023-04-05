/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 *
 * ref: Specification of CAN Driver AUTOSAR CP Release 4.4.0
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Can.h"
#include "Std_Debug.h"
#include "virt.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void Can_Init(const Can_ConfigType *Config) {
  virt_vio_init();
}

Std_ReturnType Can_SetControllerMode(uint8_t Controller, Can_ControllerStateType Transition) {
  Std_ReturnType ret = E_NOT_OK;
  return ret;
}

Std_ReturnType Can_Write(Can_HwHandleType Hth, const Can_PduType *PduInfo) {
  Std_ReturnType ret = E_OK;
  return ret;
}

void Can_DeInit(void) {
}

void Can_MainFunction_Write(void) {
}

void Can_MainFunction_Read(void) {
}
