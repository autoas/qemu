/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */

/* ================================ [ INCLUDES  ] ============================================== */
#include "Mcu.h"
#include "Std_Timer.h"
#include "Std_Critical.h"
#include "Dcm.h"
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

Std_ReturnType __weak NvM_TestStart(const uint8_t *dataIn, Dcm_OpStatusType OpStatus,
                                    uint8_t *dataOut, uint16_t *currentDataLength,
                                    Dcm_NegativeResponseCodeType *ErrorCode) {
  return E_NOT_OK;
}

Std_ReturnType __weak NvM_TestGetResult(const uint8_t *dataIn, Dcm_OpStatusType OpStatus,
                                        uint8_t *dataOut, uint16_t *currentDataLength,
                                        Dcm_NegativeResponseCodeType *ErrorCode) {
  return E_NOT_OK;
}

Std_ReturnType __weak Dem_TestStart(const uint8_t *dataIn, Dcm_OpStatusType OpStatus, uint8_t *dataOut,
                             uint16_t *currentDataLength, Dcm_NegativeResponseCodeType *ErrorCode) {
  return E_NOT_OK;
}

void __weak Xcp_PerformReset(void) {
}

#ifdef USE_BL
extern void application_main(void);
Std_ReturnType BL_IsAppValid(void) {
  Std_ReturnType ret = E_NOT_OK;
  uint32_t u32Code = *(uint32_t *)application_main;
  if (u32Code == 0xe59f0068) {
    ret = E_OK;
  }
  return ret;
}
#endif