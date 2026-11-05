/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 *
 * Interface between the lwIP port (config/LWIP) and the TcpIp platform configuration
 * (config/TcpIp) of the ARM Versatile/PB target.
 */
#ifndef __LWIP_PORT_H
#define __LWIP_PORT_H
/* ================================ [ INCLUDES  ] ============================================== */
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
/* Advance the stack once: poll the ethernet driver, hand received frames to lwIP and serve the
 * lwIP timers. Called by TcpIp_MainFunction() and by the blocking sys_arch waits. */
void Lwip_PortPump(void);
/* install the function used by Lwip_PortPump(), done by the TcpIp platform configuration */
void Lwip_PortSetPumpFnc(void (*fnc)(void));
#endif /* __LWIP_PORT_H */
