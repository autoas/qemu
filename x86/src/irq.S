/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2023 Parai Wang <parai@foxmail.com>
 * ref: http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html#s3
 *      http://infophysics.net/att0.pdf
 */
#define MACROS_ONLY
#include "x86.h"


.extern    RunningVar
.extern    exception_handler
.extern    spurious_irq
.extern    clock_handler

.extern    g_Tss
.extern    g_irq_table
.extern    sys_call_table
.extern    __bss_end__

#define hwint_master(name, irqno) \
  .align 2; \
  .global name; \
  .type name,@function; \
  name: \
    call    save; \
    /* disable current interrupt */ \
    inb    $INT_M_CTLMASK; \
    or     $(1 << irqno), %al; \
    outb   %al, $INT_M_CTLMASK; \
    mov    $EOI, %al; \
    outb   %al, $INT_M_CTL; \
    sti; /* enable interrupt */ \
    push    $irqno; \
    call    (g_irq_table + 4 * irqno); \
    pop    %ecx; \
    cli; \
    /* enable current interrupt */ \
    inb    $INT_M_CTLMASK; \
    and    $(~(1 << irqno)), %eax; \
    outb   %al, $INT_M_CTLMASK; \
    ret

#define hwint_slave(name, irqno) \
  .align 2; \
  .global name; \
  .type name,@function; \
  name: \
    call    save; \
    /* disable current interrupt */ \
    inb    $INT_S_CTLMASK; \
    or     $(1 << (irqno -8 )), %al; \
    outb   %al, $INT_S_CTLMASK; \
    mov    $EOI, %al; \
    outb   %al, $INT_S_CTL; \
    sti; /* enable interrupt */ \
    push    $irqno; \
    call    (g_irq_table + 4 * irqno); \
    pop    %ecx; \
    cli; \
    /* enable current interrupt */ \
    inb    $INT_S_CTLMASK; \
    and    $(~(1 << (irqno -8 ))), %al; \
    outb   %al, $INT_S_CTLMASK; \
    ret

#define hwexception(name, vector_no) \
  .align 2; \
  .global name; \
  .type name,@function; \
  name: \
    push $0xFFFFFFFF; \
    push $vector_no; \
    jmp exception

.section .text
hwint_master(hwint00, 0)
hwint_master(hwint01, 1)
hwint_master(hwint02, 2)
hwint_master(hwint03, 3)
hwint_master(hwint04, 4)
hwint_master(hwint05, 5)
hwint_master(hwint06, 6)
hwint_master(hwint07, 7)

hwint_slave(hwint08, 8)
hwint_slave(hwint09, 9)
hwint_slave(hwint10, 10)
hwint_slave(hwint11, 11)
hwint_slave(hwint12, 12)
hwint_slave(hwint13, 13)
hwint_slave(hwint14, 14)
hwint_slave(hwint15, 15)

save:
  pusha
  push  %ds
  push  %es
  push  %fs
  push  %gs
  mov  %ss, %dx
  mov  %dx, %ds
  mov  %dx, %es
  mov  %dx, %fs

  mov  %esp, %esi /* esi = Process Table Start Address */

  incl k_reenter(,1)
  cmp $0, k_reenter(,1)
  jne  .1
  mov  $(__bss_end__+4096), %esp
  push  restart
  jmp  RETADR(%esi)
.1:
  push  restart_reenter
  jmp  RETADR(%esi)

.align 2
.global restart
.type restart,@function
restart:
  mov  RunningVar(,1), %esp
  lldt  P_LDT_SEL(%esp)
  lea  P_STACKTOP(%esp), %eax 
  mov  %eax, 4+g_Tss(,1)
restart_reenter:
  decl k_reenter(,1)
  pop  %gs
  pop  %fs
  pop  %es
  pop  %ds
  popa
  add  $4, %esp
  iret

.align 2
.global sys_call
.type sys_call,@function
sys_call:
	ret

hwexception(divide_error, 0)
hwexception(single_step_exception, 1)
hwexception(nmi, 2)
hwexception(breakpoint_exception, 3)
hwexception(overflow, 4)
hwexception(bounds_check, 5)
hwexception(inval_opcode, 6)
hwexception(copr_not_available, 7)
hwexception(double_fault, 8)
hwexception(copr_seg_overrun, 9)
hwexception(inval_tss, 10)
hwexception(segment_not_present, 11)
hwexception(stack_exception, 12)
hwexception(general_protection, 13)
hwexception(page_fault, 14)
hwexception(copr_error, 16)

exception:
  call exception_handler
  add $(4*2), %esp  /* let the SP points to EIP, stack：EIP, CS, EFLAGS */
  hlt

.align 2
.global DisableInterrupt
.type DisableInterrupt,@function
DisableInterrupt:
		cli
		ret

.align 2
.global EnableInterrupt
.type EnableInterrupt,@function
EnableInterrupt:
		sti
		ret

#define INT_VECTOR_SYS_CALL  0x90
#define _NR_dispatch         0

.align 2
.global dispatch
.type dispatch,@function
/* void dispatch(int cmd, void* param) */
dispatch:
    mov $_NR_dispatch, %eax
    mov 4(%esp), %ebx
    mov 8(%esp), %ecx
    int $INT_VECTOR_SYS_CALL
    ret



