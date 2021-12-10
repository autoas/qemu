MEMORY
{
  RAM    (rwx)  : ORIGIN = 0x40000000, LENGTH = 256K
	APPCODE		(rwx)	: ORIGIN = 0x40060000, LENGTH = 128M
  PCINP   (rw)  : ORIGIN = 0x10000000, LENGTH = 256M
  PCIP    (rw)  : ORIGIN = 0x3eff0000, LENGTH = 64K
}

ENTRY(_start)

/* application address */
application_main = 0x40060040;

FlashHeader = 0x40050000;

FlashDriverRam = 0x40050000;

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
    *(.data*)
    . = ALIGN(8);
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