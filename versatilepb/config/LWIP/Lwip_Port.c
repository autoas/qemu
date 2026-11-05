/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 *
 * lwIP port for a bare metal target: the services lwIP requires from the platform are the
 * system time, a random number generator, the packet pump used by the blocking sys_arch
 * waits and the IP routing hook for the QEMU user mode network.
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include <stdint.h>

#include "lwip/opt.h"
#include "lwip/sys.h"

#include "Lwip_Port.h"
#include "Std_Timer.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
static void (*lPumpFnc)(void) = NULL;
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
/* called by lwIP for the tcp/ip timers, in milli seconds */
uint32_t sys_now(void) {
  return (uint32_t)(Std_GetTime() / 1000);
}

/* used for the initial TCP sequence number, the seeding is good enough for an emulated
 * point to point link */
uint32_t lwip_rand(void) {
  static uint32_t state = 0x12345678ul;

  state = (state * 1103515245ul) + 12345ul;
  state ^= (uint32_t)Std_GetTime();

  return state;
}

void Lwip_PortSetPumpFnc(void (*fnc)(void)) {
  lPumpFnc = fnc;
}

/* QEMU user mode network: the host loopback address of a received frame is translated to the
 * slirp gateway by the netif, see ethernetif_natHostLoopback() in bsp/net/ethernetif.c */

void Lwip_PortPump(void) {
  if (NULL != lPumpFnc) {
    lPumpFnc();
  }
}

/* The bare metal build of this target links a small C library without errno, but the lwIP
 * socket API sets it on every failing call. */
int *__errno(void) {
  static int lErrno = 0;

  return &lErrno;
}
