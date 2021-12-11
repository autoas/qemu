/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
#ifndef CAN_LCFG_H
#define CAN_LCFG_H
/* ================================ [ INCLUDES  ] ============================================== */
#include "Can.h"
#include "../bsp/serial.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
typedef struct {
 uint32_t ioBase;
 uint32_t irqNo;
 isr_callback_t callback;
} Can_ChannelConfigType;

struct Can_Config_s {
  Can_ChannelConfigType *channelConfigs;
  uint8_t numOfChannels;
};
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */

#endif /* CAN_LCFG_H */