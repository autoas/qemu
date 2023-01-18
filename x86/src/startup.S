/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2023 Parai Wang <parai@foxmail.com>
 */

#define MACROS_ONLY
#include "mmu.h"

/* the magic number for the multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC      0x1BADB002

/* the flags for the multiboot header.  */
#define MULTIBOOT_HEADER_FLAGS      0x00000003

.section .init, "ax"

.extern main
.extern g_GdtInfo
.extern g_IdtInfo
.extern __bss_end__

/* the system entry */
.globl _start
_start:
    jmp multiboot_entry

    /* Align 32 bits boundary.  */
    .align  4

    /* multiboot header.  */
multiboot_header:
    /* magic */
    .long   MULTIBOOT_HEADER_MAGIC
    /* flags */
    .long   MULTIBOOT_HEADER_FLAGS
    /* checksum */
    .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

multiboot_entry:
    movl    $(__bss_end__+4096),%esp

    /* reset eflags.  */
    pushl   $0
    popf

    call cstart
    lgdt g_GdtInfo
    lidt g_IdtInfo

    ljmp $SELECTOR_KERNEL_CS, $csinit

csinit:
	xor	%eax, %eax
	mov	$SELECTOR_TSS, %ax
	ltr	%ax
    popl %eax
    call    main

    /* never get here */
spin:
    hlt
    jmp spin