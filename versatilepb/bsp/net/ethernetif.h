/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 */
#ifndef __ETHERNETIF_H
#define __ETHERNETIF_H
/* ================================ [ INCLUDES  ] ============================================== */
#include "lwip/netif.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
err_t ethernetif_init(struct netif *netif);

void ethernetif_input(struct netif *netif);

/* announce the MAC address of this station with a gratuitous ARP reply, needed by slirp to be
 * able to deliver frames into the guest */
void ethernetif_announceMac(struct netif *netif);
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
#endif /* __ETHERNETIF_H */
