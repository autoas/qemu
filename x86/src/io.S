/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2023 Parai Wang <parai@foxmail.com>
 */

.section .text

/* void outb(uint32_t port, uint8_t value) */
.global outb
outb:
  mov 4(%esp), %dx    /* port */
  mov 8(%esp), %al    /* value */
  outb %al, %dx
  nop
  nop
  ret

/* char inb(int port) */
.global inb
inb:
  mov 4(%esp), %dx /* port */
  xor %eax, %eax
  inb %dx
  nop
  nop
  ret