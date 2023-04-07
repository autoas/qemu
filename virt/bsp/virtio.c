/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2023 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include <string.h>
#include "Std_Debug.h"
#include "virt.h"
#include "virtio_blk.h"
#include "virtio_net.h"
#include "virtio_console.h"
/* ================================ [ MACROS    ] ============================================== */
#ifndef RT_USING_VIRTIO_VERSION
#define RT_USING_VIRTIO_VERSION 0x1
#endif

#define AS_LOG_VIRTIO 0
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void virt_vio_init(void) {
  int i;
  rt_uint32_t irq = VIRTIO_IRQ_BASE;
  rt_ubase_t mmio_base = VIRTIO_MMIO_BASE;
  struct virtio_mmio_config *mmio_config;

  for (i = 0; i < VIRTIO_MAX_NR; ++i, ++irq, mmio_base += VIRTIO_MMIO_SIZE) {
    mmio_config = (struct virtio_mmio_config *)mmio_base;

    if ((mmio_config->magic != VIRTIO_MAGIC_VALUE) ||
        (mmio_config->version != RT_USING_VIRTIO_VERSION) ||
        (mmio_config->vendor_id != VIRTIO_VENDOR_ID) ||
        (mmio_config->device_id == VIRTIO_DEVICE_ID_INVALID)) {
      continue;
    }

    ASLOG(VIRTIO,
          ("VIRTIO %d: magic=%x version=%x device_id=%x vendor_id=%x\n", i, mmio_config->magic,
           mmio_config->version, mmio_config->device_id, mmio_config->vendor_id));

    if (VIRTIO_DEVICE_ID_BLOCK == mmio_config->device_id) {
      rt_virtio_blk_init((rt_ubase_t *)mmio_base, irq);
    } else if (VIRTIO_DEVICE_ID_NET == mmio_config->device_id) {
      rt_virtio_net_init((rt_ubase_t *)mmio_base, irq);
    } else if (VIRTIO_DEVICE_ID_CONSOLE == mmio_config->device_id) {
    rt_virtio_console_init((rt_ubase_t *)mmio_base, irq);
    }
  }
}
