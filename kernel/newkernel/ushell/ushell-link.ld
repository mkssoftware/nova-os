/* NovaOS - Linkerscript fuer den Userspace-Taskbar-Prozess (siehe
   shell_main.c/ushell_process.c). Platziert das Image bei 0x00500000
   (CODE_VADDR in ushell_process.c), flach wie der Kernel selbst - kein
   eigener Loader/ELF-Interpreter noetig, der Kernel kopiert das Ergebnis
   von objcopy -O binary direkt in die von ihm gemappten Prozessseiten. */
ENTRY(ushell_start)
SECTIONS
{
    . = 0x00500000;
    .text : { *(.text) }
    .rdata : { *(.rdata) *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    /DISCARD/ : { *(.reloc) *(.pdata) *(.xdata) *(.eh_frame) *(.CRT) }
}
