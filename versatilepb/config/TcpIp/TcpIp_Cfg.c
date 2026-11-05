/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 *
 * TcpIp platform configuration of the ARM Versatile/PB QEMU target.
 *
 * The TcpIp module itself is the unmodified infras/communication/TcpIp/TcpIp.c: it is built
 * against the lwIP socket API (--net=LWIP). What this target provides is the two functions of
 * TcpIp_Config:
 *   - InitFnc: board and stack bring up, that is the SMSC 91C111 driver, lwIP itself
 *     (tcpip_init() -> lwip_init() plus the core lock) and the netif of this board;
 *   - MainFnc: the "main loop" of the socket API. As lwIP runs with
 *     LWIP_TCPIP_CORE_LOCKING=1 and there is no OS on this target, no tcpip thread exists:
 *     the received frames are handed to the stack inline and the lwIP timers are served by
 *     sys_check_timeouts() right here.
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/ip4_addr.h"
#include "lwip/etharp.h"

#include "TcpIp_Priv.h"
#include "Std_Types.h"
#include "Std_Debug.h"

#include "Lwip_Port.h"
#include "ethernetif.h"
#include "smc91c111.h"
/* ================================ [ MACROS    ] ============================================== */
/* static IPv4 configuration, 10.0.2.15 is the default address of the QEMU user mode network */
#define AS_LOG_TCPIPI 1
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
static void TcpIp_VersatilepbInit(void);
static void TcpIp_VersatilepbMainFunction(void);
/* ================================ [ DATAS     ] ============================================== */
const struct TcpIp_Config_s TcpIp_Config = {
  TcpIp_VersatilepbInit,
  TcpIp_VersatilepbMainFunction,
};

static struct netif lNetif;
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
static void TcpIp_VersatilepbPump(void) {
  static boolean lInPump = FALSE;

  /* a blocking socket call pumps the stack from inside the API call, do not re-enter the
   * driver from there */
  if (FALSE == lInPump) {
    lInPump = TRUE;
    ethernetif_input(&lNetif);
    sys_check_timeouts();
    lInPump = FALSE;
  }
}

static void TcpIp_VersatilepbInit(void) {
  ip4_addr_t ipaddr;
  ip4_addr_t netmask;
  ip4_addr_t gw;

  IP4_ADDR(&ipaddr, 10, 0, 2, 15);
  IP4_ADDR(&netmask, 255, 255, 255, 0);
  IP4_ADDR(&gw, 10, 0, 2, 2);

  smc91c111_init();

  /* tcpip_init() = lwip_init() + the core lock of the socket API. The tcpip thread it asks
   * for is not started by sys_thread_new(), see config/LWIP/sys_arch.c */
  tcpip_init(NULL, NULL);

  netif_add(&lNetif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, netif_input);
  netif_set_default(&lNetif);
  netif_set_up(&lNetif);
  netif_set_link_up(&lNetif);

  /* The netif is configured after slirp has sent its first ARP request ("who has 10.0.2.15"),
   * so slirp does not know the MAC address of this station and drops the frames it has to
   * deliver into the guest ("Slirp: Failed to send packet"), which for example loses the very
   * first DoIP vehicle identification request. The gratuitous ARP reply teaches it the
   * address, and resolving the gateway (= the host inside slirp) right away avoids losing the
   * response while that ARP request is still pending. */
  ethernetif_announceMac(&lNetif);
  etharp_query(&lNetif, &gw, NULL);

  Lwip_PortSetPumpFnc(TcpIp_VersatilepbPump);

  ASLOG(TCPIPI, ("lwIP %s, IP %s\n", LWIP_VERSION_STRING, ip4addr_ntoa(&ipaddr)));
}

static void TcpIp_VersatilepbMainFunction(void) {
  TcpIp_VersatilepbPump();
}
