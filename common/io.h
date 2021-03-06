/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 *
 */
#ifndef _IO_H
#define _IO_H
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Types.h" 
/* ================================ [ MACROS    ] ============================================== */
#define readl(reg) (*(uint32_t*)(reg))
#define writel(reg, v) (*(uint32_t*)(reg)) = (v)

#define readb(reg) (*(uint8_t*)(reg))
#define writeb(reg, v) (*(uint8_t*)(reg)) = (v)
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
#endif /* _IO_H */
