/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2023 Parai Wang <parai@foxmail.com>
 */
#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
/* ================================ [ INCLUDES  ] ============================================== */
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void Irq_Init(void);
void Irq_Install(int irqno, void (*handler)(void), int oncpu);
void Irq_UnInstall(int irqno);

void DisableInterrupt(void);
void EnableInterrupt(void);
#endif /* __INTERRUPT_H__ */
