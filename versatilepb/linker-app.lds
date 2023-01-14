MEMORY
{
   FLASH        (rx)   : ORIGIN = 0x00040000, LENGTH = 1024K
   RAM          (rwx)  : ORIGIN = 0x00140000, LENGTH = 127M
}

ENTRY(_start)

reset_main = 0x00008040;

SECTIONS {
   .text : {
      *(.init)
      *(.text*)
      *(.rodata*)
      . = ALIGN(4);
   } > FLASH
   __etext = .;

   .data : AT (__etext) {
      . = ALIGN(4);
      __data_start__ = .;
      *(.data*)
      . = ALIGN(4);
      __ssymtab_start = .;
      KEEP(*(ShellCmdTab))
      __ssymtab_end = .;
      . = ALIGN(4);
      __devtab_start = .;
      KEEP(*(DeviceTab))
      __devtab_end = .;
      . = ALIGN(4);
      __data_end__ = .;
   } > RAM

   .bss : {
     . = ALIGN(4);
      __bss_start = .;   
      *(.bss*)
   } > RAM
   __bss_end = .;
}

