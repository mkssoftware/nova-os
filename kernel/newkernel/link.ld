/* NovaOS C-Kernel - Linkerscript. Platziert das Image bei KERNEL_ENTRY_ADDRESS
   (0x100000), identisch zum alten Flat-Binary-Kernel, damit die bestehende
   UEFI-Bootloader/NKI-Pipeline unveraendert weiterverwendet werden kann. */
ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    .text : { *(.text) }
    .rdata : { *(.rdata) *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    /DISCARD/ : { *(.reloc) *(.pdata) *(.xdata) *(.eh_frame) *(.CRT) }
}
