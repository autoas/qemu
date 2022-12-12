
ENTRY(reset_handler)

MEMORY
{
RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 64K
FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 256K
}

knl_system_stack_size = 1024;

SECTIONS
{

  .text :
  {
    KEEP(*(.isr_vector))
    *(.startup*)
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    . = ALIGN(4);
    __etext = .;
  } >FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    __data_start__ = .;
    *(.data)
    *(.data*)

    . = ALIGN(4);
    __data_end__ = .;
  } >RAM AT> FLASH

  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    __bss_start__ = .;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    __bss_end__ = .;
  } >RAM

  .init_stack ALIGN(16) (NOLOAD) : 
  {
    . = ALIGN(4);
    knl_system_stack    = .; 
    . = . + knl_system_stack_size;   
    knl_system_stack_top = .;
  } > RAM
}
