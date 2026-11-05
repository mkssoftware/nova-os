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
    /* Seitenausrichtung VOR dem ersten schreibbaren Abschnitt: dadurch
       teilen sich Code/Konstanten (.text/.rdata, oben) und veraenderliche
       Daten (.data/.bss, unten) NIE dieselbe 4-KiB-Seite - Voraussetzung
       dafuer, dass ushell_process.c sie mit unterschiedlichen Paging-
       Rechten (Code schreibgeschuetzt, Daten beschreibbar) abbilden kann.
       __rw_start markiert die Grenze fuer den Lader (siehe dort). */
    . = ALIGN(4096);
    __rw_start = .;
    .data : { *(.data) }
    .bss : { *(.bss) }
    /DISCARD/ : { *(.reloc) *(.pdata) *(.xdata) *(.eh_frame) *(.CRT) }
}
