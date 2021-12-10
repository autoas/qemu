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
  ASLOG(INFO, ("BL is alive\n"));
}

void BL_JumpToApp(void) {
}

boolean BL_IsUpdateRequested(void) {
  boolean r = FALSE;
  uint32_t *magic = (uint32_t *)0x20000000;
  if (0x12345678 == *magic) {
    r = TRUE;
  }

  return r;
}

Std_ReturnType BL_GetProgramCondition(Dcm_ProgConditionsType **cond) {
  Std_ReturnType r = E_NOT_OK;
  uint32_t *magic = (uint32_t *)0x20000000;

  if (0x12345678 == *magic) {
    r = E_OK;
    *cond = (Dcm_ProgConditionsType *)0x20000004;
  }

  return r;
}