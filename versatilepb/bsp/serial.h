#ifndef __SERIAL_H
#define __SERIAL_H
#include "Std_Types.h"
#include "irq.h"

#define UART0 0x101f1000
#define UART1 0x101f2000
#define UART2 0x101f3000
#define UART3 0x10009000

#define IRQ_UART0_NUM 12
#define IRQ_UART1_NUM 13
#define IRQ_UART2_NUM 14
#define IRQ_UART3_NUM 6

void Uart_Init(uint32_t ioBase, uint32_t irqNo, isr_callback_t callback);
void Uart_Send(uint32_t ioBase, uint8_t chr);
int Uart_IsRxIsr(uint32_t ioBase);
uint8_t Uart_Read(uint32_t ioBase);
void Uart_RxIsrAck(uint32_t ioBase);
void serial_init();

#endif
