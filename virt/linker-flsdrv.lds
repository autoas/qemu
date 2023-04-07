MEMORY
{
  FLASH        (rwx)  : ORIGIN = 0x40700000, LENGTH = 4K
}

ENTRY(FlashHeader)

SECTIONS
{
    .text :
    {
      *(.rodata*)
      *(.text*)
    } > FLASH
}
