/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 *
 * lwIP options for the ARM Versatile/PB QEMU target.
 *
 * The socket API is enabled (LWIP_SOCKET/LWIP_NETCONN) because the TcpIp module of infras is
 * reused as it is: it is written against <lwip/sockets.h>. There is no OS on this target, so
 * LWIP_TCPIP_CORE_LOCKING is used: every netconn/socket call is then executed inline in the
 * caller context instead of being handed over to the tcpip thread, and the stack is advanced
 * from TcpIp_MainFunction (netif input + sys_check_timeouts). The sys_arch of this port
 * (sys_arch.c) therefore only keeps the state of its objects, it never blocks a real thread.
 */
#ifndef __LWIPOPTS_H
#define __LWIPOPTS_H
/* ================================ [ INCLUDES  ] ============================================== */
#include <stdint.h>
/* ================================ [ MACROS    ] ============================================== */
#define NO_SYS 0
#define SYS_LIGHTWEIGHT_PROT 0
#define LWIP_TIMERS 1
#define LWIP_TIMERS_CUSTOM 0

/* socket API on top of netconn, but executed inline in the caller: see the header comment */
#define LWIP_SOCKET 1
#define LWIP_NETCONN 1
#define LWIP_NETIF_API 0
#define LWIP_TCPIP_CORE_LOCKING 1
#define LWIP_TCPIP_CORE_LOCKING_INPUT 1
#define LWIP_COMPAT_MUTEX 0

/* resources of the socket API: DoIP needs one UDP discovery socket, one TCP server socket and
 * up to max_connections accepted connections (3 in Network.json) */
#define MEMP_NUM_NETCONN 8
#define MEMP_NUM_NETBUF 8
#define MEMP_NUM_TCPIP_MSG_API 4
#define MEMP_NUM_TCPIP_MSG_INPKT 8
#define MEMP_NUM_SYS_TIMEOUT 12

/* sizes of the lwIP "thread" resources, no thread is actually created on this target */
#define TCPIP_THREAD_STACKSIZE 0
#define TCPIP_THREAD_PRIO 1
#define TCPIP_MBOX_SIZE 8
#define DEFAULT_THREAD_STACKSIZE 0
#define DEFAULT_THREAD_PRIO 1
#define DEFAULT_UDP_RECVMBOX_SIZE 8
#define DEFAULT_TCP_RECVMBOX_SIZE 8
#define DEFAULT_ACCEPTMBOX_SIZE 4

#define LWIP_IPV4 1
#define LWIP_IPV6 0
#define LWIP_ARP 1
#define LWIP_ETHERNET 1
#define LWIP_DHCP 0
#define LWIP_AUTOIP 0
#define LWIP_DNS 0
#define LWIP_ICMP 1
#define LWIP_RAW 0
#define LWIP_UDP 1
#define LWIP_TCP 1
#define LWIP_IGMP 1
#define LWIP_IPV6_MLD 0

#define LWIP_SINGLE_NETIF 1
#define LWIP_NETIF_HOSTNAME 0
#define LWIP_NETIF_STATUS_CALLBACK 0
#define LWIP_NETIF_LINK_CALLBACK 0
#define LWIP_NETIF_REMOVE_CALLBACK 0
#define LWIP_NETIF_HWADDRHINT 0

#define LWIP_TCP_TTL 64
#define LWIP_TCP_MSS 1460
#define TCP_MSS 1460
#define TCP_WND (4 * TCP_MSS)
#define TCP_SND_BUF (4 * TCP_MSS)
#define TCP_SND_QUEUELEN (4 * TCP_SND_BUF / TCP_MSS)
#define TCP_QUEUE_OOSEQ 0
#define LWIP_TCP_SACK_OUT 0
#define LWIP_TCP_KEEPALIVE 0
#define TCP_LISTEN_BACKLOG 1

#define MEM_LIBC_MALLOC 0
#define MEMP_MEM_MALLOC 0
#define MEM_ALIGNMENT 4
#define MEM_SIZE (24 * 1024)
#define MEMP_OVERFLOW_CHECK 0
#define MEMP_SANITY_CHECK 0

#define MEMP_NUM_PBUF 16
#define MEMP_NUM_UDP_PCB 4
#define MEMP_NUM_TCP_PCB 8
#define MEMP_NUM_TCP_PCB_LISTEN 2
#define MEMP_NUM_TCP_SEG 24
#define MEMP_NUM_IGMP_GROUP 4
#define MEMP_NUM_ARP_QUEUE 8
#define MEMP_NUM_REASSDATA 2
#define MEMP_NUM_FRAG_PBUF 8

#define PBUF_POOL_SIZE 16
#define PBUF_POOL_BUFSIZE 1524
#define LWIP_PBUF_REF_T u8_t

#define ARP_TABLE_SIZE 8
#define ARP_QUEUEING 1
#define ARP_MAXAGE 300

#define LWIP_STATS 0
#define LWIP_STATS_DISPLAY 0
#define MEM_STATS 0
#define MEMP_STATS 0
#define PBUF_STATS 0
#define SYS_STATS 0
#define MIB2_STATS 0

#define LWIP_DEBUG 0
#define LWIP_DBG_MIN_LEVEL 0

#define LWIP_CHKSUM_ALGORITHM 3
#define CHECKSUM_CHECK_IP 0
#define CHECKSUM_CHECK_UDP 0
#define CHECKSUM_CHECK_TCP 0
#define CHECKSUM_GEN_IP 1
#define CHECKSUM_GEN_UDP 1
#define CHECKSUM_GEN_TCP 1
#define CHECKSUM_GEN_ICMP 1

#define LWIP_CALLBACK_API 1
#define LWIP_PPP 0
#define PPP_SUPPORT 0
#define LWIP_HAVE_LOOPIF 0
#define LWIP_LOOPIF_MULTICAST 0
#define LWIP_NETIF_LOOPBACK 0
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 0
/* needed by TcpIp_Tell(): it asks ioctl(FIONREAD) for the number of received bytes, which lwIP
 * only implements when the receive buffer accounting (netconn->recv_avail) is enabled */
#define LWIP_SO_RCVBUF 1
#define LWIP_SO_LINGER 0
#define LWIP_NETIF_TX_SINGLE_PBUF 1
#define LWIP_SUPPORT_CUSTOM_PBUF 0
#define LWIP_MULTICAST_PING 1
#define LWIP_BROADCAST_PING 1
#define LWIP_NOASSERT 1
/* struct timeval comes from the C library, cc.h includes <sys/time.h> for it */
#define LWIP_TIMEVAL_PRIVATE 0
/* no stdlib rand() on the bare metal target, provided by config/LWIP/Lwip_Port.c */
uint32_t lwip_rand(void);
#define LWIP_RAND() ((u32_t)lwip_rand())
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
#endif /* __LWIPOPTS_H */
