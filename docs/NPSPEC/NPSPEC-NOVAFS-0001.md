# NPSPEC-NOVAFS-0001

## NovaFS – Distributed, Resilient and Secure Filesystem Framework

**Status:** Angenommen  
**Version:** 1.0.0  
**Kategorie:** Filesystem / Storage / Security / Cluster  
**Zielsystem:** Nova OS  
**Dokument-ID:** NPSPEC-NOVAFS-0001  

---

## 1. Zweck

Diese Spezifikation definiert die Architektur, das On-Disk-Modell, die Schutzmechanismen, die Clusterfähigkeit, den Netzwerkzugriff, die Systemintegrität und die Recovery-Funktionen des Nova File System, kurz **NovaFS**.

NovaFS ist als natives Dateisystem von Nova OS vorgesehen. Es SOLL sowohl auf einzelnen Datenträgern als auch in lokalen Multi-Disk-Pools und verteilten Storage-Clustern betrieben werden können.

NovaFS MUSS folgende Hauptziele erfüllen:

- hohe Lese- und Schreibleistung,
- möglichst geringe Speichermehrkosten,
- Schutz gegen teilweise Datenträgerschäden,
- frei wählbare Schutzstufe pro Datei,
- atomare und transaktionale Änderungen,
- unveränderliche und kryptographisch verifizierte Systembereiche,
- Live-Systemupdates ohne zwingenden Neustart,
- Recovery ohne Abhängigkeit vom aktiven System,
- Betrieb mit und ohne TPM,
- lokaler und netzwerkbasierter Zugriff,
- Erweiterbarkeit für zukünftige Nova-Storage-Funktionen.

---

## 2. Normative Begriffe

Die Schlüsselwörter **MUSS**, **DARF NICHT**, **SOLL**, **SOLL NICHT**, **KANN** und **OPTIONAL** sind normativ zu verstehen.

- **MUSS:** zwingende Anforderung,
- **DARF NICHT:** ausdrücklich verboten,
- **SOLL:** dringend empfohlene Anforderung,
- **SOLL NICHT:** nur in begründeten Ausnahmefällen,
- **KANN:** optionale Funktion.

---

## 3. Grundprinzipien

NovaFS basiert auf folgenden Prinzipien:

1. extentbasierte Datenspeicherung,
2. transaktionale Metadatenänderungen,
3. Copy-on-Write für geschützte Strukturen,
4. Prüfsummen für Metadaten und Nutzdaten,
5. Schutzrichtlinien pro Datei,
6. zonen- und gerätebewusste Datenverteilung,
7. optionale Erasure-Coding-Verfahren,
8. unveränderliche Systemgenerationen,
9. native Snapshot- und Recovery-Unterstützung,
10. Trennung von lokalem Dateisystem, Storage-Pool und Netzwerkprotokoll.

NovaFS DARF nicht als monolithische Komponente implementiert werden. Die Kernarchitektur MUSS in klar getrennte Schichten gegliedert sein.

---

## 4. Architekturübersicht

```text
NovaFS

├── Block Device Layer
├── Zone and Failure Domain Layer
├── Storage Pool Layer
├── Allocation Layer
├── Data Protection Layer
├── Tree and Metadata Layer
├── Transaction Layer
├── Namespace Layer
├── Security and Trust Layer
├── Snapshot and Generation Layer
├── Recovery Layer
├── Cluster Layer
└── Network Access Layer
```

Jede Schicht MUSS über eine definierte interne Schnittstelle angesprochen werden.

---

## 5. Betriebsarten

NovaFS MUSS mindestens folgende Betriebsarten unterstützen:

### 5.1 Single-Device-Modus

Ein einzelner physischer oder virtueller Datenträger.

### 5.2 Local-Pool-Modus

Mehrere Datenträger in einem Rechner werden zu einem gemeinsamen Pool verbunden.

### 5.3 Cluster-Modus

Mehrere Rechner stellen Datenträger und Speicherbereiche in einem gemeinsamen NovaFS-Cluster bereit.

### 5.4 Network-Client-Modus

Ein Client greift über das native NovaFS-Netzwerkprotokoll auf einen entfernten NovaFS-Pool oder Cluster zu.

---

## 6. Logische Blockgröße

Die Standardblockgröße von NovaFS beträgt:

```text
4096 Byte
```

NovaFS 1.0 MUSS 4096-Byte-Blöcke unterstützen.

Weitere Blockgrößen KÖNNEN später ergänzt werden. Ein Volume MUSS seine logische Blockgröße im Superblock speichern.

---

## 7. Datenträgerlayout

Ein NovaFS-Datenträger SOLL mindestens folgende Bereiche enthalten:

```text
+--------------------------------------------------+
| Reserved Boot Area                               |
+--------------------------------------------------+
| Primary Superblock                               |
+--------------------------------------------------+
| Security Anchor Area                             |
+--------------------------------------------------+
| Checkpoint Area                                  |
+--------------------------------------------------+
| Transaction Log                                  |
+--------------------------------------------------+
| Root Tree                                        |
+--------------------------------------------------+
| Object Tree                                      |
+--------------------------------------------------+
| Directory Tree                                   |
+--------------------------------------------------+
| Extent Tree                                      |
+--------------------------------------------------+
| Protection Policy Tree                           |
+--------------------------------------------------+
| Checksum Tree                                    |
+--------------------------------------------------+
| Free Space Tree                                  |
+--------------------------------------------------+
| Snapshot and Generation Tree                     |
+--------------------------------------------------+
| Data and Parity Extents                          |
+--------------------------------------------------+
| Backup Superblock                                |
+--------------------------------------------------+
```

Nur Bootbereich, Superblock-Kopien, Security Anchor und definierte Checkpoint-Slots SOLLEN feste Positionen verwenden.

---

## 8. Superblock

Der Superblock MUSS mindestens folgende Felder enthalten:

```c
typedef struct
{
    uint8_t  magic[8];

    uint16_t version_major;
    uint16_t version_minor;

    uint32_t block_size;

    uint64_t filesystem_size;
    uint64_t total_blocks;
    uint64_t available_blocks;

    uint64_t generation;

    uint64_t root_tree_block;
    uint64_t object_tree_block;
    uint64_t directory_tree_block;
    uint64_t extent_tree_block;
    uint64_t policy_tree_block;
    uint64_t checksum_tree_block;
    uint64_t free_space_tree_block;
    uint64_t snapshot_tree_block;
    uint64_t transaction_log_block;

    uint64_t feature_flags;
    uint64_t incompat_flags;
    uint64_t readonly_compat_flags;

    uint8_t  filesystem_uuid[16];
    uint8_t  pool_uuid[16];
    uint8_t  device_uuid[16];

    uint8_t  volume_name[128];

    uint8_t  public_trust_anchor_hash[32];
    uint8_t  checksum[32];

} novafs_superblock_t;
```

Die Magic Number SOLL lauten:

```text
NOVAFS\x01\x00
```

---

## 9. Superblock-Replikation

NovaFS MUSS mehrere Superblock-Kopien unterstützen.

Mindestens drei Kopien SOLLEN an voneinander entfernten Positionen gespeichert werden:

- am Anfang,
- im mittleren Bereich,
- am Ende des Datenträgers.

Jede Kopie MUSS eine Generation und eine Prüfsumme besitzen.

Beim Mounten MUSS NovaFS die gültige Kopie mit der höchsten vollständig bestätigten Generation auswählen.

---

## 10. Objektmodell

Jede persistente NovaFS-Ressource wird als Objekt dargestellt.

```c
typedef uint64_t novafs_object_id_t;

typedef enum
{
    NOVAFS_OBJECT_UNKNOWN = 0,
    NOVAFS_OBJECT_FILE,
    NOVAFS_OBJECT_DIRECTORY,
    NOVAFS_OBJECT_SYMLINK,
    NOVAFS_OBJECT_DEVICE,
    NOVAFS_OBJECT_STREAM,
    NOVAFS_OBJECT_SNAPSHOT,
    NOVAFS_OBJECT_SUBVOLUME,
    NOVAFS_OBJECT_SYSTEM_GENERATION,
    NOVAFS_OBJECT_RECOVERY_IMAGE,
    NOVAFS_OBJECT_POLICY
} novafs_object_type_t;
```

Objekt-ID 1 SOLL für das Root-Verzeichnis reserviert werden.

---

## 11. Object Record

```c
typedef struct
{
    uint64_t object_id;
    uint64_t parent_id;

    uint32_t object_type;
    uint32_t flags;

    uint64_t logical_size;
    uint64_t allocated_size;

    uint64_t created_time;
    uint64_t modified_time;
    uint64_t accessed_time;
    uint64_t changed_time;

    uint32_t owner_id;
    uint32_t group_id;
    uint32_t permissions;
    uint32_t link_count;

    uint64_t extent_root;
    uint64_t attribute_root;
    uint64_t protection_policy_id;

    uint64_t generation;

    uint8_t checksum[32];

} novafs_object_record_t;
```

Dateinamen DÜRFEN NICHT direkt im Object Record gespeichert werden. Sie MÜSSEN im Directory Tree liegen.

---

## 12. B+-Tree-Struktur

NovaFS SOLL Copy-on-Write-B+-Trees verwenden.

Jeder Baumknoten SOLL genau einen logischen Block belegen.

```c
typedef struct
{
    uint32_t magic;
    uint16_t level;
    uint16_t item_count;

    uint64_t tree_id;
    uint64_t block_id;
    uint64_t parent_block;
    uint64_t generation;

    uint8_t checksum[32];

} novafs_tree_node_header_t;
```

Mindestens folgende Bäume MÜSSEN vorgesehen werden:

- Root Tree,
- Object Tree,
- Directory Tree,
- Extent Tree,
- Free Space Tree,
- Checksum Tree,
- Protection Policy Tree,
- Snapshot and Generation Tree,
- Cluster Metadata Tree.

---

## 13. Dateinamen und Unicode

NovaFS MUSS UTF-8-Dateinamen unterstützen.

Dateinamen SOLLEN vor dem Speichern in Unicode NFC normalisiert werden.

Die Standardbetriebsart MUSS case-sensitive sein.

Verzeichnisse KÖNNEN ein optionales Case-Insensitive-Flag besitzen.

Ein Dateiname SOLL maximal 255 UTF-8-Bytes lang sein.

---

## 14. Extents

Dateidaten MÜSSEN extentbasiert gespeichert werden.

```c
typedef struct
{
    uint64_t logical_offset;
    uint64_t physical_block;
    uint64_t block_count;

    uint64_t uncompressed_size;
    uint64_t stored_size;

    uint64_t stripe_id;
    uint64_t generation;

    uint32_t flags;
    uint16_t compression_type;
    uint16_t protection_fragment_index;

} novafs_extent_record_t;
```

Ein Extent MUSS einer konkreten Schutzrichtlinie und einem Stripe zugeordnet werden können.

---

## 15. Inline-Daten und kleine Dateien

Kleine Dateien KÖNNEN direkt in Metadatenblöcken gespeichert werden.

Die Standardgrenze SOLL 1024 Byte betragen.

Mehrere kleine Dateien KÖNNEN alternativ in Small-File-Containern zusammengefasst werden.

Small-File-Container SOLLEN gemeinsam komprimiert und geschützt werden können.

---

## 16. Speicherzonen

Jeder Datenträger MUSS in logische Speicherzonen gegliedert werden können.

Eine Zone stellt eine Allocation- und Failure-Domain dar.

```c
typedef struct
{
    uint64_t zone_id;
    uint64_t device_id;

    uint64_t first_block;
    uint64_t block_count;
    uint64_t free_blocks;

    uint32_t zone_type;
    uint32_t health_state;

} novafs_zone_descriptor_t;
```

NovaFS MUSS Fragmente eines geschützten Stripes nach Möglichkeit auf unabhängige Zonen verteilen.

---

## 17. Failure Domains

NovaFS MUSS folgende Failure-Domain-Ebenen unterscheiden können:

1. Block,
2. Zone,
3. physischer Datenträger,
4. Controller,
5. Rechner beziehungsweise Cluster-Node,
6. Standort.

Eine Schutzrichtlinie MUSS festlegen können, welche Failure-Domains voneinander getrennt sein müssen.

---

## 18. Schutzrichtlinie pro Datei

Jede Datei MUSS eine persistente Schutzrichtlinie besitzen.

```c
typedef enum
{
    NOVAFS_PROTECTION_NONE = 0,
    NOVAFS_PROTECTION_DISTRIBUTED,
    NOVAFS_PROTECTION_MIRROR,
    NOVAFS_PROTECTION_ERASURE_CODING
} novafs_protection_type_t;
```

```c
typedef struct
{
    uint64_t policy_id;

    uint8_t protection_type;
    uint8_t data_fragments;
    uint8_t parity_fragments;
    uint8_t mirror_copies;

    uint8_t minimum_zones;
    uint8_t minimum_devices;
    uint8_t minimum_nodes;
    uint8_t reserved0;

    uint32_t fragment_size;
    uint32_t flags;

} novafs_protection_policy_t;
```

Eine Datei DARF ungeschützt, nur verteilt, gespiegelt oder mit Erasure Coding gespeichert werden.

---

## 19. Vordefinierte Schutzprofile

NovaFS SOLL mindestens folgende Profile bereitstellen:

| Profil | Verfahren | Zusatzspeicher | Toleranz |
|---|---|---:|---:|
| Unprotected | keine Redundanz | 0 % | 0 |
| Distributed | nur Zonenverteilung | 0 % | 0 |
| Efficient | EC 8+1 | 12,5 % | 1 Fragment |
| Standard | EC 8+2 | 25 % | 2 Fragmente |
| High | EC 8+3 | 37,5 % | 3 Fragmente |
| Maximum | EC 8+4 | 50 % | 4 Fragmente |
| Mirror 2 | zwei Kopien | 100 % | 1 Kopie |
| Mirror 3 | drei Kopien | 200 % | 2 Kopien |

Benutzerdefinierte Profile MÜSSEN möglich sein.

---

## 20. Vererbung von Schutzrichtlinien

Die Schutzrichtlinie MUSS in folgender Reihenfolge bestimmt werden:

```text
Volume-Standard
    ↓
Verzeichnis-Standard
    ↓
explizite Datei-Policy
    ↓
technische Extent-Policy
```

Eine neu erstellte Datei SOLL die zum Erstellungszeitpunkt gültige Policy-ID dauerhaft übernehmen.

Spätere Änderungen der Verzeichnisrichtlinie DÜRFEN bestehende Dateien nicht unbemerkt verändern.

---

## 21. Änderung einer Schutzstufe

Die Schutzstufe einer bestehenden Datei MUSS im laufenden Betrieb geändert werden können.

Die Migration MUSS transaktional erfolgen:

```text
alte Extents lesen
    ↓
neue Fragmente erzeugen
    ↓
neue Schutzstruktur schreiben
    ↓
Prüfsummen validieren
    ↓
Metadaten atomar umschalten
    ↓
alte Extents freigeben
```

Die Datei MUSS während der Migration lesbar bleiben.

---

## 22. Erasure Coding

NovaFS SOLL für platzsparende Wiederherstellung Reed-Solomon-kompatibles Erasure Coding unterstützen.

Das Standardprofil SOLL lauten:

```text
8 Datenfragmente
2 Paritätsfragmente
64 KiB Fragmentgröße
512 KiB Nutzdaten pro Stripe
640 KiB physische Speicherung
```

Normale Lesevorgänge SOLLEN ausschließlich die Datenfragmente lesen.

Paritätsfragmente SOLLEN nur bei Rekonstruktion, Scrubbing, Integritätsprüfung oder Rebalancing gelesen werden.

---

## 23. XOR-Schnellprofil

NovaFS KANN ein schnelles XOR-Profil unterstützen.

```text
8 Datenfragmente
1 XOR-Paritätsfragment
```

Dieses Profil ist besonders für leicht ersetzbare Daten geeignet.

Es DARF nur den Verlust eines beliebigen Fragments tolerieren.

---

## 24. Schreiboptimierung

Kleine Schreibvorgänge DÜRFEN NICHT zwingend sofort einen vollständigen Erasure-Coding-Stripe aktualisieren.

NovaFS SOLL ein Write-Log oder Delta-Store verwenden:

```text
kleine Änderung
    ↓
transaktionales Write-Log
    ↓
mehrere Änderungen sammeln
    ↓
Full-Stripe Write
    ↓
Parität berechnen
    ↓
Fragmente verteilen
```

Full-Stripe Writes SOLLEN bevorzugt werden.

---

## 25. Prüfsummen

Alle Metadatenblöcke MÜSSEN eine Prüfsumme besitzen.

Nutzdaten SOLLEN standardmäßig ebenfalls geprüft werden.

Empfohlene Verfahren:

- CRC32C für schnelle Fehlererkennung,
- BLAKE3 für starke Inhaltsintegrität,
- SHA-256 für kompatible kryptographische Prüfungen.

Die verwendete Methode MUSS pro Struktur oder Policy angegeben werden können.

---

## 26. Selbstheilung

Wird ein beschädigtes Fragment erkannt, MUSS NovaFS nach Möglichkeit automatisch rekonstruieren:

```text
Fragment lesen
    ↓
Prüfsumme ungültig
    ↓
gültige Daten- und Paritätsfragmente lesen
    ↓
fehlendes Fragment rekonstruieren
    ↓
Datei an Anwendung liefern
    ↓
neues Fragment in gesunder Zone speichern
```

Ein beschädigter Bereich MUSS als verdächtig oder defekt markiert werden können.

---

## 27. Scrubbing

NovaFS MUSS einen Scrubbing-Dienst unterstützen.

Scrubbing SOLL:

- Daten und Metadaten lesen,
- Prüfsummen kontrollieren,
- Redundanz prüfen,
- beschädigte Fragmente rekonstruieren,
- schwache Datenträgerbereiche erkennen,
- Rebalancing auslösen,
- den Zustand protokollieren.

Scrubbing MUSS pausierbar und priorisierbar sein.

---

## 28. Lokaler Multi-Disk-Pool

Mehrere Laufwerke MÜSSEN zu einem gemeinsamen NovaFS-Pool verbunden werden können.

Ein Pool MUSS:

- unterschiedliche Laufwerksgrößen unterstützen,
- Geräte hinzufügen und entfernen können,
- Daten automatisch verteilen,
- Schutzrichtlinien über Geräte hinweg erfüllen,
- Rebalancing durchführen,
- defekte Geräte isolieren,
- Hot-Plug unterstützen, soweit die Hardware dies erlaubt.

---

## 29. Tiering

NovaFS KANN verschiedene Speicherklassen unterscheiden:

- RAM Cache,
- NVMe,
- SSD,
- HDD,
- Archivspeicher,
- Remote Storage.

Dateien und Extents KÖNNEN anhand von Richtlinien zwischen Tiers verschoben werden.

---

## 30. Clusterbetrieb

NovaFS MUSS einen verteilten Clusterbetrieb unterstützen können.

Die Clusterarchitektur SOLL aus folgenden Komponenten bestehen:

```text
NovaFS Cluster

├── Metadata Service
├── Storage Nodes
├── Coordination Service
├── Cluster Security Service
└── Client Gateway
```

Der Cluster MUSS Split-Brain-Zustände verhindern.

Metadatenänderungen SOLLEN durch Quorum bestätigt werden.

---

## 31. Cluster-Metadaten

Cluster-Metadaten MÜSSEN mindestens folgende Informationen enthalten:

- Node-ID,
- Geräte- und Zonen-IDs,
- Cluster-Generation,
- Membership-Status,
- Health-Status,
- Fragmentpositionen,
- Lease-Informationen,
- Lock-Informationen,
- Rebalancing-Zustand,
- Schutzrichtlinien,
- Zertifikate und Trust-Beziehungen.

---

## 32. Netzwerk-Dateisystem

NovaFS SOLL ein natives Netzwerkprotokoll bereitstellen.

Das native Protokoll MUSS mindestens unterstützen:

- Datei öffnen, lesen, schreiben und schließen,
- Verzeichnisse,
- Schutzrichtlinien,
- Snapshots,
- Transaktionen,
- Locks und Leases,
- Ereignisbenachrichtigungen,
- Cluster-Weiterleitungen,
- Integritätsinformationen,
- verschlüsselte Verbindungen,
- Client-Authentifizierung.

---

## 33. Kompatibilitäts-Gateways

NovaFS KANN Gateways für folgende Protokolle bereitstellen:

- SMB,
- NFS,
- WebDAV,
- S3-kompatible Objektzugriffe.

Gateways DÜRFEN keine NovaFS-Sicherheitsregeln umgehen.

---

## 34. Subvolumes

NovaFS MUSS Subvolumes unterstützen können.

Empfohlene Standardstruktur:

```text
@system
@programs
@appdata
@users
@recovery
@logs
@temporary
@snapshots
```

Subvolumes KÖNNEN eigene Schutz-, Snapshot-, Kompressions- und Sicherheitsrichtlinien besitzen.

---

## 35. Unveränderliche Systemgenerationen

Das aktive System-Subvolume MUSS unveränderlich sein.

Systemupdates DÜRFEN die aktive Generation nicht direkt verändern.

Stattdessen MUSS eine neue Generation erzeugt werden:

```text
@system/generation-42 → aktiv und read-only
@system/generation-43 → Update-Workspace
```

Nach erfolgreicher Prüfung MUSS die neue Generation atomar aktiviert werden.

---

## 36. Live-Systemupdates

NovaFS MUSS Live-Generation-Switching unterstützen.

Dabei können verschiedene Prozesse gleichzeitig unterschiedliche Systemgenerationen sehen:

```text
alter Prozess → Generation 42
neuer Prozess → Generation 43
```

Neue Prozesse SOLLEN nach der Aktivierung die neue Generation verwenden.

Alte Prozesse DÜRFEN die vorherige Generation weiterverwenden, bis sie beendet oder migriert wurden.

---

## 37. Dienstmigration

Systemdienste SOLLEN einen Upgrade-Lifecycle unterstützen:

```c
nova_result_t service_prepare_upgrade(void);
nova_result_t service_export_state(nova_state_handle_t* state);
nova_result_t service_import_state(nova_state_handle_t state);
nova_result_t service_activate(void);
nova_result_t service_retire(void);
```

NovaOS SOLL Dienste ohne Systemneustart austauschen können.

---

## 38. Treiber- und Kernelupdates

Treiber KÖNNEN live ausgetauscht werden, wenn das Gerät sicher pausiert, der neue Treiber validiert und der Zustand migriert werden kann.

Kernelkorrekturen SOLLEN über Live-Patching erfolgen.

Strukturell inkompatible Kerneländerungen KÖNNEN weiterhin einen Neustart erfordern.

NovaFS DARF keinen Neustart als Voraussetzung für normale Datei-, Dienst- oder Programmpaketupdates verlangen.

---

## 39. System Protection Domain

NovaFS MUSS geschützte Bereiche unabhängig von klassischen Benutzerrechten absichern können.

Mindestens folgende Capabilities SOLLEN definiert werden:

```text
NOVAFS_CAP_READ_SYSTEM
NOVAFS_CAP_STAGE_SYSTEM_UPDATE
NOVAFS_CAP_ACTIVATE_SYSTEM_ROOT
NOVAFS_CAP_WRITE_RECOVERY
NOVAFS_CAP_MANAGE_KEYS
NOVAFS_CAP_REPAIR_METADATA
NOVAFS_CAP_MANAGE_CLUSTER
```

Administratorrechte DÜRFEN nicht automatisch alle System-Capabilities gewähren.

---

## 40. Kryptographische Systemintegrität

Systemgenerationen MÜSSEN kryptographisch verifizierbar sein.

NovaFS SOLL hierfür einen Merkle Tree verwenden:

```text
Dateiblock-Hashes
    ↓
Datei-Hashes
    ↓
Verzeichnis-Hashes
    ↓
System-Root-Hash
    ↓
digitale Signatur
```

Eine veränderte Systemdatei MUSS beim Lesen oder spätestens bei der Generationenprüfung erkannt werden.

---

## 41. Trust-Backend-Abstraktion

NovaFS DARF kein TPM zwingend voraussetzen.

Es MUSS eine einheitliche Trust-Backend-Schnittstelle geben.

```c
typedef enum
{
    NOVA_TRUST_BACKEND_TPM,
    NOVA_TRUST_BACKEND_UEFI,
    NOVA_TRUST_BACKEND_SOFTWARE_VAULT,
    NOVA_TRUST_BACKEND_PASSWORD,
    NOVA_TRUST_BACKEND_EXTERNAL_KEY,
    NOVA_TRUST_BACKEND_NETWORK_KEY
} nova_trust_backend_type_t;
```

NovaOS SOLL automatisch das stärkste verfügbare Backend wählen.

---

## 42. Betrieb mit TPM

Wenn ein TPM vorhanden ist, KANN der Installationsschlüssel im TPM erzeugt oder daran gebunden werden.

Das TPM SOLL nur als zusätzliche Schutzschicht dienen.

Das On-Disk-Format DARF sich durch die Verwendung eines TPM nicht inkompatibel verändern.

---

## 43. Betrieb ohne TPM

Ohne TPM MUSS NovaFS vollständig funktionsfähig bleiben.

Ein Software-Vault SOLL folgende Daten aufnehmen:

- verschlüsselter Installationsschlüssel,
- öffentlicher Schlüssel,
- Zertifikatskette,
- Recovery-Metadaten,
- Sicherheitsrichtlinien.

Der private Schlüssel SOLL mit einem aus Passwort, Recovery-Key oder externem Faktor abgeleiteten Schlüssel geschützt werden.

---

## 44. Recovery-Subvolume

NovaFS MUSS ein unabhängiges Recovery-Subvolume unterstützen.

```text
@recovery
├── Recovery-Kernel
├── Recovery-System
├── Basis-Systemimage
├── Installationsmanifest
├── Zertifikatskette
├── Reparaturwerkzeuge
└── Schlüsselwiederherstellung
```

Das Recovery-Subvolume MUSS read-only, separat signiert, besonders stark geschützt und unabhängig von der aktiven Systemgeneration bootfähig sein.

---

## 45. Recovery-Ebenen

NovaFS SOLL mindestens drei Recovery-Ebenen unterstützen:

1. Rückkehr zur letzten funktionierenden Systemgeneration,
2. Wiederherstellung aus dem lokalen Recovery-Image,
3. Wiederherstellung über externes Medium oder Netzwerk.

Ein Recovery-Image auf demselben Datenträger DARF nicht als vollständiger Schutz gegen den Ausfall dieses Datenträgers dargestellt werden.

---

## 46. Snapshots

Snapshots MÜSSEN als Root-Referenzen und nicht als vollständige Kopien implementiert werden.

NovaFS SOLL spezielle Snapshot-Typen unterstützen:

- `last-known-good`,
- `before-update`,
- `factory`,
- `recovery`,
- `user-checkpoint`.

Snapshots MÜSSEN eigene Schutzrichtlinien besitzen können.

---

## 47. Transaktionen

Alle Metadatenänderungen MÜSSEN transaktional erfolgen.

```c
nova_result_t novafs_transaction_begin(
    novafs_transaction_t* transaction);

nova_result_t novafs_transaction_commit(
    novafs_transaction_t* transaction);

void novafs_transaction_abort(
    novafs_transaction_t* transaction);
```

Eine Transaktion MUSS entweder vollständig sichtbar oder vollständig unsichtbar sein.

---

## 48. Checkpoints

Ein Checkpoint beschreibt einen konsistenten globalen Zustand.

```c
typedef struct
{
    uint64_t generation;

    uint64_t root_tree;
    uint64_t object_tree;
    uint64_t extent_tree;
    uint64_t free_space_tree;
    uint64_t checksum_tree;
    uint64_t policy_tree;
    uint64_t snapshot_tree;

    uint64_t timestamp;

    uint32_t state;
    uint32_t flags;

    uint8_t checksum[32];

} novafs_checkpoint_t;
```

Beim Mounten MUSS der neueste vollständig gültige Checkpoint gewählt werden.

---

## 49. Kompression

NovaFS KANN extentbasierte Kompression unterstützen.

Mindestens folgende Modi SOLLEN vorgesehen werden:

```text
NONE
LZ4
ZSTD
```

Kompression MUSS pro Datei, Verzeichnis oder Subvolume einstellbar sein können.

---

## 50. Verschlüsselung

NovaFS SOLL mehrere Verschlüsselungsebenen unterstützen können:

1. Blockgeräteverschlüsselung,
2. Subvolume-Verschlüsselung,
3. dateibasierte Verschlüsselung.

Verschlüsselung und Schutzrichtlinie MÜSSEN voneinander unabhängig konfigurierbar sein.

---

## 51. Deduplizierung

Inline-Deduplizierung SOLL in NovaFS 1.0 nicht verpflichtend sein.

Eine spätere Offline-Deduplizierung KANN ergänzt werden.

Deduplizierung DARF weder Verschlüsselungssicherheit noch Schutzrichtlinien unbemerkt schwächen.

---

## 52. API für Schutzrichtlinien

```c
nova_result_t novafs_file_get_protection(
    novafs_handle_t file,
    novafs_protection_policy_t* policy);

nova_result_t novafs_file_set_protection(
    novafs_handle_t file,
    const novafs_protection_policy_t* policy);

nova_result_t novafs_directory_set_default_protection(
    novafs_handle_t directory,
    const novafs_protection_policy_t* policy);
```

Änderungen MÜSSEN asynchron migrierbar, aber transaktional aktivierbar sein.

---

## 53. API für Pools

```c
nova_result_t novafs_pool_create(
    const novafs_pool_create_info_t* info,
    novafs_pool_handle_t* pool);

nova_result_t novafs_pool_add_device(
    novafs_pool_handle_t pool,
    const novafs_device_info_t* device);

nova_result_t novafs_pool_remove_device(
    novafs_pool_handle_t pool,
    uint64_t device_id);

nova_result_t novafs_pool_rebalance(
    novafs_pool_handle_t pool,
    const novafs_rebalance_options_t* options);
```

---

## 54. API für Systemgenerationen

```c
nova_result_t novafs_generation_create(
    novafs_handle_t system_subvolume,
    novafs_generation_handle_t* generation);

nova_result_t novafs_generation_verify(
    novafs_generation_handle_t generation);

nova_result_t novafs_generation_activate(
    novafs_generation_handle_t generation);

nova_result_t novafs_generation_rollback(
    uint64_t target_generation);
```

---

## 55. Zustandsmodell

```c
typedef enum
{
    NOVAFS_STATE_CLEAN = 0,
    NOVAFS_STATE_DIRTY,
    NOVAFS_STATE_DEGRADED,
    NOVAFS_STATE_REBUILDING,
    NOVAFS_STATE_REBALANCING,
    NOVAFS_STATE_RECOVERY_REQUIRED,
    NOVAFS_STATE_READ_ONLY,
    NOVAFS_STATE_FAILED
} novafs_state_t;
```

NovaFS MUSS seinen Zustand eindeutig melden.

---

## 56. Diagnose

NovaFS MUSS Diagnoseinformationen bereitstellen:

- belegter und freier Speicher,
- effektiver Redundanzfaktor,
- Schutzstatus pro Datei,
- Zonen- und Gerätezustand,
- beschädigte Fragmente,
- laufende Migrationen,
- Scrubbing-Fortschritt,
- Clusterquorum,
- Generationenstatus,
- Recovery-Bereitschaft,
- Trust-Backend,
- TPM-Status,
- Signaturstatus.

---

## 57. Sicherheitsstatus

NovaFS MUSS Sicherheit transparent anzeigen.

```text
Systemintegrität:         verifiziert
Aktive Generation:        43
System-Root:              read-only
Live-Update:              aktiv
Trust Backend:            Software Vault
TPM:                      nicht vorhanden
Secure Boot:              deaktiviert
Recovery-Schlüssel:       vorhanden
Recovery-Image:           gültig
Clusterzustand:           gesund
```

NovaFS DARF nicht den Eindruck hardwaregestützter Sicherheit vermitteln, wenn nur ein Software-Backend verwendet wird.

---

## 58. Bootloader-Unterstützung

Der Nova-Bootloader benötigt nur einen eingeschränkten Read-only-Treiber.

Dieser MUSS unterstützen:

- Superblock lesen,
- Checkpoint auswählen,
- Root Tree lesen,
- Pfade auflösen,
- Extents lesen,
- Prüfsummen validieren,
- Systemgeneration auswählen,
- Recovery-Generation starten,
- optional komprimierte Bootdateien lesen.

Der Bootloader SOLL keine allgemeinen Schreiboperationen unterstützen.

---

## 59. Kompatibilität

Neue NovaFS-Versionen MÜSSEN Feature-Flags verwenden.

```text
feature_flags
incompat_flags
readonly_compat_flags
```

Unbekannte `incompat_flags` MÜSSEN das Read-Write-Mounten verhindern.

Unbekannte `readonly_compat_flags` DÜRFEN ein Read-only-Mount erlauben.

---

## 60. Implementierungsphasen

### Phase 1 – NovaFS Core

- Superblock,
- Object Tree,
- Directory Tree,
- Extents,
- Free-Space-Bitmap,
- CRC32C,
- lokales Lesen und Schreiben.

### Phase 2 – Transaktionen

- Copy-on-Write,
- Checkpoints,
- Transaction Log,
- Crash Recovery,
- mehrere Superblöcke.

### Phase 3 – Schutzrichtlinien

- Zonen,
- Mirror,
- EC 8+1,
- EC 8+2,
- Scrubbing,
- Selbstheilung.

### Phase 4 – Multi-Disk-Pool

- Geräteverwaltung,
- Failure Domains,
- Rebalancing,
- Tiering.

### Phase 5 – Systemschutz

- unveränderliche Generationen,
- Merkle Trees,
- Signaturen,
- Recovery-Subvolume,
- Software-Vault und TPM-Backend.

### Phase 6 – Cluster und Netzwerk

- Metadata Service,
- Storage Nodes,
- Quorum,
- natives Netzwerkprotokoll,
- Gateways.

---

# ADR-0001 – Extentbasierte Speicherung

## Entscheidung

NovaFS verwendet Extents statt klassischer Blocklisten.

## Begründung

Extents reduzieren Metadaten, verbessern sequentielle I/O-Leistung und vereinfachen Erasure Coding, Kompression und Rebalancing.

## Konsequenz

Die Extent-Verwaltung wird komplexer als eine einfache Blockkette, ist aber für die langfristige Architektur notwendig.

---

# ADR-0002 – Schutzrichtlinie pro Datei

## Entscheidung

Jede Datei besitzt eine eigene persistente Schutzrichtlinie.

## Begründung

Nicht alle Daten benötigen denselben Schutz. Temporäre Dateien sollen keinen unnötigen Speicher belegen, während System- und Recovery-Daten besonders stark geschützt werden müssen.

## Konsequenz

NovaFS muss Schutzmigrationen und gemischte Extent-Zustände unterstützen.

---

# ADR-0003 – Reed-Solomon als Standard-Erasure-Coding

## Entscheidung

NovaFS verwendet für den Standardschutz ein Reed-Solomon-kompatibles 8+2-Schema.

## Begründung

Das Schema bietet eine gute Balance aus 25 Prozent Zusatzspeicher, hoher Lesegeschwindigkeit und Wiederherstellung von zwei verlorenen Fragmenten.

## Konsequenz

Kleine Schreibzugriffe benötigen ein Write-Log oder Full-Stripe-Aggregation.

---

# ADR-0004 – Multi-Disk-Pool und Cluster als getrennte Ebenen

## Entscheidung

Lokale Pools und verteilte Cluster werden getrennt implementiert.

## Begründung

Ein lokaler Pool benötigt keine verteilte Konsenslogik. Clusterfunktionen dürfen den lokalen Kern nicht unnötig verkomplizieren.

## Konsequenz

Beide Ebenen teilen Schutz-, Objekt- und Policy-Modelle, besitzen aber unterschiedliche Koordinationsmechanismen.

---

# ADR-0005 – Unveränderliche Systemgenerationen

## Entscheidung

Aktive Systemgenerationen sind read-only und werden niemals direkt aktualisiert.

## Begründung

Dadurch bleiben Rollback, Integritätsprüfung und Schutz gegen Manipulation zuverlässig möglich.

## Konsequenz

Updates müssen eine neue Generation erzeugen und atomar aktivieren.

---

# ADR-0006 – Live-Update durch Namespace-Switch

## Entscheidung

NovaOS aktiviert neue Systemgenerationen durch atomare Namespace-Umschaltung.

## Begründung

So können Anwendungen, Dienste und Bibliotheken ohne vollständigen Neustart aktualisiert werden.

## Konsequenz

Alte Generationen müssen verfügbar bleiben, solange Prozesse auf sie verweisen.

---

# ADR-0007 – TPM optional

## Entscheidung

TPM-Unterstützung ist optional und wird über ein Trust-Backend abstrahiert.

## Begründung

NovaOS soll auch auf älterer Hardware vollständig funktionieren.

## Konsequenz

Ohne TPM ist die kryptographische Integrität weiterhin möglich, der Schutz privater Schlüssel gegen physische Offline-Angriffe ist jedoch schwächer.

---

# ADR-0008 – Separates Recovery-Subvolume

## Entscheidung

Recovery-Komponenten werden in einem unabhängig signierten und besonders geschützten Subvolume gespeichert.

## Begründung

Das Recovery-System darf nicht von der Integrität der aktiven Systemgeneration abhängen.

## Konsequenz

Recovery-Updates benötigen eine eigene autorisierte Transaktion und Vertrauenskette.

---

# ADR-0009 – Native Netzwerkfunktion plus Gateways

## Entscheidung

NovaFS erhält ein natives Netzwerkprotokoll und optionale Kompatibilitäts-Gateways.

## Begründung

Nur ein natives Protokoll kann alle NovaFS-Funktionen vollständig übertragen. Gateways sichern die Kompatibilität mit anderen Betriebssystemen.

## Konsequenz

Gateway-Zugriffe können nicht immer alle nativen Schutz- und Snapshot-Funktionen abbilden.

---

# ADR-0010 – Transparente Sicherheitsanzeige

## Entscheidung

NovaFS zeigt den tatsächlich erreichten Sicherheitszustand offen an.

## Begründung

Softwarebasierte und hardwarebasierte Vertrauensanker bieten unterschiedliche Schutzstärken.

## Konsequenz

Die Benutzeroberfläche muss Integrität, TPM-Status, Trust-Backend und Recovery-Bereitschaft getrennt darstellen.

---

## Schlussbestimmung

NovaFS ist ein extentbasiertes, transaktionales, zonenbewusstes, selbstheilendes und clusterfähiges Dateisystem mit frei definierbarer Schutzstufe pro Datei.

Es verbindet:

- lokale Hochleistungs-Datenspeicherung,
- platzsparende mathematische Redundanz,
- Multi-Disk-Pooling,
- verteilte Cluster,
- Netzwerkzugriff,
- unveränderliche Systemgenerationen,
- kryptographische Integrität,
- Live-Systemupdates,
- TPM-optionale Vertrauensmodelle,
- unabhängige Recovery-Funktionen.

Die Implementierung MUSS schrittweise erfolgen. Der lokale NovaFS-Kern, das On-Disk-Format und die Transaktionssicherheit haben Vorrang vor Cluster-, Gateway- und erweiterten Optimierungsfunktionen.