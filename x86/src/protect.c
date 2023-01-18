/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2017-2013 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "mmu.h"
#include "x86.h"
#include <string.h>
/* ================================ [ MACROS    ] ============================================== */
#define LDT_SIZE 2
#define GDT_SIZE 256
#define IDT_SIZE 256
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
extern void serial_init(void);
extern void terminal_initialize(void);

extern void init_8259A(void);

/* 中断处理函数 */
void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();
void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();
void sys_call();
/* ================================ [ DATAS     ] ============================================== */
mmu_descriptor_t g_Gdt[GDT_SIZE];
mmu_descinfo_t g_GdtInfo = {GDT_SIZE * sizeof(mmu_descriptor_t) - 1, g_Gdt};

mmu_gate_t g_Idt[IDT_SIZE];
mmu_gateinfo_t g_IdtInfo = {IDT_SIZE * sizeof(mmu_gate_t) - 1, g_Idt};
tss_t g_Tss;
/* ================================ [ LOCALS    ] ============================================== */
static void init_descriptor(mmu_descriptor_t *p_desc, uint32_t base, uint32_t limit,
                            uint16_t attribute) {
  p_desc->limit_low = limit & 0x0FFFF;     // 段界限 1		(2 字节)
  p_desc->base_low = base & 0x0FFFF;       // 段基址 1		(2 字节)
  p_desc->base_mid = (base >> 16) & 0x0FF; // 段基址 2		(1 字节)
  p_desc->attr1 = attribute & 0xFF;        // 属性 1
  p_desc->limit_high_attr2 =
    ((limit >> 16) & 0x0F) | ((attribute >> 8) & 0xF0); // 段界限 2 + 属性 2
  p_desc->base_high = (base >> 24) & 0x0FF;             // 段基址 3		(1 字节)
}

static void init_idt_desc(unsigned char vector, uint8_t desc_type, void (*handler)(void),
                          unsigned char privilege) {
  mmu_gate_t *p_gate = &g_Idt[vector];
  uint32_t base = (uint32_t)handler;
  p_gate->offset_low = base & 0xFFFF;
  p_gate->selector = SELECTOR_KERNEL_CS;
  p_gate->dcount = 0;
  p_gate->attr = desc_type | (privilege << 5);
  p_gate->offset_high = (base >> 16) & 0xFFFF;
}

static uint32_t seg2phys(uint16_t seg) {
  mmu_descriptor_t *p_dest = &g_Gdt[seg >> 3];

  return (p_dest->base_high << 24) | (p_dest->base_mid << 16) | (p_dest->base_low);
}

static void init_prot(void) {
  init_8259A();
#if 0
  // 全部初始化成中断门(没有陷阱门)
  init_idt_desc(INT_VECTOR_DIVIDE, DA_386IGate, divide_error, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_DEBUG, DA_386IGate, single_step_exception, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_NMI, DA_386IGate, nmi, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_BREAKPOINT, DA_386IGate, breakpoint_exception, PRIVILEGE_USER);
  init_idt_desc(INT_VECTOR_OVERFLOW, DA_386IGate, overflow, PRIVILEGE_USER);
  init_idt_desc(INT_VECTOR_BOUNDS, DA_386IGate, bounds_check, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_INVAL_OP, DA_386IGate, inval_opcode, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_COPROC_NOT, DA_386IGate, copr_not_available, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_DOUBLE_FAULT, DA_386IGate, double_fault, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_COPROC_SEG, DA_386IGate, copr_seg_overrun, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_INVAL_TSS, DA_386IGate, inval_tss, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_SEG_NOT, DA_386IGate, segment_not_present, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_STACK_FAULT, DA_386IGate, stack_exception, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_PROTECTION, DA_386IGate, general_protection, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_PAGE_FAULT, DA_386IGate, page_fault, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_COPROC_ERR, DA_386IGate, copr_error, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KRNL);
  init_idt_desc(INT_VECTOR_SYS_CALL, DA_386IGate, sys_call, PRIVILEGE_USER);
#endif
}
/* ================================ [ FUNCTIONS ] ============================================== */
void cstart(void) {
  terminal_initialize();
  serial_init();
  init_prot();

  init_descriptor(&g_Gdt[INDEX_DUMMY], 0, 0, 0);
  init_descriptor(&g_Gdt[INDEX_FLAT_C], 0, 0xfffff, DA_CR | DA_32 | DA_LIMIT_4K);
  init_descriptor(&g_Gdt[INDEX_FLAT_RW], 0, 0xfffff, DA_DRW | DA_32 | DA_LIMIT_4K);
  init_descriptor(&g_Gdt[INDEX_VIDEO], 0xB8000, 0xffff, DA_DRW | DA_DPL3);
  /* 填充 GDT 中 TSS 这个描述符 */
  memset(&g_Tss, 0, sizeof(g_Tss));
  g_Tss.ss0 = SELECTOR_KERNEL_DS;
  init_descriptor(&g_Gdt[INDEX_TSS], vir2phys(seg2phys(SELECTOR_KERNEL_DS), &g_Tss),
                  sizeof(g_Tss) - 1, DA_386TSS);
  g_Tss.iobase = sizeof(g_Tss); /* 没有I/O许可位图 */
}
