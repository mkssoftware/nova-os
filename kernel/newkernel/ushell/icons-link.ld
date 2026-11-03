/* NovaOS - Linkerscript fuer den Userspace-Desktop-Icons-Prozess (siehe
   icons_main.c/ushell_process.c). Gleiche Basisadresse wie
   shell_main.c/startmenu_main.c (0x00500000) - KEIN Konflikt, jeder
   Prozess bekommt einen eigenen Adressraum (siehe ushell_process.c). */
ENTRY(icons_start)
SECTIONS
{
    . = 0x00500000;
    .text : { *(.text) *(.text.*) }
    .rdata : { *(.rdata) *(.rdata.*) *(.rodata) *(.rodata.*) }
    .data : { *(.data) *(.data.*) }
    /* -fdata-sections benennt unitialisierte Statics ".bss.<name>" statt
       schlicht ".bss" - beide Wildcard-Formen hier aufnehmen, sonst
       landen sie (GNU-ld-Orphan-Platzierung) faelschlich in .data und
       blaehen die per objcopy -O binary erzeugte Flat-Binary unnoetig mit
       echten Nullbytes auf, statt platzsparend als SHT_NOBITS zu
       bleiben. */
    .bss : { *(.bss) *(.bss.*) *(COMMON) }
    /DISCARD/ : { *(.reloc) *(.pdata) *(.xdata) *(.eh_frame) *(.CRT) }
}
