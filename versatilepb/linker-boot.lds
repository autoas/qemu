/**
 * BlueThunder Linker Script for the raspberry Pi!
 *
 *
 *
 **/
MEMORY
{
	RESERVED	(r)		: ORIGIN = 0x00000000, LENGTH = 32K
	INIT_RAM	(rwx)	: ORIGIN = 0x00008000, LENGTH = 32K
	RAM			(rwx)	: ORIGIN = 0x00010000, LENGTH = 256K
	APPCODE		(rwx)	: ORIGIN = 0x00060000, LENGTH = 128M
	/* This PCIIO PCINP PCIP is for versatilepb */
	PCIIO		(rw)	: ORIGIN = 0x43000000, LENGTH = 16M
	PCINP		(rw)	: ORIGIN = 0x50000000, LENGTH = 256M
	PCIP		(rw)	: ORIGIN = 0x60000000, LENGTH = 256M
}

ENTRY(_start)

/* application address */
application_main = 0x00060040;

FlashHeader = 0x00050000;

FlashDriverRam = 0x00050000;

SECTIONS {
    /*
	 * Our init section allows us to place the bootstrap code at address 0x8000
	 *
	 * This is where the Graphics processor forces the ARM to start execution.
	 * However the interrupt vector code remains at 0x0000, and so we must copy the correct
	 * branch instructions to 0x0000 - 0x001C in order to get the processor to handle interrupts.
	 *
	 */
	.init : {
		KEEP(*(.init))
	} > INIT_RAM = 0

	.module_entries : {
		__module_entries_start = .;
	  	KEEP(*(.module_entries))
		KEEP(*(.module_entries.*))
		__module_entries_end = .;
		__module_entries_size = SIZEOF(.module_entries);
	} > INIT_RAM


	/**
 	 *	This is the main code section, it is essentially of unlimited size. (128Mb).
	 *
	 **/
	.text : {
		*(.text)
        /* section information for shell */
        . = ALIGN(4);
        __ssymtab_start = .;
        KEEP(*(SSymTab))
        __ssymtab_end = .;
        /* section information for ELF Symbols */
        . = ALIGN(4);
        __elfsymtab_start = .;
        KEEP(*(ELFSymTab))
        __elfsymtab_end = .;
		. = ALIGN(4);
        autostart_processes = .;
        KEEP(*(ProtoSymTab))
        KEEP(*(ProtoSymTabEnd))

	} > RAM
    _etext = .;

	/*
	* Next we put the data.
	*/
	.data : {
		*(.data)
	} > RAM
	

	.bss :
	{
		__bss_start = .;	
		*(.bss)
		*(.bss.*)
	} > RAM
	__bss_end = .;
    _end = .;

	.pciio :
	{
		*(.pciio)
		*(.pciio.*)
	} > PCIIO

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

