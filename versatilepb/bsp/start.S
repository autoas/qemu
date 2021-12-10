/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2021 Parai Wang <parai@foxmail.com>
 */
.extern __bss_start
.extern __bss_end
.extern main

.macro DEFAULT_ISR_HANDLER name=
  .section .text
  .weak \name
\name:
  b \name /* endless loop */
.endm

	.section .init
	.globl _start
;;
_start:
    ldr     pc, reset_handler
    ldr     pc, _vector_undef
    ldr     pc, _vector_swi
    ldr     pc, _vector_pabt
    ldr     pc, _vector_dabt
    ldr     pc, _vector_resv
    ldr     pc, _vector_irq
    ldr     pc, _vector_fiq

reset_handler:  .word reset
_vector_undef:  .word vector_undef
_vector_swi:    .word vector_swi
_vector_pabt:   .word vector_pabt
_vector_dabt:   .word vector_dabt
_vector_resv:   .word vector_resv
_vector_irq:    .word vector_irq
_vector_fiq:    .word vector_fiq

.global reset
reset:
	;@	In the reset handler, we need to copy our interrupt vector table to 0x0000, its currently at 0x8000

	ldr r0,=_start								;@ Store the source pointer
    mov r1,#0x0000								;@ Store the destination pointer.

	;@	Here we copy the branching instructions
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}			;@ Load multiple values from indexed address. 		; Auto-increment R0
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}			;@ Store multiple values from the indexed address.	; Auto-increment R1

	;@	So the branches get the correct address we also need to copy our vector table!
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}			;@ Load from 4*n of regs (8) as R0 is now incremented.
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}			;@ Store this extra set of data.


	;@	Set up the various STACK pointers for different CPU modes
    ;@ (PSR_IRQ_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD2
    msr cpsr_c,r0
    mov sp,#0x8000

    ;@ (PSR_FIQ_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD1
    msr cpsr_c,r0
    mov sp,#0x6000

    ;@ (PSR_SVC_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD3
    msr cpsr_c,r0
	mov sp,#0x4000

    ;@ (PSR_SYS_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xDF
    msr cpsr_c,r0
	mov sp,#0x2000

	ldr r0, =__bss_start
	ldr r1, =__bss_end

	mov r2, #0

zero_loop:
	cmp 	r0,r1
	it		lt
	strlt	r2,[r0], #4
	blt		zero_loop

	bl 		Irq_Disable   ;@  to be modified

	;@ 	mov	sp,#0x1000000
	b main									;@ We're ready?? Lets start main execution!
	b .

DEFAULT_ISR_HANDLER vector_undef
DEFAULT_ISR_HANDLER vector_swi
DEFAULT_ISR_HANDLER vector_pabt
DEFAULT_ISR_HANDLER vector_dabt
DEFAULT_ISR_HANDLER vector_resv
DEFAULT_ISR_HANDLER vector_fiq

.extern Os_PortIsrHandler
.global vector_irq
.weak vector_irq
vector_irq:

    /* store caller-saved registers */
    stmfd sp!, {r0-r3,r9,r11,ip,lr}

    bl Os_PortIsrHandler

 	/* restore caller-saved registers */
    ldmfd sp!, {r0-r3,r9,r11,ip,lr}

    /* return to interrupted task */
    subs pc,lr,#4

	.section .text
    .global Std_EnterCritical
    .type   Std_EnterCritical, %function
/* void Std_EnterCritical( imask_t intsts ); */
Std_EnterCritical:
    msr cpsr, r0
    mov pc, lr

    .global Std_ExitCritical
    .type   Std_ExitCritical, %function
/* imask_t Std_ExitCritical( void ); */
Std_ExitCritical:
    mrs r0, cpsr
    orr r1, r0, #0xc0
    msr cpsr_c, r1
    mov pc, lr


/* void Irq_Disable(void)   */
    .global Irq_Disable
    .type   Irq_Disable, %function
Irq_Disable:
    mrs r0, cpsr
    orr r0, r0, #0xc0
    msr cpsr_c, r0
    mov pc, lr

/* void Irq_Enable(void) */
    .global Irq_Enable
    .type   Irq_Enable, %function
Irq_Enable:
    mrs r0, cpsr
    bic r0, r0, #0xc0
    msr cpsr_c, r0
    mov pc, lr

    .global loadsp
    .type   loadsp, %function
/* void loadsp( void* sp ); */
loadsp:
    mov sp, r0
    mov pc, lr

