/* NovaOS - Linkerscript fuer den Userspace-Startmenue-Prozess (siehe
   startmenu_main.c/ushell_process.c). Gleiche Basisadresse wie
   shell_main.c (0x00500000) - KEIN Konflikt, jeder Prozess bekommt einen
   eigenen Adressraum (siehe ushell_process.c). */
ENTRY(startmenu_start)
SECTIONS
{
    . = 0x00500000;
    .text : { *(.text) }
    .rdata : { *(.rdata) *(.rodata) }
    /* Seitenausrichtung VOR dem ersten schreibbaren Abschnitt - siehe
       ushell-link.ld fuer die Begruendung (W^X-Trennung in ushell_process.c). */
    . = ALIGN(4096);
    __rw_start = .;
    .data : { *(.data) }
    .bss : { *(.bss) }
    /DISCARD/ : { *(.reloc) *(.pdata) *(.xdata) *(.eh_frame) *(.CRT) }
}
