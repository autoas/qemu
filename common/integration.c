/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */

/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"
#include "Std_Debug.h"
#include "Dcm.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void BL_AliveIndicate(void) {
}

void BL_JumpToApp(void) {
}

boolean BL_IsUpdateRequested(void) {
  boolean r = FALSE;
  uint32_t *magic = (uint32_t *)BL_FLSDRV_MEMORY_LOW;
  if (0x12345678 == *magic) {
    r = TRUE;
  }

  return r;
}

Std_ReturnType BL_GetProgramCondition(Dcm_ProgConditionsType **cond) {
  Std_ReturnType r = E_NOT_OK;
  uint32_t *magic = (uint32_t *)BL_FLSDRV_MEMORY_LOW;

  if (0x12345678 == *magic) {
    r = E_OK;
    *cond = (Dcm_ProgConditionsType *)(BL_FLSDRV_MEMORY_LOW + 4);
  }

  return r;
}

void App_AliveIndicate(void) {
}

void User_Init(void) {
}

void User_MainTask10ms(void) {
}

void App_EnterProgramSession(void) {
}