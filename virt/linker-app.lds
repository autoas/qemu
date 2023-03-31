MEMORY
{
	RAM		(rwx)	: ORIGIN = 0x40000000, LENGTH = 128M
  PCINP   (rw)  : ORIGIN = 0x10000000, LENGTH = 256M
  PCIP    (rw)  : ORIGIN = 0x3eff0000, LENGTH = 64K
}
ENTRY(_start)
/* application address */
reset_main = 0x40000000;
SECTIONS
{
  .init : {
    KEEP(*(.init))
  } > RAM

  .text : {
    . = ALIGN(8);
    *(.text*)
  } > RAM
  _etext = .;

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
}