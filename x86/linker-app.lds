ENTRY(_start)

SECTIONS {
  . = 0x00100000;
  .init : {
    KEEP(*(.init))
  }

  .text : {
    . = ALIGN(4);
    *(.text*)
    . = ALIGN(4);
    __ssymtab_start = .;
    KEEP(*(ShellCmdTab))
    __ssymtab_end = .;
    . = ALIGN(4);
    __devtab_start = .;
    KEEP(*(DeviceTab))
    __devtab_end = .;
    . = ALIGN(4);
  }
  _etext = .;

  .data : {
    . = ALIGN(4);
    __data_start__ = .;
    *(.data*)
    __data_end__ = .;
  }

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;  
    *(.bss)
    *(.bss.*)
    __bss_end__ = .;
  }
  
  _end = .;

}

