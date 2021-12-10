/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
.extern __bss_start
.extern __bss_end
.extern main
.extern vector_table_el1
.extern stack_top
.extern stack2_top
.extern secondary_main
.extern Os_PortException

  .section .init

  .globl _start
_start:
  /* set up stack */
  mov x4, #1
  msr spsel, x4
  isb
  ldr x4, =stack_top
  mov sp, x4

  /* enable FP/ASIMD */
  mov x4, #(3 << 20)
  msr cpacr_el1, x4

  /* set up exception handling */
  ldr x4, =vector_table_el1
  msr vbar_el1, x4
  isb

  b main
  b .

  .section .text

  .weak secondary_main
secondary_main:
  b .

  .global secondary_start
secondary_start:
  /* set up stack */
  mov x4, #1
  msr spsel, x4
  isb
  ldr x4, =stack2_top
  mov sp, x4

  /* enable FP/ASIMD */
  mov x4, #(3 << 20)
  msr cpacr_el1, x4

  /* set up exception handling */
  ldr x4, =vector_table_el1
  msr vbar_el1, x4
  isb

  b secondary_main
  b .

  .global smp_processor_id
smp_processor_id:
  mrs  x0, mpidr_el1
  and  x0, x0, #3
  ret

/* for i in range(16): print("stp x%d, x%d, [sp, #-16]!"%(2*i, 2*i+1)) */
.macro SaveContext
	stp x0, x1, [sp, #-16]!
	stp x2, x3, [sp, #-16]!
	stp x4, x5, [sp, #-16]!
	stp x6, x7, [sp, #-16]!
	stp x8, x9, [sp, #-16]!
	stp x10, x11, [sp, #-16]!
	stp x12, x13, [sp, #-16]!
	stp x14, x15, [sp, #-16]!
	stp x16, x17, [sp, #-16]!
	stp x18, x19, [sp, #-16]!
	stp x20, x21, [sp, #-16]!
	stp x22, x23, [sp, #-16]!
	stp x24, x25, [sp, #-16]!
	stp x26, x27, [sp, #-16]!
	stp x28, x29, [sp, #-16]!
	str x30, [sp, #-8]!

	mrs x0, spsr_el1
	mrs x1, elr_el1
	stp x0, x1, [sp, #-16]!
.endm

/* for i in range(16): print("ldp x%d, x%d, [sp], #16"%(31-(2*i+1), 31-2*i)) */
.macro RestoreContext
	ldp x0, x1, [sp], #16
	msr spsr_el1, x0
	msr elr_el1, x1

	ldr x30, [sp], #8
	ldp x28, x29, [sp], #16
	ldp x26, x27, [sp], #16
	ldp x24, x25, [sp], #16
	ldp x22, x23, [sp], #16
	ldp x20, x21, [sp], #16
	ldp x18, x19, [sp], #16
	ldp x16, x17, [sp], #16
	ldp x14, x15, [sp], #16
	ldp x12, x13, [sp], #16
	ldp x10, x11, [sp], #16
	ldp x8, x9, [sp], #16
	ldp x6, x7, [sp], #16
	ldp x4, x5, [sp], #16
	ldp x2, x3, [sp], #16
	ldp x0, x1, [sp], #16
.endm

/*
 * Vectors
 * Adapted from arch/arm64/kernel/entry.S
 */
.macro vector_stub, name, vec
  .weak \name
\name:
	SaveContext

	mov	x0, \vec
	mov	x1, sp
	mrs	x2, esr_el1
	bl	Os_PortException

	RestoreContext
	eret
.endm

vector_stub el1t_sync,     0
vector_stub el1t_irq,      1
vector_stub el1t_fiq,      2
vector_stub el1t_error,    3

vector_stub el1h_sync,     4
vector_stub el1h_irq,      5
vector_stub el1h_fiq,      6
vector_stub el1h_error,    7

vector_stub el0_sync_64,   8
vector_stub el0_irq_64,    9
vector_stub el0_fiq_64,   10
vector_stub el0_error_64, 11

vector_stub el0_sync_32,  12
vector_stub el0_irq_32,   13
vector_stub el0_fiq_32,   14
vector_stub el0_error_32, 15

.balign 0x800
.global vector_table_el1
vector_table_el1:
curr_el_sp0_sync:
	b el1t_sync

.balign 0x80
curr_el_sp0_irq:
	b el1t_irq

.balign 0x80
curr_el_sp0_fiq:
	b el1t_fiq

.balign 0x80
curr_el_sp0_serror:
	b el1t_error

.balign 0x80
curr_el_spx_sync:
	b el1h_sync

.balign 0x80
curr_el_spx_irq:
	b el1h_irq

.balign 0x80
curr_el_spx_fiq:
	b el1h_fiq

.balign 0x80
curr_el_spx_serror:
	b el1h_error

.balign 0x80
lower_el_aarch64_sync:
	b el0_sync_64

.balign 0x80
lower_el_aarch64_irq:
	b el0_irq_64

.balign 0x80
lower_el_aarch64_fiq:
	b el0_fiq_64

.balign 0x80
lower_el_aarch64_serror:
	b el0_error_64

.balign 0x80
lower_el_aarch32_sync:
	b el0_sync_32

.balign 0x80
lower_el_aarch32_irq:
	b el0_irq_32

.balign 0x80
lower_el_aarch32_fiq:
	b el0_fiq_32

.balign 0x80
lower_el_aarch32_serror:
	b el0_error_32
