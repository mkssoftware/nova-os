/* Linkerscript fuer ein einzelnes C-Modul, das per incbin an einer bestimmten
   Adresse in den Flat-Binary-Kernel eingebettet wird. BASE wird vom Aufrufer
   via --defsym gesetzt (Adresse, an der das Modul im Kernel-Image landet). */
ENTRY(c_hello)
SECTIONS
{
    . = BASE;
    .text : { *(.text) }
    .rdata : { *(.rdata) *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    /DISCARD/ : { *(.reloc) *(.pdata) *(.xdata) }
}
