/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2019-2023 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"
#include "Std_Debug.h"
#include "pci_core.h"
#include "io.h"
/* ================================ [ MACROS    ] ============================================== */
#define AS_LOG_PCI 1

#define VIRT_PCIE_BASE ((void *)0x3f000000)
/* https://github.com/qemu/qemu/blob/master/hw/arm/virt.c
 * https://github.com/torvalds/linux/blob/master/drivers/pci/ecam.c */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
static void *virt_map_bus(unsigned int busnr, unsigned int devfn, int offset) {

  return VIRT_PCIE_BASE + ((busnr << 16) | (devfn << 8) | offset);
}
/* ================================ [ FUNCTIONS ] ============================================== */
int pci_init(void) {
  int rv = 0;

  return rv;
}

int pci_generic_config_write(unsigned int busnr, unsigned int devfn, int where, int size,
                             uint32_t val) {
  void *addr;

  addr = virt_map_bus(busnr, devfn, where);
  if (!addr) {
    return -__LINE__;
  }
  if (size == 1)
    writeb(addr, val);
  else if (size == 2)
    writew(addr, val);
  else
    writel(addr, val);

  return 0;
}

int pci_generic_config_read(unsigned int busnr, unsigned int devfn, int where, int size,
                            uint32_t *val) {
  void *addr;

  addr = virt_map_bus(busnr, devfn, where & ~0x3);

  if (!addr) {
    *val = ~0;
    return -__LINE__;
  }

  *val = readl(addr);

  if (size <= 2)
    *val = (*val >> (8 * (where & 3))) & ((1 << (size * 8)) - 1);

  return 0;
}

/* Lunatic */
uint32 pci_read_config_reg32(pci_reg *reg, uint8 offset) {
  uint32 value;

  pci_generic_config_read(reg->bus, reg->fn, offset | (reg->dev << 11), 4, &value);

  return value;
}

uint16 pci_read_config_reg16(pci_reg *reg, uint8 offset) {
  uint32 value;

  pci_generic_config_read(reg->bus, reg->fn, offset | (reg->dev << 11), 2, &value);

  return (uint16)value;
}

uint8 pci_read_config_reg8(pci_reg *reg, uint8 offset) {
  uint32 value;

  pci_generic_config_read(reg->bus, reg->fn, offset | (reg->dev << 11), 1, &value);

  return (uint8)value;
}

void pci_write_config_reg32(pci_reg *reg, uint8 offset, const uint32 value) {
  pci_generic_config_write(reg->bus, reg->fn, offset | (reg->dev << 11), 4, value);
}

void pci_write_config_reg16(pci_reg *reg, uint8 offset, const uint16 value) {
  pci_generic_config_write(reg->bus, reg->fn, offset | (reg->dev << 11), 2, value);
}

void pci_write_config_reg8(pci_reg *reg, uint8 offset, const uint8 value) {
  pci_generic_config_write(reg->bus, reg->fn, offset | (reg->dev << 11), 1, value);
}

int pci_disable_IRQ_line(uint32 irq) {
  return 0;
}

int pci_enable_IRQ_line(uint32 irq) {
  return 0;
}

int pci_sys_set_irq_handle(uint32 irq, void (*handle)(void)) {
  return 0;
}

int pci_sys_irq_set_level_trigger(uint32 irq) {
  return 1;
}

int pci_sys_irq_set_edge_trigger(uint32 irq) {
  return 1;
}
