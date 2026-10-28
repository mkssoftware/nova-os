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

/* ===================================================================== */
/* B+-Tree-Kern (Batch 4): Node-I/O, Cursor, Item-Vergleich, einfaches    */
/* Einfuegen OHNE Split/Merge (folgt erst mit einem spaeteren Batch).     */
/* Erfordert einen zuvor per novafs_probe_volume() erfolgreich            */
/* sondierten Datentraeger.                                               */
/* ===================================================================== */

typedef struct {
    uint32_t id;
    uint32_t item_size;
    uint32_t max_items;
    uint32_t key_size;       /* 8 oder 16 */
    uint32_t sb_root_offset; /* Offset der Baumwurzel im Superblock */
} novafs_tree_t;

extern const novafs_tree_t novafs_tree_object;
extern const novafs_tree_t novafs_tree_directory;
extern const novafs_tree_t novafs_tree_extent;

/* Setzt den globalen Suchschluessel (nfs_key) fuer cursor_seek/compare.
   secondary wird fuer Baeume mit 8-Byte-Schluessel ignoriert. */
void novafs_key_set(uint32_t primary, uint32_t secondary);

/* Positioniert den globalen Cursor auf das erste Item >= dem zuvor per
   novafs_key_set() gesetzten Schluessel (steigt dabei durch beliebig
   viele innere Knotenebenen ab, bis NFS_MAX_HEIGHT). 0 = Erfolg
   (novafs_cursor_item_ptr() gueltig oder der Cursor steht am Ende des
   Blatts - novafs_cursor_advance() steigt dann automatisch ins naechste
   Geschwisterblatt auf), -1 bei I/O-/Strukturfehler. */
int novafs_cursor_seek(const novafs_tree_t *tree);

/* Rohdaten-Zeiger (tree->item_size Bytes) auf das Item, auf dem der
   Cursor aktuell steht, 0 wenn der Cursor am Ende des Baums steht. NUR
   gueltig direkt nach einem erfolgreichen novafs_cursor_seek()/
   novafs_cursor_advance() fuer DENSELBEN Baum. */
uint8_t *novafs_cursor_item_ptr(void);

/* Bewegt den Cursor zum naechsten Item (wechselt bei Bedarf automatisch
   ins naechste Geschwisterblatt). 0 = Erfolg, -1 = Ende des Baums. */
int novafs_cursor_advance(void);

/* Fuegt item (tree->item_size Bytes) sortiert in das Blatt ein, auf das
   der zuletzt per novafs_cursor_seek() positionierte Cursor zeigt. NUR
   der Fall "Blatt hat noch Platz" ist implementiert - ist das Zielblatt
   voll, schlaegt die Operation sauber mit -1 fehl (KEIN Split - folgt
   erst mit einem spaeteren Batch). 0 = Erfolg (sofort auf den
   Datentraeger geschrieben - es gibt in diesem Batch noch keine
   Transaktions-/Journal-Klammer). */
int novafs_tree_insert_simple(const novafs_tree_t *tree, const uint8_t *item);

int novafs_tree_self_test(void); /* 0 = ok */

/* ===================================================================== */
/* Verzeichnisse und Pfade (Batch 5): Object-Lookup + Namensaufloesung   */
/* ueber den Directory-Baum + Pfadaufloesung - NUR LESEN (Anlegen/        */
/* Schreiben folgt mit dem Journal-/Write-Batch).                        */
/* ===================================================================== */

#define NOVAFS_TYPE_FILE      1u
#define NOVAFS_TYPE_DIRECTORY 2u

/* NFS_O_ITEM Byte Pufferkapazitaet fuer novafs_object_find()'s Ausgabe
   (Objekt-Item-Rohbytes - siehe novafs32.inc NFS_O_* Feldoffsets fuer den
   Aufbau: NFS_O_TYPE bei Offset 16 ist der am haeufigsten benoetigte
   Wert). */
#define NOVAFS_OBJECT_ITEM_SIZE 152u

/* Sucht object_id im Object-Baum, prueft dessen eigene Item-Pruefsumme
   und kopiert das Rohitem (NOVAFS_OBJECT_ITEM_SIZE Byte) nach out_item.
   0 = Erfolg, -1 = nicht gefunden/beschaedigt. */
int novafs_object_find(uint32_t object_id, uint8_t *out_item);

/* Sucht einen Verzeichniseintrag mit Namen name (len Byte, KEIN
   Nullterminator) unter parent_id. 0 = Erfolg (*out_object_id und *out_type
   gesetzt), -1 wenn nicht vorhanden/beschaedigt. */
int novafs_lookup(uint32_t parent_id, const char *name, uint32_t len, uint32_t *out_object_id,
                   uint32_t *out_type);

/* NFS_D_ITEM Byte Pufferkapazitaet fuer novafs_dir_entry_at()'s Ausgabe. */
#define NOVAFS_DIR_ITEM_SIZE 288u

/* Kopiert den index-ten Verzeichniseintrag (0-basiert) von dir_id nach
   out_item (NOVAFS_DIR_ITEM_SIZE Byte) - fuer Verzeichnislistungen. Die
   Eintraege eines Verzeichnisses erscheinen in KEINER garantierten Namens-
   oder Erstellungsreihenfolge (sortiert nach internem Namens-Hash). 0 =
   Erfolg, -1 = Index >= Anzahl der Eintraege (oder dir_id hat gar keine). */
int novafs_dir_entry_at(uint32_t dir_id, uint32_t index, uint8_t *out_item);

/* Loest path relativ zu start_dir auf - ein fuehrendes '/' im Pfad
   beginnt bei der Wurzel (ObjectID 1) und ignoriert start_dir. 0 = Erfolg
   (*out_object_id und *out_type gesetzt), -1 wenn ein Segment nicht existiert
   oder ein Nicht-Verzeichnis durchquert werden muesste. */
int novafs_resolve_from(uint32_t start_dir, const char *path, uint32_t len, uint32_t *out_object_id,
                         uint32_t *out_type);

/* Wie novafs_resolve_from(1, ...), erfordert aber ein fuehrendes '/'
   (relative Pfade werden abgelehnt). */
int novafs_resolve_path(const char *path, uint32_t len, uint32_t *out_object_id,
                         uint32_t *out_type);

int novafs_path_self_test(void); /* 0 = ok */

int novafs_dir_self_test(void); /* 0 = ok */

#endif
