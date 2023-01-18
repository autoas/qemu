/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2017-2023 Parai Wang <parai@foxmail.com>
 *
 * ref: https://github.com/autoas/as/blob/master/com/as.infrastructure/arch/x86/
 */
#ifndef __MMU_H__
#define __MMU_H__
/* ================================ [ INCLUDES  ] ============================================== */
#ifndef MACROS_ONLY
#include "Std_Compiler.h"
#include <stdint.h>
#endif
/* ================================ [ MACROS    ] ============================================== */
/* GDT */
#define INDEX_DUMMY 0   // ┓
#define INDEX_FLAT_C 1  // ┣ LOADER 里面已经确定了的.
#define INDEX_FLAT_RW 2 // ┃
#define INDEX_VIDEO 3   // ┛
#define INDEX_TSS 4
#define INDEX_LDT_FIRST 5
/* 选择子 */
#define SELECTOR_DUMMY 0          // ┓
#define SELECTOR_FLAT_C 0x08      // ┣ LOADER 里面已经确定了的.
#define SELECTOR_FLAT_RW 0x10     // ┃
#define SELECTOR_VIDEO (0x18 + 3) // ┛<-- RPL=3
#define SELECTOR_TSS 0x20         // TSS. 从外层跳到内存时 SS 和 ESP 的值从里面获得.
#define SELECTOR_LDT_FIRST 0x28

#define SELECTOR_KERNEL_CS SELECTOR_FLAT_C
#define SELECTOR_KERNEL_DS SELECTOR_FLAT_RW
#define SELECTOR_KERNEL_GS SELECTOR_VIDEO

/* 描述符类型值说明 */
#define DA_32 0x4000       /* 32 位段				*/
#define DA_LIMIT_4K 0x8000 /* 段界限粒度为 4K 字节			*/
#define DA_DPL0 0x00       /* DPL = 0				*/
#define DA_DPL1 0x20       /* DPL = 1				*/
#define DA_DPL2 0x40       /* DPL = 2				*/
#define DA_DPL3 0x60       /* DPL = 3				*/
/* 存储段描述符类型值说明 */
#define DA_DR 0x90   /* 存在的只读数据段类型值		*/
#define DA_DRW 0x92  /* 存在的可读写数据段属性值		*/
#define DA_DRWA 0x93 /* 存在的已访问可读写数据段类型值	*/
#define DA_C 0x98    /* 存在的只执行代码段属性值		*/
#define DA_CR 0x9A   /* 存在的可执行可读代码段属性值		*/
#define DA_CCO 0x9C  /* 存在的只执行一致代码段属性值		*/
#define DA_CCOR 0x9E /* 存在的可执行可读一致代码段属性值	*/
/* 系统段描述符类型值说明 */
#define DA_LDT 0x82      /* 局部描述符表段类型值			*/
#define DA_TaskGate 0x85 /* 任务门类型值				*/
#define DA_386TSS 0x89   /* 可用 386 任务状态段类型值		*/
#define DA_386CGate 0x8C /* 386 调用门类型值			*/
#define DA_386IGate 0x8E /* 386 中断门类型值			*/
#define DA_386TGate 0x8F /* 386 陷阱门类型值			*/

/* 选择子类型值说明 */
/* 其中, SA_ : Selector Attribute */
#define SA_RPL_MASK 0xFFFC
#define SA_RPL0 0
#define SA_RPL1 1
#define SA_RPL2 2
#define SA_RPL3 3

#define SA_TI_MASK 0xFFFB
#define SA_TIG 0
#define SA_TIL 4

/* 中断向量 */
#define INT_VECTOR_DIVIDE 0x0
#define INT_VECTOR_DEBUG 0x1
#define INT_VECTOR_NMI 0x2
#define INT_VECTOR_BREAKPOINT 0x3
#define INT_VECTOR_OVERFLOW 0x4
#define INT_VECTOR_BOUNDS 0x5
#define INT_VECTOR_INVAL_OP 0x6
#define INT_VECTOR_COPROC_NOT 0x7
#define INT_VECTOR_DOUBLE_FAULT 0x8
#define INT_VECTOR_COPROC_SEG 0x9
#define INT_VECTOR_INVAL_TSS 0xA
#define INT_VECTOR_SEG_NOT 0xB
#define INT_VECTOR_STACK_FAULT 0xC
#define INT_VECTOR_PROTECTION 0xD
#define INT_VECTOR_PAGE_FAULT 0xE
#define INT_VECTOR_COPROC_ERR 0x10

/* 中断向量 */
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

/* 系统调用 */
#define INT_VECTOR_SYS_CALL 0x90

/* 线性地址 → 物理地址 */
#define vir2phys(seg_base, vir) (uint32_t)(((uint32_t)seg_base) + (uint32_t)(vir))

#define PROT_READ 0x1  /* Page can be read.  */
#define PROT_WRITE 0x2 /* Page can be written.  */
#define PROT_EXEC 0x4  /* Page can be executed.  */
#define PROT_NONE 0x0  /* Page can not be accessed.  */
/* ================================ [ TYPES     ] ============================================== */
#ifndef MACROS_ONLY
typedef struct {
  uint16_t limit_low;       /* Limit */
  uint16_t base_low;        /* Base */
  uint8_t base_mid;         /* Base */
  uint8_t attr1;            /* P(1) DPL(2) DT(1) TYPE(4) */
  uint8_t limit_high_attr2; /* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
  uint8_t base_high;        /* Base */
} __packed mmu_descriptor_t;

typedef struct {
  uint16_t size;
  mmu_descriptor_t *base;
} __packed mmu_descinfo_t;

typedef struct {
  uint16_t offset_low; /* Offset Low */
  uint16_t selector;   /* Selector */
  uint8_t dcount;
  uint8_t attr;         /* P(1) DPL(2) DT(1) TYPE(4) */
  uint16_t offset_high; /* Offset High */
} __packed mmu_gate_t;

typedef struct {
  uint16_t size;
  mmu_gate_t *base;
} __packed mmu_gateinfo_t;
#endif
/* ================================ [ DECLARES  ] ============================================== */
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
#endif /* __MMU_H__ */
