/* NovaOS - Linkerscript fuer den Userspace-Systemeinstellungen-Prozess
   (siehe settings_main.c/ushell_process.c). Gleiche Basisadresse wie
   alle anderen Ring-3-Prozesse (0x00500000) - KEIN Konflikt, jeder
   Prozess bekommt einen eigenen Adressraum (siehe ushell_process.c).
   Seitenausrichtung VOR dem ersten schreibbaren Abschnitt (W^X-
   Trennung, siehe ushell-link.ld/icons-link.ld fuer die volle
   Begruendung in ushell_process.c). */
ENTRY(settings_start)
SECTIONS
{
    . = 0x00500000;
    .text : { *(.text) *(.text.*) }
    .rdata : { *(.rdata) *(.rdata.*) *(.rodata) *(.rodata.*) }
    . = ALIGN(4096);
    __rw_start = .;
    .data : { *(.data) *(.data.*) }
    .bss : { *(.bss) *(.bss.*) *(COMMON) }
    /DISCARD/ : { *(.reloc) *(.pdata) *(.xdata) *(.eh_frame) *(.CRT) }
}
