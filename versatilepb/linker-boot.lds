MEMORY
{
   FLASH        (rx)   : ORIGIN = 0x00008000, LENGTH = 224K
   APPCODE      (rx)   : ORIGIN = 0x00040000, LENGTH = 1024K
   FLSDRV       (rwx)  : ORIGIN = 0x00140000, LENGTH = 4K
   RAM          (rwx)  : ORIGIN = 0x00141000, LENGTH = 252K
}

ENTRY(_start)

application_main = 0x00040040;
reset_main = 0x00008040;
FlashHeader = 0x00140000;
FlashDriverRam = 0x00140000;

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
      __data_end__ = .;
   } > RAM

   .bss : {
      . = ALIGN(4);
      __bss_start = .;   
      *(.bss*)
   } > RAM
   __bss_end = .;

   .app : {
      KEEP(*(.app))
   } > APPCODE
}

