MEMORY
{
  RESERVED  (r)    : ORIGIN = 0x00000000, LENGTH = 32K
  FLASH     (rwx)  : ORIGIN = 0x00050000, LENGTH = 64K
}

ENTRY(FlashHeader)

SECTIONS
{
    .text :
    {
    *(.rodata*)
        *(.data*)
        *(.text*)
    } > FLASH
}
