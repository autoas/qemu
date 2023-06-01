MEMORY
{
  FLASH (rwx) : ORIGIN = 0x40800000, LENGTH = 1M
  RAM   (rwx) : ORIGIN = 0x40900000, LENGTH = 119M
  PCINP (rw)  : ORIGIN = 0x10000000, LENGTH = 256M
  PCIP  (rw)  : ORIGIN = 0x3eff0000, LENGTH = 64K
}
ENTRY(_start)
/* application address */
reset_main = 0x40000000;
SECTIONS
{
  .init : {
    KEEP(*(.init))
  } > FLASH

  .text : {
    . = ALIGN(8);
    *(.text*)
    . = ALIGN(8);
    *(.rodata*)
    . = ALIGN(8);
  } > FLASH
  __etext = .;

  .data : AT (__etext) {
    . = ALIGN(8);
    __data_start__ = .;
    *(.data*)
    . = ALIGN(8);
    __ssymtab_start = .;
    KEEP(*(ShellCmdTab))
    __ssymtab_end = .;
    . = ALIGN(8);
    __devtab_start = .;
    KEEP(*(DeviceTab))
    __devtab_end = .;
    . = ALIGN(8);
    __data_end__ = .;
  } > RAM

  .bss :
  {
    . = ALIGN(8);
    __bss_start = .;  
    *(.bss)
    *(.bss.*)
  } > RAM
  __bss_end = .;
    _end = .;
  . = ALIGN(8);
  . = . + 0x4000; /* 16kB of stack memory */
  stack_top = .;
  . = ALIGN(8);
  . = . + 0x4000; /* 16kB of stack memory */
  stack2_top = .;

  .pcinp :
  {
    *(.pcinp)
    *(.pcinp.*)
  } > PCINP

  .pcip :
  {
    *(.pcip)
    *(.pcip.*)
  } > PCIP
}