ENTRY(_start)
 
SECTIONS
{
    /* Starts at LOADER_ADDR. */
    . = 0x10000;
    __start = .;
    __text_start = .;
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)
    }
    . = ALIGN(4096); /* align to page size */
    __text_end = .;
 
    __rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096); /* align to page size */
    __rodata_end = .;
 
    __data_start = .;
    .data :
    {
        *(.data)
    }
    . = ALIGN(4096); /* align to page size */
    __data_end = .;
 
    __bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096); /* align to page size */
    __bss_end = .;
    __end = .;
    .trailer ALIGN(4) :
    {
        LONG(0)
        LONG(0)
        LONG(0x00000001)    /* DTOK = 1, please give us device tree */
        LONG(0x00000004)
        LONG(0x4b4f5444)    /* DTOK */
        LONG(0x00000020)
        LONG(0x00000004)
        LONG(0x4c545052)
    }
}