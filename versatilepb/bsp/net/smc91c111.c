/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2026 Parai Wang <parai@foxmail.com>
 */
/* SMSC 91C111 ethernet controller integrated on the ARM Versatile/PB board.
 *
 * The controller is memory mapped at 0x10010000 (IRQ 25, not used as this driver is polled)
 * and offers a 16 byte register window, where 0x0E is the bank select register:
 *   bank 0: TCR/RCR
 *   bank 1: CONFIG/IA(MAC)/CTR
 *   bank 2: MMU command, packet number, TX/RX FIFO, data pointer and data port
 *   bank 3: revision
 * The packet memory is shared by 4 packets of 2048 bytes each, a frame is handed over by
 * writing a 4 bytes header (2 bytes status + 2 bytes size, size includes the header itself)
 * followed by the frame data.
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"
#include "Std_Debug.h"
#include "io.h"
#include "smc91c111.h"
/* ================================ [ MACROS    ] ============================================== */
#define AS_LOG_SMC91 0
#define AS_LOG_SMC91E 2

#define SMC91C111_BASE 0x10010000

/* register offsets, valid inside the selected bank */
#define SMC_REG_BSR 0x0E /* bank select */
#define SMC_REG_TCR_L 0x00
#define SMC_REG_TCR_H 0x01
#define SMC_REG_RCR_L 0x04
#define SMC_REG_RCR_H 0x05
#define SMC_REG_CR_L 0x00
#define SMC_REG_CR_H 0x01
#define SMC_REG_IA 0x04 /* MAC address, 6 bytes */
#define SMC_REG_CTR_L 0x0C
#define SMC_REG_CTR_H 0x0D
#define SMC_REG_MMU_CMD 0x00
#define SMC_REG_PNR 0x02
#define SMC_REG_AR 0x03
#define SMC_REG_TX_FIFO 0x04
#define SMC_REG_RX_FIFO 0x05
#define SMC_REG_PTR_L 0x06
#define SMC_REG_PTR_H 0x07
#define SMC_REG_DATA 0x08 /* 4 bytes data port */
#define SMC_REG_INT 0x0C
#define SMC_REG_IMASK 0x0D
#define SMC_REG_REV 0x0A

/* RX status word bits */
#define SMC_RX_STATUS_ODD 0x1000 /* odd sized frame */

/* RCR bits */
#define RCR_STRIP_CRC 0x0200
#define RCR_RXEN 0x0100
#define RCR_SOFT_RST 0x8000

/* TCR bits */
#define TCR_PAD_EN 0x0080
#define TCR_TXEN 0x0001

/* CTR bits */
#define CTR_AUTO_RELEASE 0x0800

/* MMU commands, value written is command << 5 */
#define MMU_CMD_ALLOCATE 0x20
#define MMU_CMD_RESET 0x40
#define MMU_CMD_REMOVE_RELEASE 0x80
#define MMU_CMD_ENQUEUE 0xC0

#define SMC_CTR_VALUE (0x1210 | CTR_AUTO_RELEASE)

/* pointer register bits */
#define PTR_AUTOINCR 0x4000
#define PTR_RCV 0x8000

#define SMC_PKT_NOT_READY 0x80

#define SMC_TX_TIMEOUT 100000
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
static const uint32_t lBase = SMC91C111_BASE;
static uint8_t lMac[6];
/* ================================ [ LOCALS    ] ============================================== */
static void smcBank(uint8_t bank) {
  writeb(lBase + SMC_REG_BSR, bank);
}

static uint8_t smcRead(uint32_t offset) {
  return readb(lBase + offset);
}

static void smcWrite(uint32_t offset, uint8_t value) {
  writeb(lBase + offset, value);
}

static void smcDataWrite(const uint8_t *data, uint32_t length) {
  uint32_t i;
  uint32_t w;

  for (i = 0; (i + 4) <= length; i += 4) {
    w = ((uint32_t)data[i]) | (((uint32_t)data[i + 1]) << 8) | (((uint32_t)data[i + 2]) << 16) |
        (((uint32_t)data[i + 3]) << 24);
    writel(lBase + SMC_REG_DATA, w);
  }
  for (; i < length; i++) {
    smcWrite(SMC_REG_DATA, data[i]);
  }
}

static void smcDataRead(uint8_t *data, uint32_t length) {
  uint32_t i;
  uint32_t w;

  for (i = 0; (i + 4) <= length; i += 4) {
    w = readl(lBase + SMC_REG_DATA);
    data[i] = (uint8_t)(w & 0xFF);
    data[i + 1] = (uint8_t)((w >> 8) & 0xFF);
    data[i + 2] = (uint8_t)((w >> 16) & 0xFF);
    data[i + 3] = (uint8_t)((w >> 24) & 0xFF);
  }
  for (; i < length; i++) {
    data[i] = smcRead(SMC_REG_DATA);
  }
}
/* ================================ [ FUNCTIONS ] ============================================== */
void smc91c111_init(void) {
  uint32_t i;

  smcBank(0);
  /* soft reset the controller */
  smcWrite(SMC_REG_RCR_H, (uint8_t)(RCR_SOFT_RST >> 8));
  /* reset the MMU and thus release all packet memory */
  smcBank(2);
  smcWrite(SMC_REG_MMU_CMD, MMU_CMD_RESET);
  smcWrite(SMC_REG_INT, 0xFF);
  smcWrite(SMC_REG_IMASK, 0x00); /* polled mode, no interrupt */

  /* read back the MAC address, QEMU fills it according to the -nic mac option */
  smcBank(1);
  for (i = 0; i < sizeof(lMac); i++) {
    lMac[i] = smcRead(SMC_REG_IA + i);
  }
  /* let the controller release the packet memory once a frame is sent */
  smcWrite(SMC_REG_CTR_L, (uint8_t)(SMC_CTR_VALUE & 0xFF));
  smcWrite(SMC_REG_CTR_H, (uint8_t)((SMC_CTR_VALUE >> 8) & 0xFF));

  smcBank(0);
  /* no CRC in the receive buffer as the IP stack does not care about it */
  smcWrite(SMC_REG_RCR_L, (uint8_t)((RCR_RXEN | RCR_STRIP_CRC) & 0xFF));
  smcWrite(SMC_REG_RCR_H, (uint8_t)((RCR_RXEN | RCR_STRIP_CRC) >> 8));
  /* pad short frames as ethernet requires a minimum frame size of 60 bytes */
  smcWrite(SMC_REG_TCR_L, (uint8_t)((TCR_TXEN | TCR_PAD_EN) & 0xFF));
  smcWrite(SMC_REG_TCR_H, (uint8_t)((TCR_TXEN | TCR_PAD_EN) >> 8));

  ASLOG(SMC91, ("MAC %02X:%02X:%02X:%02X:%02X:%02X, revision 0x%X\n", lMac[0], lMac[1], lMac[2],
                lMac[3], lMac[4], lMac[5], smcRead(SMC_REG_REV)));
}

void smc91c111_get_mac(uint8_t *mac) {
  uint32_t i;
  for (i = 0; i < sizeof(lMac); i++) {
    mac[i] = lMac[i];
  }
}

uint16_t smc91c111_recv(uint8_t *buf, uint16_t maxLen) {
  uint16_t pkt;
  uint32_t hdr;
  uint32_t length;

  smcBank(2);
  pkt = smcRead(SMC_REG_RX_FIFO);
  if (SMC_PKT_NOT_READY == pkt) {
    return 0; /* RX FIFO empty */
  }

  smcWrite(SMC_REG_PNR, (uint8_t)pkt);
  smcWrite(SMC_REG_PTR_L, 0x00);
  smcWrite(SMC_REG_PTR_H, (uint8_t)((PTR_RCV | PTR_AUTOINCR) >> 8));
  hdr = readl(lBase + SMC_REG_DATA);
  /* the size field covers the 4 bytes header plus the trailer: 2 bytes for an even sized
   * frame, only 1 byte for an odd sized frame - which is flagged by SMC_RX_STATUS_ODD */
  length = (hdr >> 16) & 0x07FF;
  if (0 != (hdr & SMC_RX_STATUS_ODD)) {
    length = (length > 5) ? (length - 5) : 0;
  } else {
    length = (length > 6) ? (length - 6) : 0;
  }

  if (length > maxLen) {
    ASLOG(SMC91E, ("drop frame as %d > %d\n", (int)length, (int)maxLen));
    length = 0;
  } else {
    smcDataRead(buf, length);
  }

  /* remove the frame from the RX FIFO and release its packet memory */
  smcWrite(SMC_REG_MMU_CMD, MMU_CMD_REMOVE_RELEASE);

  return (uint16_t)length;
}

boolean smc91c111_send(const uint8_t *buf, uint16_t len) {
  uint32_t pkt;
  uint32_t timeout = SMC_TX_TIMEOUT;
  uint32_t hdrLength = (uint32_t)len + 6;
  uint8_t hdr[4];

  if ((len > SMC91C111_FRAME_MAX_SIZE) || (0 == len)) {
    return FALSE;
  }

  smcBank(2);
  smcWrite(SMC_REG_MMU_CMD, MMU_CMD_ALLOCATE);
  pkt = smcRead(SMC_REG_AR);
  while ((SMC_PKT_NOT_READY == pkt) && (timeout > 0)) {
    pkt = smcRead(SMC_REG_AR);
    timeout--;
  }

  if (SMC_PKT_NOT_READY == pkt) {
    ASLOG(SMC91E, ("out of TX packet memory\n"));
    return FALSE;
  }

  smcWrite(SMC_REG_PNR, (uint8_t)pkt);
  smcWrite(SMC_REG_PTR_L, 0x00);
  smcWrite(SMC_REG_PTR_H, (uint8_t)(PTR_AUTOINCR >> 8));
  /* status word is filled by the controller itself */
  hdr[0] = 0x00;
  hdr[1] = 0x00;
  hdr[2] = (uint8_t)(hdrLength & 0xFF);
  hdr[3] = (uint8_t)((hdrLength >> 8) & 0xFF);
  smcDataWrite(hdr, sizeof(hdr));
  smcDataWrite(buf, len);
  /* clear the "last data byte"/control bytes behind the frame */
  hdr[0] = 0x00;
  hdr[1] = 0x00;
  smcDataWrite(hdr, 2);

  smcWrite(SMC_REG_MMU_CMD, MMU_CMD_ENQUEUE);

  return TRUE;
}
