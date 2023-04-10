MEMORY
{
  RAM     (rwx): ORIGIN = 0x40000000, LENGTH = 7M
  FLSDRV  (rwx): ORIGIN = 0x40700000, LENGTH = 1M
  APPCODE (rx) : ORIGIN = 0x40800000, LENGTH = 120M
  PCINP   (rw) : ORIGIN = 0x10000000, LENGTH = 256M
  PCIP    (rw) : ORIGIN = 0x3eff0000, LENGTH = 64K
}
ENTRY(_start)

reset_main = 0x40000000;

application_main = 0x40800000;
FlashHeader = 0x40700000;
FlashDriverRam = 0x40700000;
SECTIONS
{
  .init : {
    KEEP(*(.init))
  } > RAM

  .text : {
    . = ALIGN(8);
    *(.text*)
  } > RAM
  __etext = .;

  .data : {
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
    *(.rodata*)
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
  .app : {
    KEEP(*(.app))
  } > APPCODE
}