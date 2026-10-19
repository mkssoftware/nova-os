/* Test-Linkerscript: platziert den Code bei 0x100000, wie der bisherige Nova-Kernel. */
ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    .text : { *(.text) }
    .rdata : { *(.rdata) *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}
