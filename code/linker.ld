ENTRY(reset_handler)

MEMORY
{
    FLASH (rx): ORIGIN = 0x08000000, LENGTH = 256K
    SRAM (rwx): ORIGIN = 0x20000000, LENGTH = 64K
}

_stack = ORIGIN(SRAM) + LENGTH(SRAM);
    
SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH
    
    .text :
    {
        . = ALIGN(4);

        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)

        . = ALIGN(4);
        _etext = .;
    } >FLASH

    _idata = LOADADDR(.data);
    
    .data :
    {
        . = ALIGN(4);
        _sdata = .;

        *(.data)
        *(.data*)
    
        . = ALIGN(4);
        _edata = .;
    } >SRAM AT> FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;

        *(.bss)
        *(.bss*)

        . = ALIGN(4);
        _ebss = .;        
    } >SRAM
}
