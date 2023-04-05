/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2023 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "device.h"
#include "rtdef.h"
#include "Std_Debug.h"
#include <string.h>
/* ================================ [ MACROS    ] ============================================== */
#define AS_LOG_RTDEV 0
#define AS_LOG_RTDEVI 2
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
static rt_device_t lRtDevBlk0 = NULL;
static rt_device_t lRtDevNet0 = NULL;
/* ================================ [ LOCALS    ] ============================================== */
static int dev_asblk_open(const device_t *device) {
  int r = 0;
  rt_device_t dev = *(rt_device_t *)device->priv;

  ASLOG(RTDEV, ("open %s dev=%p\n", dev->parent.name, dev));
  if (NULL != dev->ops->open) {
    r = dev->ops->open(dev, RT_DEVICE_OFLAG_RDWR);
  }
  return r;
}

static int dev_asblk_close(const device_t *device) {
  int r = 0;
  rt_device_t dev = *(rt_device_t *)device->priv;

  ASLOG(RTDEV, ("close %s dev=%p\n", dev->parent.name, dev));
  if (NULL != dev->ops->close) {
    r = dev->ops->close(dev);
  }
  return r;
}

static int dev_asblk_read(const device_t *device, size_t pos, void *buffer, size_t size) {
  int r = 0;
  rt_ssize_t len;

  rt_device_t dev = *(rt_device_t *)device->priv;

  ASLOG(RTDEV,
        ("read %s dev=%p pos=%u size=%u\n", dev->parent.name, dev, (uint32_t)pos, (uint32_t)size));
  len = dev->ops->read(dev, pos, buffer, size);
  if (size != len) {
    r = -1;
  }
  ASHEXDUMP(RTDEV, ("read"), buffer, size * 512);
  return r;
}

static int dev_asblk_write(const device_t *device, size_t pos, const void *buffer, size_t size) {
  int r = 0;
  rt_ssize_t len;
  rt_device_t dev = *(rt_device_t *)device->priv;

  ASLOG(RTDEV,
        ("write %s dev=%p pos=%u size=%u\n", dev->parent.name, dev, (uint32_t)pos, (uint32_t)size));
  len = dev->ops->write(dev, pos, buffer, size);
  if (size != len) {
    r = -1;
  }
  return r;
}

static int dev_asblk_ctrl(const device_t *device, int cmd, void *args) {
  rt_device_t dev = *(rt_device_t *)device->priv;
  int ercd = 0;
  struct rt_device_blk_geometry geometry;

  ercd = dev->ops->control(dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);

  ASLOG(RTDEV, ("ctrl %s dev=%p: %d %d %d\n", dev->parent.name, dev, (int)geometry.bytes_per_sector,
                (int)geometry.block_size, (int)geometry.sector_count));
  asAssert(geometry.bytes_per_sector == 512);

  switch (cmd) {
  case DEVICE_CTRL_GET_SECTOR_SIZE:
    *(size_t *)args = geometry.bytes_per_sector;
    break;
  case DEVICE_CTRL_GET_BLOCK_SIZE:
    *(size_t *)args = geometry.block_size;
    break;
  case DEVICE_CTRL_GET_SECTOR_COUNT:
    *(size_t *)args = geometry.sector_count;
    break;
  case DEVICE_CTRL_GET_DISK_SIZE:
    *(size_t *)args = geometry.sector_count * geometry.bytes_per_sector;
    break;
  default:
    ercd = EINVAL;
    break;
  }

  return ercd;
}

static int dev_asnet_open(const device_t *device) {
  int r = 0;
  rt_device_t dev = *(rt_device_t *)device->priv;

  if (NULL != dev->ops->init) {
    r = dev->ops->init(dev);
  }

  return r;
}

static int dev_asnet_close(const device_t *device) {
  int r = 0;
  rt_device_t dev = *(rt_device_t *)device->priv;

  if (NULL != dev->ops->close) {
    r = dev->ops->close(dev);
  }

  return r;
}

static int dev_asnet_read(const device_t *device, size_t pos, void *buffer, size_t size) {
  int r = 0;
  rt_device_t dev = *(rt_device_t *)device->priv;

  r = dev->ops->read(dev, pos, buffer, size);

  return r;
}

static int dev_asnet_write(const device_t *device, size_t pos, const void *buffer, size_t size) {
  int r = 0;
  rt_device_t dev = *(rt_device_t *)device->priv;

  r = dev->ops->write(dev, pos, buffer, size);

  return r;
}

static int dev_asnet_ctrl(const device_t *device, int cmd, void *args) {
  rt_device_t dev = *(rt_device_t *)device->priv;
  int ercd = 0;

  switch (cmd) {
  case DEVICE_CTRL_GET_MAC_ADDR:
    ercd = dev->ops->control(dev, NIOCTL_GADDR, args);
    break;
  default:
    ercd = EINVAL;
    break;
  }

  return ercd;
}
/* ================================ [ FUNCTIONS ] ============================================== */
rt_err_t rt_device_register(rt_device_t dev, const char *name, rt_uint16_t flags) {
  rt_err_t r = RT_EOK;
  ASLOG(RTDEVI, ("register %s dev=%p flags = %X\n", name, dev, flags));
  if (0 == strcmp(name, "virtio-blk0")) {
    lRtDevBlk0 = dev;
  } else if (0 == strcmp(name, "virtio-net0")) {
    lRtDevNet0 = dev;
  } else {
    r = RT_ERROR;
  }
  return r;
}

DEVICE_REGISTER(sd0, BLOCK, asblk, &lRtDevBlk0);
DEVICE_REGISTER(eth0, NET, asnet, &lRtDevNet0);