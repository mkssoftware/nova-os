/* NovaOS C-Kernel - AHCI SATA Storage Driver (Port aus storage32.inc,
   NPSPEC-STORAGE-DEVICE-0001 / NPSPEC-STORAGE-DISCOVERY-0001). Nur der
   AHCI-/ATA-Teil dieser Datei - GPT-Partitionssuche und CRC-Tabellen folgen
   in einem spaeteren Batch, sobald sie fuer die NovaFS-Montage benoetigt
   werden.

   Polling-Betrieb (kein Interrupt, analog zum alten ASM): jedes Kommando
   wird synchron in Slot 0 ausgefuehrt und per Timeout ueberwacht. */
#ifndef NOVA_AHCI_H
#define NOVA_AHCI_H

#include <stdint.h>

#define AHCI_DISK_CAPACITY 4u

/* Durchsucht die Driver-Discovery-Tabelle (driver.c) nach einem AHCI-
   Controller, setzt ihn in den AHCI-Modus (Polling, keine Interrupts) und
   registriert jeden erkannten ATA-Datentraeger als Block-Device (block.c).
   Fehlende Hardware ist kein Fehler (immer 0). */
int ahci_initialize(void);
int ahci_self_test(void); /* 0 = ok (kein Controller/Datentraeger = ok) */

/* Fuer Boot-Diagnose (kmain.c): Anzahl erkannter Datentraeger und
   Sektorzahl (lo-Wort) des ersten, 0 wenn keiner erkannt wurde. */
uint32_t ahci_disk_count_value(void);
uint32_t ahci_disk_sectors_lo(uint32_t index);

#endif
