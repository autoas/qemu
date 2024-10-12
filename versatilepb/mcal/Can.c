/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 *
 * ref: Specification of CAN Driver AUTOSAR CP Release 4.4.0
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Can.h"
#include "CanIf_Can.h"
#include "Can_Lcfg.h"
#include "Std_Debug.h"
#include <string.h>
/* ================================ [ MACROS    ] ============================================== */
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
extern Can_ConfigType Can_Config;
/* ================================ [ DATAS     ] ============================================== */
static uint32_t lOpenFlag = 0;
static uint32_t lWriteFlag = 0;
static uint32_t lswPduHandle[32];
static struct can_frame lCanFrame[32];
static uint8_t lCanRxLen[32];
static Can_ControllerStateType lCanCtrlState[32];
static CanTrcv_TrcvModeType lCanTrcvMode[CAN_MAX_HOH];
/* ================================ [ LOCALS    ] ============================================== */

/* ================================ [ FUNCTIONS ] ============================================== */
void Can_Init(const Can_ConfigType *Config) {
  (void)Config;
  lOpenFlag = 0;
  lWriteFlag = 0;
  memset(lCanRxLen, 0, sizeof(lCanRxLen));
  memset(lCanCtrlState, 0, sizeof(lCanRxLen));
}

Std_ReturnType Can_SetControllerMode(uint8_t Controller, Can_ControllerStateType Transition) {
  Std_ReturnType ret = E_NOT_OK;
  Can_ChannelConfigType *config;

  if (Controller < CAN_CONFIG->numOfChannels) {
    config = &CAN_CONFIG->channelConfigs[Controller];
    switch (Transition) {
    case CAN_CS_STARTED:
      Uart_Init(config->ioBase, config->irqNo, config->callback);
      lOpenFlag |= (1 << Controller);
      lWriteFlag = 0;
      lCanCtrlState[Controller] = CAN_CS_STARTED;
      break;
    case CAN_CS_STOPPED:
    case CAN_CS_SLEEP:
      lOpenFlag &= ~(1 << Controller);
      lCanCtrlState[Controller] = Transition;
      break;
    default:
      break;
    }
  }

  return ret;
}

Std_ReturnType CanIf_SetTrcvMode(uint8_t TransceiverId, CanTrcv_TrcvModeType TransceiverMode) {
  Std_ReturnType ret = E_NOT_OK;

  if (TransceiverId < CAN_CONFIG->numOfChannels) {
    lCanTrcvMode[TransceiverId] = TransceiverMode;
    ret = E_OK;
  }

  return ret;
}

Std_ReturnType CanIf_GetTrcvMode(uint8_t TransceiverId, CanTrcv_TrcvModeType *TransceiverModePtr) {
  Std_ReturnType ret = E_NOT_OK;

  if (TransceiverId < CAN_CONFIG->numOfChannels) {
    *TransceiverModePtr = lCanTrcvMode[TransceiverId];
    ret = E_OK;
  }

  return ret;
}

Std_ReturnType Can_GetControllerMode(uint8_t Controller,
                                     Can_ControllerStateType *ControllerModePtr) {
  Std_ReturnType ret = E_NOT_OK;

  if (Controller < CAN_CONFIG->numOfChannels) {
    *ControllerModePtr = lCanCtrlState[Controller];
  }

  return ret;
}

Std_ReturnType Can_Write(Can_HwHandleType Hth, const Can_PduType *PduInfo) {
  Std_ReturnType ret = E_OK;
  struct can_frame frame;
  int i;

  if (lOpenFlag & (1 << Hth)) {
    if (0 == (lWriteFlag & (1 << Hth))) {
      memcpy(frame.data, PduInfo->sdu, PduInfo->length);
      mSetCANDLC(&frame, PduInfo->length);
      mSetCANID(&frame, PduInfo->id);
      for (i = 0; i < CAN_MTU; i++) {
        Uart_Send(CAN_CONFIG->channelConfigs[Hth].ioBase, frame.data[i]);
      }
      lWriteFlag |= (1 << Hth);
      lswPduHandle[Hth] = PduInfo->swPduHandle;
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
}
