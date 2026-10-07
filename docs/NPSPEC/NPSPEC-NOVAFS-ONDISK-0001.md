# NPSPEC-NOVAFS-ONDISK-0001 – NovaFS 1.0 On-Disk-Format, Phase 1 (Core)

## Status

Angenommen – umgesetzt (Host-Tool `tools/novafs`, Kernel `kernel/arch/x86_64/novafs32.inc`)

## Kategorie

Filesystem / Storage / On-Disk-Format

## Bezug

- `NPSPEC-NOVAFS-0001` (Architektur, §6–§15, §25, §55, §59, §60 Phase 1)
- `NPSPEC-FILESYSTEM-NAMESPACE-0002`, `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-STORAGE-DEVICE-0001`, `NPSPEC-FSSTORAGE-VOLUME-0001`, `NPSPEC-STORAGE-MOUNT-0001`

## Zweck

Diese Spezifikation legt das konkrete Byte-Layout fest, mit dem NovaFS 1.0 in
Phase 1 („NovaFS Core“) auf einem einzelnen Datenträger gespeichert wird. Sie
konkretisiert die in NPSPEC-NOVAFS-0001 als Mindestfelder beschriebenen
Strukturen, ohne deren Bedeutung zu verändern. Host-Werkzeuge und Kernel MÜSSEN
exakt dieses Layout verwenden.

Phase 1 umfasst: Superblock (primär und Backup), Free-Space-Bitmap, Object Tree,
Directory Tree, Extent Tree, CRC32C für alle Metadaten sowie lokales Lesen und
Schreiben. Copy-on-Write, Transaction Log, Checkpoints, Schutzrichtlinien,
Kompression und Verschlüsselung folgen in späteren Phasen und werden über
Feature-Flags ergänzt.

---

## 1. Grundregeln

```text
Byte-Reihenfolge:      Little Endian
Logische Blockgröße:   4096 Byte
Blockadressierung:     relativ zum Beginn der NovaFS-Partition
Block 0:               Reserved Boot Area (Nullen)
Block 1:               Primary Superblock
Letzter Block:         Backup Superblock
Prüfsummen:            CRC32C (Castagnoli, reflektiert, Polynom 0x82F63B78,
                       Startwert 0xFFFFFFFF, Endwert XOR 0xFFFFFFFF)
```

Jedes 32-Byte-Prüfsummenfeld enthält in Phase 1 den CRC32C in den ersten vier
Bytes; die übrigen 28 Bytes MÜSSEN 0 sein. Die Prüfsumme wird immer über die
gesamte Struktur berechnet, während das Prüfsummenfeld selbst mit Nullen belegt
ist.

## 2. GPT-Einbindung

Ein NovaFS-Volume liegt in einer eigenen GPT-Partition.

```text
Partition Type GUID:   4E4F5641-4653-5359-5354-454D30303031   ("NOVA FS SYSTEM0001")
Unique Partition GUID: identisch mit filesystem_uuid des Superblocks
Partitionsname:        "NovaOS System"
Ausrichtung:           Start auf 1-MiB-Grenze (LBA mod 2048 = 0)
```

Die Typ-GUID kennzeichnet nur den Inhalt. Die stabile Volume-Identität ist
ausschließlich `filesystem_uuid` (VolumeID). Partitionsnummer, LBA und
Gerätepfad sind keine Identität.

## 3. Datenträgerlayout Phase 1

```text
Block 0                         Reserved Boot Area
Block 1                         Primary Superblock
Block 2 … 2+B-1                 Free-Space-Bitmap (B Blöcke)
Block 2+B … 2+B+J-1             Transaction-Log-Region (J Blöcke, siehe §9)
ab Block 2+B+J                  Tree-Knoten und Daten-Extents (frei vergeben)
Block total_blocks-1            Backup Superblock
```

`B = ceil(total_blocks / 32768)`. `J = 1 + NOVAFS_JOURNAL_SLOTS` (Phase 2:
`J = 17`, siehe §9) ist für alle Volumes fest und hängt nicht von
`total_blocks` ab. Bit `n` (Byte `n/8`, Bit `n%8`) beschreibt Block `n`; 1
bedeutet belegt. Blöcke 0, 1, die Bitmap selbst, die Transaction-Log-Region
und der Backup-Superblock sind immer belegt. Bits jenseits von `total_blocks`
MÜSSEN 1 sein.

## 4. Superblock

Der Superblock belegt einen vollständigen Block. Die Felder 0–383 entsprechen
`novafs_superblock_t` aus NPSPEC-NOVAFS-0001 §8 in unveränderter Reihenfolge.

| Offset | Typ | Feld | Phase-1-Belegung |
|---:|---|---|---|
| 0 | u8[8] | magic | `NOVAFS\x01\x00` |
| 8 | u16 | version_major | 1 |
| 10 | u16 | version_minor | 0 |
| 12 | u32 | block_size | 4096 |
| 16 | u64 | filesystem_size | total_blocks × 4096 |
| 24 | u64 | total_blocks | Partitionsgröße in Blöcken |
| 32 | u64 | available_blocks | freie Blöcke laut Bitmap |
| 40 | u64 | generation | +1 bei jeder abgeschlossenen Änderung |
| 48 | u64 | root_tree_block | 0 (Phase 2) |
| 56 | u64 | object_tree_block | Wurzelknoten Object Tree |
| 64 | u64 | directory_tree_block | Wurzelknoten Directory Tree |
| 72 | u64 | extent_tree_block | Wurzelknoten Extent Tree |
| 80 | u64 | policy_tree_block | 0 (Phase 3) |
| 88 | u64 | checksum_tree_block | 0 (Phase 2) |
| 96 | u64 | free_space_tree_block | erster Bitmap-Block (siehe Incompat-Flag) |
| 104 | u64 | snapshot_tree_block | 0 (Phase 5) |
| 112 | u64 | transaction_log_block | Beginn der Journal-Region, `2+B` (§9) |
| 120 | u64 | feature_flags | 0 |
| 128 | u64 | incompat_flags | `FREE_SPACE_BITMAP \| FIXED_ITEM_TREES` = 3 |
| 136 | u64 | readonly_compat_flags | 0 |
| 144 | u8[16] | filesystem_uuid | VolumeID |
| 160 | u8[16] | pool_uuid | 0 (Single-Device-Modus) |
| 176 | u8[16] | device_uuid | 0 (Phase 4) |
| 192 | u8[128] | volume_name | UTF-8, mit Nullen aufgefüllt |
| 320 | u8[32] | public_trust_anchor_hash | 0 (Phase 5) |
| 352 | u8[32] | checksum | CRC32C über den gesamten Block |
| 384 | u32 | extension_size | 128 |
| 388 | u32 | state | `novafs_state_t` (CLEAN = 0, DIRTY = 1, …) |
| 392 | u64 | next_object_id | nächste nie vergebene ObjectID |
| 400 | u64 | bitmap_blocks | B |
| 408 | u64 | object_count | Anzahl lebender Objekte |
| 416 | u64 | backup_superblock_block | total_blocks − 1 |
| 424 | u32 | bitmap_crc32c | CRC32C über alle B Bitmap-Blöcke |
| 428 | u32 | checksum_type | 1 = CRC32C |
| 432 | u64 | mount_count | +1 bei jedem Read-Write-Mount |
| 440 | – | reserviert | 0 bis Blockende |

### Flags

```text
incompat_flags
  bit 0  FREE_SPACE_BITMAP   free_space_tree_block zeigt auf eine Bitmap statt
                             auf einen Free Space Tree
  bit 1  FIXED_ITEM_TREES    alle Bäume verwenden Knoten mit festen Itemgrößen
                             gemäß §5
```

Unbekannte `incompat_flags` verhindern jedes Mounten. Unbekannte
`readonly_compat_flags` erlauben ausschließlich Read-only (NPSPEC-NOVAFS-0001 §59).

### Auswahl beim Mounten

1. Primären Superblock (Block 1) und Backup (letzter Partitionsblock) lesen.
2. Jede Kopie auf Magic, Version 1.x, Blockgröße, CRC32C, `total_blocks` ≤
   Partitionsgröße und gültige Baumverweise prüfen.
3. Die gültige Kopie mit der höchsten `generation` gewinnt.
4. `state ≠ CLEAN` löst eine Journal-Wiederherstellung aus (§9). Gelingt
   sie, gilt das Volume als `CLEAN` und wird normal (read-write, sofern
   sonst zulässig) gemountet. Schlägt sie fehl oder fehlt ein gültiges
   Journal (ältere Volumes ohne Transaction-Log-Region), bleibt es
   Read-only mit Zustand `DIRTY`.

## 5. Baumknoten

Jeder Knoten belegt genau einen Block. Bytes 0–71 entsprechen
`novafs_tree_node_header_t` (NPSPEC-NOVAFS-0001 §12).

| Offset | Typ | Feld |
|---:|---|---|
| 0 | u32 | magic `NTRE` (0x4552544E) |
| 4 | u16 | level (0 = Blatt) |
| 6 | u16 | item_count |
| 8 | u64 | tree_id |
| 16 | u64 | block_id (eigene Blocknummer) |
| 24 | u64 | parent_block (0 bei Wurzel) |
| 32 | u64 | generation |
| 40 | u8[32] | checksum (CRC32C über den Block) |
| 72 | u16 | item_size |
| 74 | u16 | max_items |
| 76 | u16 | key_size (8 oder 16) |
| 78 | u16 | reserviert |
| 80 | … | Items, dicht und aufsteigend nach Schlüssel sortiert |

`tree_id`: 2 = Object Tree, 3 = Directory Tree, 4 = Extent Tree.

Schlüssel bestehen aus `key1 = u64 @ Item+0` und – bei `key_size = 16` –
`key2 = u64 @ Item+8`; sonst ist `key2 = 0`. Verglichen wird vorzeichenlos,
zuerst `key1`, dann `key2`. Gleiche Schlüssel sind im Directory Tree zulässig
(Hash-Kollision) und werden dort über den Namen unterschieden.

### Innere Knoten

`level = 1`, `item_size = 24`, `max_items = 167`.

```text
Item: u64 key1, u64 key2, u64 child_block
```

Für jedes Kind `i` gilt: alle Schlüssel in Kind `i` sind ≥ `key(i)` und ≤
`key(i+1)`. Die Suche nach dem ersten Schlüssel ≥ S beginnt im letzten Kind mit
`key(i) < S` (sonst Kind 0) und setzt sich bei Bedarf im nächsten Kind fort.

Phase 1 erzeugt höchstens Baumhöhe 2 (Wurzel = Blatt oder innerer Knoten über
Blättern). Ein volles Blatt wird hälftig geteilt; ist die Wurzel ein Blatt,
entsteht dabei eine neue innere Wurzel. Leere Blätter bleiben bestehen. Ein
Leser MUSS Knoten mit `level > 1` ablehnen, bis eine spätere Version sie
definiert.

Löschen entfernt ein Item aus seinem Blatt, ohne Blätter zusammenzulegen oder
die Schlüssel der inneren Wurzel anzupassen: die verbleibenden Items bleiben
≥ `key(i)`, die Einträge der inneren Wurzel sind damit weiterhin gültige
Untergrenzen. Ein Blatt mit `item_count = 0` ist zulässig; Leser überspringen
es, Schreiber dürfen es wieder befüllen. Eine innere Wurzel behält mindestens
ein Kind, die Baumhöhe sinkt in Phase 1 nicht.

### Object Tree (tree_id 2)

`item_size = 152`, `max_items = 26`, `key_size = 8`, Schlüssel = ObjectID.
Das Item ist exakt `novafs_object_record_t` (NPSPEC-NOVAFS-0001 §11):

| Offset | Typ | Feld |
|---:|---|---|
| 0 | u64 | object_id |
| 8 | u64 | parent_id |
| 16 | u32 | object_type (1 = Datei, 2 = Verzeichnis) |
| 20 | u32 | flags |
| 24 | u64 | logical_size |
| 32 | u64 | allocated_size |
| 40 | u64 ×4 | created/modified/accessed/changed_time (Phase 1: 0, keine Uhr) |
| 72 | u32 | owner_id |
| 76 | u32 | group_id |
| 80 | u32 | permissions |
| 84 | u32 | link_count |
| 88 | u64 | extent_root (0 = globaler Extent Tree) |
| 96 | u64 | attribute_root (0) |
| 104 | u64 | protection_policy_id (0 = Profil „Unprotected“) |
| 112 | u64 | generation (Superblock-Generation der letzten Änderung) |
| 120 | u8[32] | checksum (CRC32C über das Item) |

Das entspricht dem natürlichen C-Layout von `novafs_object_record_t`
(`sizeof = 152`).

Object-Flags:

```text
bit 0  SYSTEM       Systembereich (/System, /Boot)
bit 1  NAMESPACE    stabiler Wurzel-Namespace (1–255)
```

### Directory Tree (tree_id 3)

`item_size = 288`, `max_items = 13`, `key_size = 16`,
Schlüssel = (parent_id, name_hash).

| Offset | Typ | Feld |
|---:|---|---|
| 0 | u64 | parent_id |
| 8 | u64 | name_hash = CRC32C(name), nullerweitert |
| 16 | u64 | object_id |
| 24 | u32 | object_type |
| 28 | u16 | name_length (1–255) |
| 30 | u16 | flags |
| 32 | u8[256] | name, UTF-8, mit Nullen aufgefüllt |

Namen sind case-sensitive, dürfen weder `/` noch NUL enthalten und dürfen nicht
`.` oder `..` sein. Die NFC-Normalisierung (NPSPEC-NOVAFS-0001 §13) übernimmt der
schreibende Host- bzw. Userspace-Pfad; der Kernel speichert die Bytes
unverändert.

### Extent Tree (tree_id 4)

`item_size = 72`, `max_items = 55`, `key_size = 16`,
Schlüssel = (object_id, logical_offset).

| Offset | Typ | Feld |
|---:|---|---|
| 0 | u64 | object_id |
| 8 | u64 | logical_offset (Vielfaches von 4096) |
| 16 | u64 | physical_block |
| 24 | u64 | block_count |
| 32 | u64 | uncompressed_size |
| 40 | u64 | stored_size |
| 48 | u64 | stripe_id (0) |
| 56 | u64 | generation |
| 64 | u32 | flags |
| 68 | u16 | compression_type (0 = NONE) |
| 70 | u16 | protection_fragment_index (0) |

Bytes 8–71 entsprechen `novafs_extent_record_t` (§14). Nicht abgedeckte
Bereiche innerhalb von `logical_size` sind Löcher und lesen sich als Nullen.

## 6. Objektidentität und Grundlayout

- ObjectID 0 ist ungültig, ObjectID 1 ist das Root-Verzeichnis `/`.
- ObjectIDs 1–255 sind für stabile Wurzel-Namespaces reserviert.
- ObjectIDs werden aus `next_object_id` vergeben und nie wiederverwendet.
- Ein neu formatiertes Systemvolume enthält:

```text
1  /            Verzeichnis
2  /System      Verzeichnis, SYSTEM | NAMESPACE
3  /Benutzer    Verzeichnis, NAMESPACE
4  /Apps        Verzeichnis, NAMESPACE
5  /Volumes     Verzeichnis, NAMESPACE
6  /Boot        Verzeichnis, SYSTEM | NAMESPACE
7  /Solutions   Verzeichnis, NAMESPACE
next_object_id = 256
```

Die ObjectIDs 1–7 entsprechen den stabilen Objekten des Kernel-Semantic-Core
(`OBJI:1` … `OBJI:7`); der Kernel prüft diese Zuordnung beim Mount.

## 7. Schreibreihenfolge Phase 1

Phase 1 schreibt ohne Copy-on-Write direkt an Ort und Stelle. Jede
Änderungsoperation läuft deshalb in dieser Reihenfolge ab:

```text
1. Superblock state = DIRTY schreiben (nur beim Übergang CLEAN → DIRTY)
2. Datenblöcke schreiben
3. geänderte Baumknoten schreiben
4. Bitmap schreiben, bitmap_crc32c aktualisieren
5. generation + 1, state = CLEAN, primären Superblock schreiben
6. Backup-Superblock schreiben
```

Bricht der Vorgang ab, bleibt `state = DIRTY` sichtbar. Ein unvollständiger
Vorgang darf nie als sauberer Zustand erscheinen. Seit Phase 2 (§9) wird
dieser Zustand beim nächsten Mount – oder noch in derselben Sitzung, wenn
die auslösende Operation selbst fehlschlägt, ohne dass der Kernel neu
startet – über das Transaction Log kontrolliert zurückgerollt, statt das
Volume dauerhaft read-only zu belassen.

## 7a. Löschen und Umbenennen

Löschen einer Datei (Kernel und Host-Werkzeug, gleiche Reihenfolge):

```text
1. für jedes Extent des Objekts: Extent-Item entfernen, dann dessen Blöcke in
   der Bitmap freigeben (die Bitmap wird erst beim Abschluss geschrieben)
2. Objekt-Item entfernen
3. Verzeichniseintrag entfernen, object_count − 1
```

- Verzeichnisse lassen sich nur löschen, wenn sie keinen Eintrag mehr haben.
- Objekte mit ObjectID < 256 (stabile Namespaces) sind weder lösch- noch
  umbenennbar.
- Gelöschte ObjectIDs werden nicht wiederverwendet (`next_object_id` bleibt).

Umbenennen bzw. Verschieben:

```text
1. Zielname prüfen (§5), Ziel muss ein Verzeichnis sein und darf nicht das
   Objekt selbst oder einer seiner Nachfahren sein
2. existiert der Zielname bereits, wird abgebrochen (kein Überschreiben)
3. neuen Verzeichniseintrag einfügen, alten entfernen
4. bei anderem Elternverzeichnis parent_id im Objekt-Item aktualisieren
```

Ein Prüfer MUSS verlangen, dass `parent_id` jedes Objekts mit dem `parent_id`
seines einzigen Verzeichniseintrags übereinstimmt.

## 8. Grenzen der Phase-1-Implementierung

- höchstens Baumhöhe 2 (≈ 4.300 Objekte, ≈ 2.100 Verzeichniseinträge,
  ≈ 9.000 Extents pro Volume),
- Kernel: höchstens 4 Bitmap-Blöcke (Volumes ≤ 512 MiB) und Dateigrößen
  < 4 GiB,
- keine Nutzdatenprüfsummen, keine Zeitstempel, kein Zusammenlegen leerer
  Blätter (siehe §5),
- kein Locking: der Kernel ruft NovaFS bislang ausschließlich aus dem
  Single-Threaded-Bootpfad auf.

## 9. Transaction Log und Crash Recovery (Phase 2)

Phase 1 schreibt in-place (§7); ein Absturz mitten in einer Änderung kann
Knoten, Bitmap-Blöcke oder den Superblock selbst nur teilweise aktualisiert
hinterlassen, während die noch gültige On-Disk-Kopie des Superblocks (primär,
mit `state = DIRTY`) weiterhin auf die *alten* Baumwurzeln zeigt. Ohne
weitere Vorsorge könnten genau die Blöcke, auf die diese alten Wurzeln
verweisen, durch den abgebrochenen Vorgang bereits teilweise überschrieben
worden sein. Phase 2 schließt diese Lücke mit einem Undo-Journal: vor jedem
Schreiben eines Blocks innerhalb einer laufenden Änderung wird dessen
*aktueller* Inhalt (das Pre-Image) gesichert. Bricht die Änderung ab, stellt
die Wiederherstellung exakt den Zustand vor ihrem Beginn wieder her – eine
Änderungsoperation ist damit atomar: entweder vollständig sichtbar oder
vollständig unsichtbar, nie halb.

### Journal-Region

Die Journal-Region (§3) beginnt bei `transaction_log_block` (Superblock-Feld,
Offset 112) und umfasst `J = 1 + NOVAFS_JOURNAL_SLOTS` Blöcke, Phase 2:
`NOVAFS_JOURNAL_SLOTS = 16`, also `J = 17`. Block 0 der Region ist der
Journal-Header, Blöcke 1…16 sind Daten-Slots (je ein vollständiges 4096-Byte
Pre-Image).

Journal-Header (ein Block):

| Offset | Typ | Feld |
|---:|---|---|
| 0 | u8[4] | magic `NJRN` |
| 4 | u32 | version (1) |
| 8 | u64 | generation (Generation des Primär-Superblocks bei Transaktionsbeginn) |
| 16 | u32 | entry_count (0…16) |
| 24 | u8[32] | checksum (CRC32C über den Block) |
| 56 | u32[16] | entries: betroffene Blocknummern, Index = Slot-Index |

### Schreibreihenfolge mit Journal

Jeder Block, der während einer laufenden Änderung (zwischen
`novafs_change_begin` und `novafs_change_commit`, siehe §7) zum ersten Mal in
dieser Transaktion beschrieben wird, wird zuerst journalisiert:

```text
1. aktuellen (alten) Inhalt des Zielblocks lesen
2. Inhalt in Journal-Daten-Slot entry_count schreiben
3. Zielblocknummer in entries[entry_count] eintragen, entry_count + 1,
   Header versiegeln und schreiben, Flush
4. erst jetzt den eigentlichen (neuen) Inhalt in den Zielblock schreiben
```

Ein Block wird pro Transaktion höchstens einmal journalisiert (weitere
Schreibzugriffe auf denselben Block in derselben Transaktion werden nicht
erneut gesichert). Damit beschreiben die gesammelten Pre-Images gemeinsam
genau den Zustand des Volumes unmittelbar vor der Transaktion – einschließlich
des Superblocks selbst, dessen `state = DIRTY`-Schreiben (§7, Schritt 1) der
allererste journalisierte Schreibzugriff jeder Transaktion ist. Übersteigt
eine Transaktion die `NOVAFS_JOURNAL_SLOTS` Slots, MUSS sie abgebrochen
werden, bevor ein ungesicherter Block geschrieben wird.

Der abschließende Schreibzugriff auf den primären Superblock mit
`state = CLEAN` (§7, Schritt 5) wird noch journalisiert (dedupliziert sich
in aller Regel mit dem Eintrag aus Schritt 1); danach ist die Transaktion
abgeschlossen. Der Backup-Superblock (§7, Schritt 6) wird **nicht**
journalisiert: wird er bei einem Absturz nicht vollständig geschrieben,
zeigt der primäre Superblock zu diesem Zeitpunkt bereits `state = CLEAN`,
sodass eine Wiederherstellung über das Journal gar nicht erst ausgelöst wird.

### Wiederherstellung (Undo)

Liest ein Mount `state = DIRTY`, UND ist eine gültige Journal-Region
vorhanden (Magic, Version, CRC32C, `generation` identisch mit der
`generation` des gelesenen – weiterhin `DIRTY` – Superblocks), läuft die
Wiederherstellung wie folgt:

```text
1. für i = entry_count-1 … 0: Inhalt von Daten-Slot i unverändert in den
   Block entries[i] zurückschreiben
2. Flush
3. entry_count = 0, Header neu versiegeln und schreiben (Journal leeren)
4. primären Superblock neu von Platte lesen (jetzt wieder im Zustand vor
   der abgebrochenen Transaktion, state = CLEAN)
```

Die Reihenfolge innerhalb der Wiederherstellung selbst ist irrelevant, da
jeder Slot einen eigenständigen, unabhängigen Block beschreibt. Nach
erfolgreicher Wiederherstellung ist das Volume exakt in dem Zustand, in dem
es vor der abgebrochenen Transaktion war, und wird normal weiterverarbeitet
(§4, Abschnitt „Auswahl beim Mounten“). Schlägt die Wiederherstellung fehl
(fehlendes oder beschädigtes Journal, falsche `generation` – etwa bei einem
Volume ohne Transaction-Log-Region aus der Zeit vor Phase 2), bleibt es beim
Phase-1-Verhalten: Read-only-Mount mit sichtbarem `state = DIRTY`.

Scheitert eine Änderungsoperation, ohne dass der Kernel neu startet (z. B.
ein ungültiger Pfad mitten in `VFS.Rename`), wird dieselbe
Wiederherstellung sofort angewendet (`novafs_change_abort`) – das Volume
bleibt beschreibbar, statt für den Rest des Boots read-only zu werden.

## Normative Anforderungen

1. NovaFS 1.0 MUSS 4096-Byte-Blöcke und Little Endian verwenden.
2. Superblock-Felder 0–383 MÜSSEN der Reihenfolge aus NPSPEC-NOVAFS-0001 §8 entsprechen.
3. Jeder Superblock und jeder Baumknoten MUSS eine gültige CRC32C-Prüfsumme besitzen.
4. Ein Mount MUSS die gültige Superblock-Kopie mit der höchsten Generation wählen.
5. Unbekannte `incompat_flags` MÜSSEN das Mounten verhindern.
6. Ein Volume mit `state ≠ CLEAN` DARF NICHT read-write gemountet werden,
   es sei denn, eine Journal-Wiederherstellung (§9) hat es zuvor
   nachweislich auf `state = CLEAN` zurückgeführt.
7. Dateinamen MÜSSEN ausschließlich im Directory Tree gespeichert werden.
8. ObjectIDs DÜRFEN NICHT wiederverwendet werden.
9. Die Partition-Typ-GUID DARF NICHT als Volume-Identität verwendet werden.
10. Leser MÜSSEN Baumknoten mit unbekanntem Level, falscher Itemgröße oder falscher `tree_id` ablehnen.
11. Jedes Objekt außer `/` MUSS genau einen Verzeichniseintrag besitzen, dessen `parent_id` dem `parent_id` des Objekts entspricht.
12. Leser MÜSSEN Blätter mit `item_count = 0` akzeptieren.
13. Jeder Block, der innerhalb einer laufenden Änderung zum ersten Mal
    beschrieben wird, MUSS vorher mit seinem alten Inhalt im Transaction
    Log (§9) gesichert werden.
14. Eine Wiederherstellung MUSS entweder den Zustand exakt vor der
    abgebrochenen Transaktion wiederherstellen oder das Volume read-only
    mit `state = DIRTY` belassen; ein teilweise wiederhergestelltes Volume
    DARF NICHT read-write gemountet werden.
