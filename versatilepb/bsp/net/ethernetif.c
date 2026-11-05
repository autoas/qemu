/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 */
/* lwIP netif glue for the SMSC 91C111 controller of the ARM Versatile/PB board */
/* ================================ [ INCLUDES  ] ============================================== */
#include <string.h>

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "Std_Debug.h"
#include "ethernetif.h"
#include "smc91c111.h"
/* ================================ [ MACROS    ] ============================================== */
#define AS_LOG_ETHIF 0
#define AS_LOG_ETHIFE 2

#define ETHIF_FRAME_MAX_SIZE 1518

/* QEMU user mode network (slirp): packets that the host sends from its own loopback reach the
 * guest with source address 127.0.0.1, which slirp can not deliver back to the host again
 * ("Slirp: Failed to send packet"). Inside slirp the host is always reachable as 10.0.2.2, so
 * the address is translated on reception, exactly like slirp translates the other direction.
 * Without it a DoIP tester on the host would never get the vehicle identification response. */
#define ETHIF_HOST_LOOPBACK 127, 0, 0, 1
#define ETHIF_HOST_GATEWAY 10, 0, 2, 2
#define ETHIF_ETH_HDR_LEN 14
#define ETHIF_IPV4_HDR_MIN_LEN 20

/* gratuitous ARP reply: 14 bytes ethernet + 28 bytes ARP */
#define ETHIF_ARP_FRAME_SIZE 42
#define ETHIF_ARP_SHA 22
#define ETHIF_ARP_SPA 28
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
static uint8_t lRxBuffer[ETHIF_FRAME_MAX_SIZE];
static const uint8_t lHostLoopback[4] = {ETHIF_HOST_LOOPBACK};
static const uint8_t lHostGateway[4] = {ETHIF_HOST_GATEWAY};
/* ================================ [ LOCALS    ] ============================================== */
static uint16_t ethernetif_ipv4Checksum(const uint8_t *header, uint16_t length) {
  uint32_t sum = 0;
  uint16_t i;

  for (i = 0; (i + 1) < length; i += 2) {
    sum += (uint32_t)((header[i] << 8) | header[i + 1]);
  }

  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  return (uint16_t)(~sum);
}

/* translate a loopback source address of a received IPv4 frame to the slirp host gateway */
static void ethernetif_natHostLoopback(uint8_t *frame, uint16_t length) {  uint8_t *ip;
  uint16_t ipHeaderLen;
  uint16_t checksum;

  if (length < (ETHIF_ETH_HDR_LEN + ETHIF_IPV4_HDR_MIN_LEN)) {
    return;
  }

  if ((0x08 != frame[12]) || (0x00 != frame[13])) { /* not an IPv4 frame */
    return;
  }

  ip = &frame[ETHIF_ETH_HDR_LEN];
  if (0x45 != ip[0]) { /* only plain IPv4 without options */
    return;
  }

  if (0 != memcmp(&ip[12], lHostLoopback, sizeof(lHostLoopback))) {
    return;
  }

  ipHeaderLen = (uint16_t)((ip[0] & 0x0F) * 4);
  memcpy(&ip[12], lHostGateway, sizeof(lHostGateway));
  ip[10] = 0;
  ip[11] = 0;
  checksum = ethernetif_ipv4Checksum(ip, ipHeaderLen);
  ip[10] = (uint8_t)(checksum >> 8);
  ip[11] = (uint8_t)(checksum & 0xFF);
}

static err_t ethernetif_linkoutput(struct netif *netif, struct pbuf *p) {
  struct pbuf *q;
  uint32_t offset = 0;

  if (p->len == p->tot_len) {
    if (FALSE == smc91c111_send(p->payload, p->len)) {
      return ERR_IF;
    }
  } else {
    /* gather the pbuf chain into the flat frame buffer used for reception as well,
     * a frame is never received and transmitted at the same time (polled single thread) */
    for (q = p; NULL != q; q = q->next) {
      if ((offset + q->len) > sizeof(lRxBuffer)) {
        return ERR_BUF;
      }
      memcpy(&lRxBuffer[offset], q->payload, q->len);
      offset += q->len;
    }
    if (FALSE == smc91c111_send(lRxBuffer, (uint16_t)offset)) {
      return ERR_IF;
    }
  }

  return ERR_OK;
}

/* ================================ [ FUNCTIONS ] ============================================== */
static err_t ethernetif_igmpMacFilter(struct netif *netif, const ip4_addr_t *group,
                                      enum netif_mac_filter_action action) {
  (void)netif;
  (void)group;

  /* the 91C111 offers a 64 bit hash table for the multicast addresses, but the QEMU model
   * delivers every frame to the guest, so there is nothing to program here */
  return ((NETIF_ADD_MAC_FILTER == action) || (NETIF_DEL_MAC_FILTER == action)) ? ERR_OK
                                                                               : ERR_VAL;
}

err_t ethernetif_init(struct netif *netif) {
  netif->name[0] = 'e';
  netif->name[1] = 'n';
  netif->output = etharp_output;
  netif->linkoutput = ethernetif_linkoutput;
  netif->mtu = 1500;
  netif->hwaddr_len = ETH_HWADDR_LEN;
  smc91c111_get_mac(netif->hwaddr);
  /* NETIF_FLAG_IGMP is needed by DoIP vehicle announcement/discovery: lwIP only accepts a join
   * request for a netif that is flagged as IGMP capable and has a MAC filter installed */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP | NETIF_FLAG_LINK_UP;
  netif_set_igmp_mac_filter(netif, ethernetif_igmpMacFilter);
  ASLOG(ETHIF, ("MAC %02X:%02X:%02X:%02X:%02X:%02X\n", netif->hwaddr[0], netif->hwaddr[1],
                netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]));
  return ERR_OK;
}

/* Broadcast a gratuitous ARP reply ("10.0.2.15 is at my MAC"). QEMU's slirp asks for the MAC
 * of the guest as soon as the first frame has to be delivered into the guest, and it only
 * records the address of a station from an ARP *reply*: the ARP request of
 * etharp_gratuitous() is not enough, and at that early point of the start up the guest does
 * not answer yet, so the first frames of a tester would be dropped by slirp. */
void ethernetif_announceMac(struct netif *netif) {
  uint8_t frame[ETHIF_ARP_FRAME_SIZE];
  uint8_t *ip = &frame[ETHIF_ARP_SPA];

  memset(frame, 0, sizeof(frame));
  memset(&frame[0], 0xFF, ETH_HWADDR_LEN); /* broadcast */
  memcpy(&frame[6], netif->hwaddr, ETH_HWADDR_LEN); /* ethernet src */
  memcpy(&frame[ETHIF_ARP_SHA], netif->hwaddr, ETH_HWADDR_LEN);
  frame[12] = 0x08;
  frame[13] = 0x06; /* ARP */
  frame[14] = 0x00;
  frame[15] = 0x01; /* ethernet */
  frame[16] = 0x08;
  frame[17] = 0x00; /* IPv4 */
  frame[18] = ETH_HWADDR_LEN;
  frame[19] = 4;
  frame[20] = 0x00;
  frame[21] = 0x02; /* reply */
  memcpy(&ip[0], &netif->ip_addr.addr, 4); /* spa */
  memcpy(&ip[4], &netif->ip_addr.addr, 4); /* tpa */

  if (FALSE == smc91c111_send(frame, sizeof(frame))) {
    ASLOG(ETHIFE, ("failed to send the gratuitous ARP\n"));
  }
}

void ethernetif_input(struct netif *netif) {  struct pbuf *p;
  uint16_t length;

  while (0 != (length = smc91c111_recv(lRxBuffer, sizeof(lRxBuffer)))) {
    ethernetif_natHostLoopback(lRxBuffer, length);
    p = pbuf_alloc(PBUF_RAW, length, PBUF_POOL);
    if (NULL != p) {
      if (ERR_OK != pbuf_take(p, lRxBuffer, length)) {
        ASLOG(ETHIFE, ("failed to fill pbuf with %d bytes\n", (int)length));
        pbuf_free(p);
      } else if (ERR_OK != netif->input(p, netif)) {
        ASLOG(ETHIFE, ("failed to input %d bytes frame\n", (int)length));
        pbuf_free(p);
      }
    } else {
      ASLOG(ETHIFE, ("failed to allocate pbuf for %d bytes frame\n", (int)length));
    }
  }
}
