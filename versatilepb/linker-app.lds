MEMORY
{
   FLASH        (rx)   : ORIGIN = 0x00040000, LENGTH = 1024K
   RAM          (rwx)  : ORIGIN = 0x00140000, LENGTH = 256K
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
      __data_end__ = .;
   } > RAM

   .bss : {
     . = ALIGN(4);
      __bss_start = .;   
      *(.bss*)
   } > RAM
   __bss_end = .;
}

