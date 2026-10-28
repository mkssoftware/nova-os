/* NovaOS C-Kernel - NovaFS On-Disk-Dateisystem (Port aus novafs32.inc).
   Dieses Modul waechst ueber mehrere Batches:

   Batch 3 (dieser Stand): Superblock-Auswahl (primaer/Backup, hoechste
   Generation gewinnt) + Free-Space-Bitmap-Laden/-Verifikation. B+-Tree-
   Kern, Object/Directory/Extent-Baeume, VFS-Anbindung, Journal/Writes
   folgen in Folgebatches - deshalb heisst die Einstiegsfunktion bewusst
   NICHT "novafs_mount" (ein vollstaendiger Mount im Sinn des alten ASM
   prueft zusaetzlich die B+-Tree-Wurzeln und laedt das Root-Verzeichnis,
   was ohne B+-Tree-Kern noch nicht moeglich ist).

   CRC32C (Castagnoli) wird fuer alle NovaFS-Metadaten verwendet (ANDERE
   Pruefsumme als gpt.c's CRC32/IEEE fuer GPT) - bit-fuer-Bit ohne Tabelle,
   fuer die hier anfallenden Datenmengen (Superblock+Bitmap, max. ~20KB)
   ausreichend schnell. */
#ifndef NOVA_NOVAFS_H
#define NOVA_NOVAFS_H

#include <stdint.h>

#define NOVAFS_BLOCK_SIZE 4096u

/* Liest Superblock (primaer, Block 1, UND Backup, letzter Partitionsblock)
   eines Datentraegers, waehlt die gueltige Kopie mit der hoechsten
   Generation, prueft Feature-Flags/Zustand, laedt die Free-Space-Bitmap
   und verifiziert sie per CRC32C sowie per Belegt-Markierung der beiden
   Superblock-Bloecke.

   record = block.c-Record (z.B. von ahci_disk_block_record()).
   part_first_lba/part_sector_count = Partitionsgeometrie in 512-Byte-
   Sektoren (z.B. aus gpt_find_novafs_partition()).

   0 = Erfolg (Superblock+Bitmap verifiziert - siehe novafs_total_blocks()
   etc. fuer das Ergebnis), -1 wenn keine gueltige Kopie gefunden wurde,
   die Bitmap-CRC nicht stimmt, nicht unterstuetzte Incompat-Flags gesetzt
   sind, oder der Zustand DIRTY ist (Journal-Wiederherstellung folgt erst
   mit dem Journal-/Write-Batch - ein DIRTY-Volume gilt bis dahin bewusst
   als nicht nutzbar statt blind weiterzumachen). KEIN Fehlerzustand im
   Boot-Sinn, solange schlicht keine NovaFS-Partition existiert (siehe
   gpt.h). */
int novafs_probe_volume(void *record, uint32_t part_first_lba, uint32_t part_sector_count);

uint32_t novafs_total_blocks(void);
uint32_t novafs_bitmap_blocks(void);
uint32_t novafs_generation_lo(void); /* Low-Dword der 64-Bit-Generation */
int novafs_is_readonly(void);
int novafs_is_used_backup(void); /* 1 = Backup-Superblock war die gueltigere Kopie */

int novafs_self_test(void); /* 0 = ok */

#endif
