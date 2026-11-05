/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 */
/* SMSC 91C111 ethernet controller integrated on the ARM Versatile/PB board.
 * ref: http://infocenter.arm.com/help/topic/com.arm.doc.dui0224i/DUI0224I_realview_platform_baseboard_for_arm926ej_s_ug.pdf
 *      3.10 LAN ethernet interface
 */
#ifndef __SMC91C111_H
#define __SMC91C111_H
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"
/* ================================ [ MACROS    ] ============================================== */
#define SMC91C111_FRAME_MAX_SIZE 1514
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void smc91c111_init(void);

void smc91c111_get_mac(uint8_t *mac);

/* poll one received ethernet frame, return the frame length, 0 if no frame is available */
uint16_t smc91c111_recv(uint8_t *buf, uint16_t maxLen);

/* transmit one ethernet frame, FALSE if the controller is out of packet memory */
boolean smc91c111_send(const uint8_t *buf, uint16_t len);

#endif /* __SMC91C111_H */
