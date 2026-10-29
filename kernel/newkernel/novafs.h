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
/* 4096 / 512 - fuer Aufrufer AUSSERHALB von novafs.c, die ein eigenes
   BLOCK_TRANSPORT_VIRT-Testgeraet registrieren (siehe block.h) und dessen
   LBA (Sektor) in einen Block umrechnen muessen (z.B. vfsapi.c's
   Selbsttest-Fixture, siehe novafs_build_test_volume unten). */
#define NOVAFS_SECTORS_PER_BLOCK 8u

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

/* Liest Typ und Groesse (64-Bit als lo/hi) eines Objekts, ohne dessen
   internes Rohitem-Layout (NFS_O_*-Offsets) nach aussen zu exponieren -
   fuer den VFS-Syscall-Layer (vfs_api.c, VFS.Query). out_*-Parameter sind
   einzeln optional (0 = ignorieren). 0 = Erfolg, -1 = object_id unbekannt. */
int novafs_object_info(uint32_t object_id, uint32_t *out_type, uint32_t *out_size_lo,
                        uint32_t *out_size_hi);

/* Liest den Parent-ObjectID eines Objekts - fuer Ahnen-Pruefungen wie
   novafs_rename's Zyklen-Schutz (intern) oder vfs_api.c's Systembereichs-
   Policy (NPSPEC-POLICY-SYSTEMWRITE-0001). 0 = Erfolg (*out_parent_id
   gesetzt, ObjectID 1 = Wurzel hat sich selbst als Parent), -1 wenn
   object_id unbekannt. */
int novafs_object_parent(uint32_t object_id, uint32_t *out_parent_id);

/* 1 = ein Volume ist aktuell gemountet (novafs_probe_volume erfolgreich),
   0 sonst - fuer Aufrufer ausserhalb von novafs.c, die vor jeder Operation
   pruefen muessen (vfs_api.c), ohne auf novafs_total_blocks()!=0 als
   impliziten Proxy angewiesen zu sein. */
int novafs_is_mounted(void);

/* Erzwingt Read-only fuer den Rest des Starts (NPSPEC-NOVAFS-ONDISK-0001
   §9 Fallback: wenn selbst ein Rollback per novafs_change_abort nach einem
   gescheiterten Commit fehlschlaegt, siehe vfs_api.c's vfs_change_end).
   Es gibt bewusst keinen Weg zurueck (kein novafs_force_readwrite) -
   dieser Zustand ist erst mit einem Neustart wieder aufloesbar, genauso
   wie im alten ASM (nfs_readonly blieb dort ebenfalls dauerhaft 1). */
void novafs_force_readonly(void);

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

/* Zerlegt ein per novafs_dir_entry_at() geholtes Rohitem in die fuer
   Aufrufer relevanten Felder - fuer den VFS-Syscall-Layer (vfsapi.c,
   VFS.ReadDirectory), ohne das interne NFS_D_*-Layout nach aussen zu
   exponieren. out_name muss mindestens 255 Byte Platz bieten und wird
   NICHT nullterminiert (siehe *out_name_len fuer die echte Laenge).
   out_*-Parameter sind einzeln optional (0 = ignorieren). */
void novafs_dir_entry_decode(const uint8_t *item, uint32_t *out_object_id,
                              uint32_t *out_type, char *out_name, uint32_t *out_name_len);

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

/* ===================================================================== */
/* Extents und Dateidaten (Batch 7): Logisch->Physisch-Abbildung + echtes */
/* Lesen von Dateiinhalten - NUR LESEN (Allokation/Schreiben folgt mit    */
/* dem Journal-/Write-Batch).                                             */
/* ===================================================================== */

/* Bildet logical_block (0-basierter 4-KiB-Blockindex innerhalb der
   Datei) auf einen physischen Volume-Block ab. 0 = Erfolg
   (*out_physical_block gesetzt - 0 bedeutet ein Loch/Sparse-Bereich,
   KEIN Fehler), -1 bei einem beschaedigten Extent-Baum. */
int novafs_extent_map(uint32_t object_id, uint32_t logical_block, uint32_t *out_physical_block);

/* Liest length Byte ab offset aus der Datei object_id nach out_buffer
   (gekuerzt auf die tatsaechliche Dateigroesse, siehe *out_bytes_read).
   Loecher werden als Nullen geliefert, ohne I/O. 0 = Erfolg, -1 wenn
   object_id keine Datei ist oder der Extent-/Datenbereich beschaedigt
   ist. */
int novafs_read(uint32_t object_id, uint32_t offset, uint32_t length, uint8_t *out_buffer,
                 uint32_t *out_bytes_read);

int novafs_extent_self_test(void); /* 0 = ok */

/* ===================================================================== */
/* Transaction Log + Writes (Batch 8): echte, journalisierte Aenderungen. */
/* OHNE Copy-on-Write/Snapshots (Stretch/BLOCKED - dieser Kernel hat kein */
/* Snapshot-Feature; ohne Snapshots ist ein In-Place-Update unter         */
/* Journal-Schutz genauso crash-sicher). OHNE novafs_create (Anlegen      */
/* neuer Objekte/Verzeichniseintraege) - das folgt mit dem Objekt-        */
/* Lifecycle-Batch (Create/Delete/Rename).                                */
/* ===================================================================== */

/* Beginnt eine neue Aenderung - MUSS vor novafs_write() (und jeder
   zukuenftigen schreibenden Operation) aufgerufen werden, damit
   Metadaten-Schreibvorgaenge journalisiert und bei einem Fehler per
   novafs_change_abort() zurueckgerollt werden koennen. 0 = Erfolg, -1
   wenn nicht gemountet oder das Volume read-only ist. */
int novafs_change_begin(void);

/* Schliesst die laufende Aenderung unumkehrbar ab (Generation++, Zustand
   CLEAN, Bitmap falls geaendert, Backup-Superblock aktualisiert). 0 =
   Erfolg. */
int novafs_change_commit(void);

/* Bricht die laufende Aenderung ab und stellt per Journal-Wiedergabe
   exakt den Zustand vor novafs_change_begin() wieder her. 0 = Erfolg
   (auch wenn gar keine Aenderung lief - dann ein No-op), -1 wenn die
   Wiederherstellung selbst fehlschlaegt (das Volume bleibt dann aus
   Sicherheitsgruenden im DIRTY-Zustand). */
int novafs_change_abort(void);

/* Schreibt item (NOVAFS_OBJECT_ITEM_SIZE Byte, z.B. eine per
   novafs_object_find() geholte und veraenderte Kopie) an seine Position
   im Object-Baum zurueck. 0 = Erfolg. */
int novafs_object_store(uint32_t object_id, const uint8_t *item);

/* Schreibt length Byte aus buffer ab offset in die Datei object_id -
   erfordert eine laufende Aenderung (novafs_change_begin) fuer echte
   Crash-Sicherheit. Neue Bloecke werden genullt, bestehende per Read-
   Modify-Write aktualisiert; die Dateigroesse waechst monoton. 0 =
   Erfolg, -1 wenn das Volume read-only ist, object_id keine Datei ist,
   oder ein Block nicht alloziert/geschrieben werden kann. */
int novafs_write(uint32_t object_id, uint32_t offset, uint32_t length, const uint8_t *buffer);

int novafs_write_self_test(void); /* 0 = ok */

/* ===================================================================== */
/* Objekt-Lifecycle (Batch 9): Anlegen/Loeschen/Umbenennen. Erfordert     */
/* eine laufende Aenderung (novafs_change_begin) fuer echte Crash-        */
/* Sicherheit, wie novafs_write. Zeitstempel werden bewusst NICHT         */
/* gesetzt (keine RTC-Anbindung in novafs.c verdrahtet).                 */
/* ===================================================================== */

/* Legt eine neue Datei/ein neues Verzeichnis namens name (len Byte, KEIN
   Nullterminator) unter parent_id an. Die ObjectID wird monoton vergeben
   und NIE wiederverwendet. 0 = Erfolg (*out_object_id gesetzt), -1 wenn
   read-only, der Name ungueltig ist, parent_id kein Verzeichnis ist, der
   Name bereits existiert, oder die Kapazitaet erschoepft ist. */
int novafs_create(uint32_t parent_id, uint32_t type, const char *name, uint32_t len,
                   uint32_t *out_object_id);

/* Loescht den Verzeichniseintrag name unter parent_id samt Objekt und
   (bei einer Datei) allen Extents/deren Bloecken. ObjectIDs unterhalb
   NOVAFS_FIRST_DYNAMIC_ID(256) sind geschuetzt. Ein Verzeichnis muss leer
   sein. 0 = Erfolg. */
int novafs_delete(uint32_t parent_id, const char *name, uint32_t len);

/* Benennt um bzw. verschiebt src_name unter src_parent zu dst_name unter
   dst_parent. Ein vorhandenes Ziel wird NICHT ueberschrieben;
   Verzeichnisse duerfen nicht in sich selbst oder ihre eigenen
   Nachfahren verschoben werden. 0 = Erfolg (auch wenn Quelle und Ziel
   identisch sind - dann ein No-op). */
int novafs_rename(uint32_t src_parent, const char *src_name, uint32_t src_len,
                   uint32_t dst_parent, const char *dst_name, uint32_t dst_len);

int novafs_lifecycle_self_test(void); /* 0 = ok */

/* Baut ein minimales, leeres NovaFS-Testvolume (Superblock+Bitmap+leere
   Object-/Directory-/Extent-Baum-Blaetter mit nur der Wurzel ObjectID=1,
   DIRECTORY) - faktorisiert aus novafs_lifecycle_self_test()'s eigener
   Fixture heraus, fuer Selbsttests AUSSERHALB von novafs.c, die ein
   echtes, gemountetes Volume brauchen (z.B. vfsapi.c), ohne das interne
   B+-Tree-Bootstrap-Rohformat (novafs_node_init/_seal, die privaten
   novafs_tree_*-Deskriptoren) selbst nachzubauen.

   accessor(block_number) muss einen Zeiger auf einen genullten,
   NOVAFS_BLOCK_SIZE Byte grossen, dauerhaft gueltigen Puffer fuer diesen
   Block liefern (0 bei Fehler/Tabelle voll) - exakt die Rolle, die die
   BLOCK_TRANSPORT_VIRT-Testgeraete-Callbacks des Aufrufers ebenfalls
   brauchen (siehe novafs_lifecycle_self_test's eigene
   novafs_lifecycle_test_block() als Vorbild). Diese Funktion schreibt NUR
   in den per accessor() zurueckgegebenen Speicher - kein echter
   Block-I/O (passiert erst spaeter durch novafs_probe_volume() ueber das
   vom Aufrufer registrierte Geraet).

   total_blocks: Gesamtgroesse des Volumes. obj_leaf/dir_leaf/ext_leaf:
   Blocknummern fuer die drei leeren B+-Tree-Wurzelblaetter (muessen > dem
   intern berechneten "erster freier Block" liegen, siehe
   novafs_lifecycle_self_test fuer ein funktionierendes Beispiel: 60/61/62
   bei total=64). 0 = Erfolg, -1 wenn accessor() fuer einen benoetigten
   Block 0 liefert. */
int novafs_build_test_volume(uint8_t *(*accessor)(uint32_t block_number),
                              uint32_t total_blocks, uint32_t obj_leaf,
                              uint32_t dir_leaf, uint32_t ext_leaf);

#endif
