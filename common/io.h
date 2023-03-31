/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021-2023 Parai Wang <parai@foxmail.com>
 *
 */
#ifndef _IO_H
#define _IO_H
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h"
/* ================================ [ MACROS    ] ============================================== */
#define readl(reg) (*(uint32_t *)(reg))
#define writel(reg, v) (*(uint32_t *)(reg)) = (v)

#define readw(reg) (*(uint16_t *)(reg))
#define writew(reg, v) (*(uint16_t *)(reg)) = (v)

#define readb(reg) (*(uint8_t *)(reg))
#define writeb(reg, v) (*(uint8_t *)(reg)) = (v)
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
uint8_t inb(uint32_t p);
uint16_t inw(uint32_t p);
uint32_t inl(uint32_t p);

void outb(uint32_t p, uint8_t v);
void outw(uint32_t p, uint16_t v);
void outl(uint32_t p, uint32_t v);
#endif /* _IO_H */
