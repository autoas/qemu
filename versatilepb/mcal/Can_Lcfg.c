/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Can_Lcfg.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
extern void Can_Input(uint8_t Controller, uint8_t ch);
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
static int can0_irq(void *cpu) {
  (void)cpu;
  if (Uart_IsRxIsr(UART1)) {
    Can_Input(0, Uart_Read(UART1));
    Uart_RxIsrAck(UART1);
  }
  return 0;
}
static int can1_irq(void *cpu) {
  (void)cpu;
  if (Uart_IsRxIsr(UART2)) {
    Can_Input(1, Uart_Read(UART2));
    Uart_RxIsrAck(UART2);
  }
  return 0;
}
static int can2_irq(void *cpu) {
  (void)cpu;
  if (Uart_IsRxIsr(UART3)) {
    Can_Input(2, Uart_Read(UART3));
    Uart_RxIsrAck(UART3);
  }
  return 0;
}
/* ================================ [ FUNCTIONS ] ============================================== */
Can_ChannelConfigType Can_ChannelConfigs[] = {
  {
    UART1,
    IRQ_UART1_NUM,
    can0_irq,
  },
  {
    UART2,
    IRQ_UART2_NUM,
    can1_irq,
  },
  {
    UART3,
    IRQ_UART3_NUM,
    can2_irq,
  },
};

Can_ConfigType Can_Config = {
  Can_ChannelConfigs,
  ARRAY_SIZE(Can_ChannelConfigs),
};
