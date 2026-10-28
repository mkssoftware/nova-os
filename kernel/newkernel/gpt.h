/* NovaOS C-Kernel - GPT-Partitionssuche fuer die NovaFS-Systempartition
   (Port aus storage32.inc, GPT-Teil, NPSPEC-NOVAFS-ONDISK-0001 Abschnitt 2).

   Arbeitet generisch ueber die block.c-Abstraktion (block_io_submit) statt
   wie im alten ASM direkt ueber eine storage.c-eigene API - funktioniert
   damit fuer jeden ueber block.c registrierten Datentraeger (AHCI/NVMe/...),
   nicht nur AHCI. */
#ifndef NOVA_GPT_H
#define NOVA_GPT_H

#include <stdint.h>

/* Liest den GPT-Header (LBA 1) und die Partitionstabelle eines
   Block-Devices, prueft Signatur "EFI PART", Header-Groesse (92..512 Byte),
   Header- UND Eintrags-CRC32 (IEEE 802.3, wie im alten ASM), Eintrags-LBA/
   -Groesse (<2^32 bzw. genau 128 Byte/Eintrag), und sucht einen Eintrag mit
   der NovaFS-Partitions-Typ-GUID. Die gefundene Partition muss auf einer
   4-KiB-Grenze beginnen und vollstaendig innerhalb der Geraetekapazitaet
   liegen (block_record_info()). record ist ein von
   block_device_register()/block_device_lookup_by_handle() geliefertes
   Opaque-Handle.

   0 = Erfolg (*out_first_lba und *out_sector_count gesetzt), -1 wenn keine
   gueltige NovaFS-Systempartition gefunden wurde oder ein I/O-/CRC-Fehler
   auftrat - KEIN Fehlerzustand im Boot-Sinn (siehe gpt_self_test fuer die
   eigentliche Korrektheitspruefung; ein echtes Fehlen der Partition auf
   dem jeweiligen Datentraeger ist normal, solange kein NovaFS-Volume
   angelegt wurde). */
int gpt_find_novafs_partition(void *record, uint32_t *out_first_lba, uint32_t *out_sector_count);

int gpt_self_test(void); /* 0 = ok */

#endif
