/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */

/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"
#include "Std_Debug.h"
#include "Std_Critical.h"
#include "Dcm.h"
#ifdef __AARCH64__
#include "psci.h"
#endif
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
extern void application_main(void);
extern void reset_main(void);
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void BL_AliveIndicate(void) {
}

void BL_JumpToApp(void) {
#ifdef USE_BL
  application_main();
#endif
}
#ifdef BL_FLSDRV_MEMORY_LOW
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
#endif

void App_AliveIndicate(void) {
}

void User_Init(void) {
}

void User_MainTask10ms(void) {
}

void App_EnterProgramSession(void) {
#ifdef BL_FLSDRV_MEMORY_LOW
  uint32_t *magic = (uint32_t *)BL_FLSDRV_MEMORY_LOW;
  Dcm_ProgConditionsType *cond = (Dcm_ProgConditionsType *)(BL_FLSDRV_MEMORY_LOW + 4);
  EnterCritical();
  *magic = 0x12345678;
  cond->ConnectionId = 0;
  cond->TesterAddress = 0;
  cond->Sid = 0x10;
  cond->SubFncId = 0x02;
  cond->Reprograming = TRUE;
  cond->ApplUpdated = TRUE;
  cond->ResponseRequired = TRUE;
  reset_main();
  ExitCritical();
#endif
}

void Dcm_PerformReset(uint8_t resetType) {
  EnterCritical();
#ifdef __AARCH64__
  psci_sys_reset();
#else
  reset_main();
#endif
  ExitCritical();
}

void InitBssSection(uint32_t *pSrc, uint32_t *pEnd) {
  while (pSrc < pEnd) {
    *pSrc = 0;
    pSrc++;
  }
}

void InitDataSection(uint32_t *pSrc, uint32_t *pDst, uint32_t *pEnd) {
  while (pDst < pEnd) {
    *pDst = *pSrc;
    pDst++;
    pSrc++;
  }
}