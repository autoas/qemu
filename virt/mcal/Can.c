/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 *
 * ref: Specification of CAN Driver AUTOSAR CP Release 4.4.0
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Can.h"
#include "CanIf.h"
#include "Std_Debug.h"
#include "virt.h"
#include "device.h"
#include "Std_Timer.h"
#include "virtio_console.h"
/* ================================ [ MACROS    ] ============================================== */
#define CAN_USE_ISR_RX

/* this simulation just alow only one HTH/HRH for each CAN controller */
#define CAN_MAX_HOH 32

#define CAN_CONFIG (&Can_Config)

#define CAN_MAX_DLEN 64 /* 64 for CANFD */
#define CAN_MTU (CAN_MAX_DLEN + 5)

#define mCANID(frame)                                                                              \
  (((uint32_t)(frame)->data[CAN_MAX_DLEN + 0] << 24) +                                             \
   ((uint32_t)(frame)->data[CAN_MAX_DLEN + 1] << 16) +                                             \
   ((uint32_t)(frame)->data[CAN_MAX_DLEN + 2] << 8) + ((uint32_t)(frame)->data[CAN_MAX_DLEN + 3]))

#define mSetCANID(frame, canid)                                                                    \
  do {                                                                                             \
    (frame)->data[CAN_MAX_DLEN + 0] = (uint8_t)(canid >> 24);                                      \
    (frame)->data[CAN_MAX_DLEN + 1] = (uint8_t)(canid >> 16);                                      \
    (frame)->data[CAN_MAX_DLEN + 2] = (uint8_t)(canid >> 8);                                       \
    (frame)->data[CAN_MAX_DLEN + 3] = (uint8_t)(canid);                                            \
  } while (0)

#define mCANDLC(frame) ((uint8_t)(frame)->data[CAN_MAX_DLEN + 4])
#define mSetCANDLC(frame, dlc)                                                                     \
  do {                                                                                             \
    (frame)->data[CAN_MAX_DLEN + 4] = dlc;                                                         \
  } while (0)
/* ================================ [ TYPES     ] ============================================== */
struct can_frame {
  uint8_t data[CAN_MTU];
};
/* ================================ [ DECLARES  ] ============================================== */
extern const device_t dev_can0;

void Can_Input(uint8_t Controller, uint8_t ch);
/* ================================ [ DATAS     ] ============================================== */
static uint32_t lOpenFlag = 0;
static uint32_t lWriteFlag = 0;
static uint32_t lswPduHandle[32];
static struct can_frame lCanFrame[32];
static uint8_t lCanRxLen[32];
/* ================================ [ LOCALS    ] ============================================== */
#ifdef CAN_USE_ISR_RX
static rt_err_t can_rx_indicate(rt_device_t dev, rt_size_t size) {
  uint8_t buffer[128];
  int i;
  int len = dev->ops->read(dev, 0, buffer, sizeof(buffer));
  asAssert(size < sizeof(buffer));
  if (len > 0) {
    for (i = 0; i < len; i++) {
      Can_Input(0, buffer[i]);
    }
  }
  return RT_EOK;
}
#endif
/* ================================ [ FUNCTIONS ] ============================================== */
void Can_Init(const Can_ConfigType *Config) {
  virt_vio_init();
  lOpenFlag = 0;
  lWriteFlag = 0;
  memset(lCanRxLen, 0, sizeof(lCanRxLen));
}

Std_ReturnType Can_SetControllerMode(uint8_t Controller, Can_ControllerStateType Transition) {
  Std_ReturnType ret = E_NOT_OK;
  int ercd;
  if (0 == Controller) {
    switch (Transition) {
    case CAN_CS_STARTED:
      ercd = dev_can0.ops.open(&dev_can0);
      if (0 == ercd) {
#ifdef CAN_USE_ISR_RX
        dev_can0.ops.ctrl(&dev_can0, RT_DEVICE_CTRL_SET_INT, can_rx_indicate);
#endif
        lOpenFlag |= (1 << Controller);
        lWriteFlag = 0;
      } else {
        ASLOG(ERROR, ("CAN virtion can0 open failed: %d\n", ercd));
      }
      break;
    case CAN_CS_STOPPED:
    case CAN_CS_SLEEP:
      ercd = dev_can0.ops.close(&dev_can0);
      if (0 == ercd) {
        lOpenFlag &= ~(1 << Controller);
      }
      break;
    default:
      break;
    }
  }
  return ret;
}

Std_ReturnType Can_Write(Can_HwHandleType Hth, const Can_PduType *PduInfo) {
  Std_ReturnType ret = E_OK;
  struct can_frame frame;
  int ercd;

  if (lOpenFlag & (1 << Hth)) {
    if (0 == (lWriteFlag & (1 << Hth))) {
      memcpy(frame.data, PduInfo->sdu, PduInfo->length);
      mSetCANDLC(&frame, PduInfo->length);
      mSetCANID(&frame, PduInfo->id);
      ercd = dev_can0.ops.write(&dev_can0, 0, &frame, sizeof(frame));
      if (ercd == sizeof(frame)) {
        lWriteFlag |= (1 << Hth);
        lswPduHandle[Hth] = PduInfo->swPduHandle;
      } else {
        ret = E_NOT_OK;
        ASLOG(ERROR, ("CAN virtion can0 write failed: %d\n", ercd));
      }
    } else {
      ret = CAN_BUSY;
    }
  }

  return ret;
}

void Can_DeInit(void) {
}

void Can_MainFunction_Write(void) {
  int i;
  PduIdType swPduHandle;

  for (i = 0; i < CAN_MAX_HOH; i++) {
    if (lWriteFlag & (1 << i)) {
      swPduHandle = lswPduHandle[i];
      lWriteFlag &= ~(1 << i);
      CanIf_TxConfirmation(swPduHandle);
    }
  }
}

void Can_Input(uint8_t Controller, uint8_t ch) {
  Can_HwType Mailbox;
  PduInfoType PduInfo;
  if (lCanRxLen[Controller] < CAN_MTU) {
    lCanFrame[Controller].data[lCanRxLen[Controller]++] = ch;
    if (lCanRxLen[Controller] >= CAN_MTU) {
      lCanRxLen[Controller] = 0;
      Mailbox.CanId = mCANID(&lCanFrame[Controller]);
      Mailbox.ControllerId = Controller;
      Mailbox.Hoh = Controller;
      PduInfo.SduLength = mCANDLC(&lCanFrame[Controller]);
      PduInfo.SduDataPtr = lCanFrame[Controller].data;
      PduInfo.MetaDataPtr = NULL;
      CanIf_RxIndication(&Mailbox, &PduInfo);
    }
  } else {
    ASLOG(ERROR, ("CAN protocol broken\n"));
  }
}

void Can_MainFunction_Read(void) {
#ifndef CAN_USE_ISR_RX
  uint8_t buffer[128];
  int i;
  int len = dev_can0.ops.read(&dev_can0, 0, buffer, sizeof(buffer));
  if (len > 0) {
    for (i = 0; i < len; i++) {
      Can_Input(0, buffer[i]);
    }
  }
#endif
}
