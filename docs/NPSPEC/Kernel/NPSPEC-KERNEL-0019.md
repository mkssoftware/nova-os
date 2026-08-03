# NPSPEC-KERNEL-0019 – Virtual File System

| Feld | Wert |
|---|---|
| Dokument-ID | NPSPEC-KERNEL-0019 |
| Titel | Virtual File System |
| Status | Spezifikation |
| Version | 1.0 |
| Datum | 2026-08-03 |
| Bereich | Kernel / Dateisystem |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | NPSPEC-KERNEL-0007, NPSPEC-KERNEL-0012, NPSPEC-KERNEL-0013, NPSPEC-KERNEL-0015, NPSPEC-KERNEL-0016, NPSPEC-KERNEL-0017 |
| Zugehörige ADRs | ADR-VFS-0001, ADR-VFS-0002, ADR-VFS-0003, ADR-VFS-0004, ADR-VFS-0005, ADR-VFS-0006, ADR-VFS-0007, ADR-VFS-0008, ADR-VFS-0009, ADR-VFS-0010 |

---

## 1. Zweck

Diese Spezifikation definiert das Virtual File System von NovaOS, im Folgenden VFS genannt.

Das VFS stellt eine einheitliche Schnittstelle für unterschiedliche Dateisysteme und dateiähnliche Objekte bereit.

Es verwaltet:

- Dateien und Verzeichnisse,
- Dateisysteminstanzen,
- Mounts und Mount Namespaces,
- Pfadauflösung,
- File Handles,
- symbolische Links,
- Attribute und ACLs,
- File Cache und Buffer Cache,
- Memory-Mapped Files,
- asynchrone I/O,
- Dateisystemereignisse.

## 2. Ziele

Das VFS muss:

- mehrere Dateisystemtypen gleichzeitig unterstützen,
- Dateisystemdetails vor Anwendungen abstrahieren,
- capability-basierte Zugriffe erzwingen,
- sichere und konsistente Pfadauflösung bereitstellen,
- Mount Namespaces und Sandboxen unterstützen,
- synchrone und asynchrone I/O ermöglichen,
- lokale und netzwerkbasierte Dateisysteme integrieren,
- Caching und Journaling-Schnittstellen bereitstellen,
- Dateisystemfehler auf geeignete Domains begrenzen.

## 3. Abgrenzung

| Komponente | Verantwortung |
|---|---|
| VFS | einheitliches Datei- und Pfadmodell |
| Dateisystemtreiber | konkrete On-Disk- oder Netzwerksemantik |
| Mount Manager | Mount-Lebenszyklus und Namespace-Zuordnung |
| File Cache | Caching dateibasierter Seiten |
| Buffer Cache | Caching von Block- und Metadatenpuffern |
| Block Layer | I/O zu Blockgeräten |
| Object Manager | Objektidentität und Namespaces |
| Handle Manager | File Handles und Rechte |
| Security Manager | ACL-, Capability- und Policy-Prüfungen |

## 4. Grundmodell

Dateisystemressourcen werden als Kernelobjekte dargestellt.

```text
Filesystem Type
  -> Filesystem Instance
      -> Mount
          -> VFS Node
              -> Open File Description
                  -> File Handle
```

Ein File Handle und ein VFS Node sind unterschiedliche Objekte.

## 5. Zentrale Objekttypen

```c
typedef enum np_vfs_object_type {
    NP_VFS_FILESYSTEM,
    NP_VFS_MOUNT,
    NP_VFS_NODE,
    NP_VFS_OPEN_FILE,
    NP_VFS_DIRECTORY_CURSOR,
    NP_VFS_WATCH
} np_vfs_object_type_t;
```

Alle dauerhaft referenzierbaren VFS-Ressourcen werden durch den Object Manager verwaltet.

## 6. Knotentypen

```c
typedef enum np_vfs_node_type {
    NP_VFS_NODE_REGULAR_FILE,
    NP_VFS_NODE_DIRECTORY,
    NP_VFS_NODE_SYMBOLIC_LINK,
    NP_VFS_NODE_BLOCK_DEVICE,
    NP_VFS_NODE_CHARACTER_DEVICE,
    NP_VFS_NODE_PIPE,
    NP_VFS_NODE_SOCKET,
    NP_VFS_NODE_MOUNT_POINT,
    NP_VFS_NODE_SPECIAL
} np_vfs_node_type_t;
```

Nicht jeder Dateisystemtyp muss alle Knotentypen unterstützen.

## 7. VFS Node

Ein VFS Node repräsentiert ein Dateisystemobjekt unabhängig von einem geöffneten Handle.

```c
typedef struct np_vfs_node {
    np_kernel_object_t object;

    np_filesystem_instance_t* filesystem;
    np_vfs_node_id_t node_id;
    np_vfs_node_type_t node_type;

    np_vfs_attributes_t attributes;
    np_vfs_node_operations_t* operations;

    np_atomic_uint64_t open_count;
    uint32_t flags;
    np_rwlock_t lock;
} np_vfs_node_t;
```

Die interne Struktur ist nicht Bestandteil der Userspace-ABI.

## 8. Node ID

```c
typedef uint64_t np_vfs_node_id_t;
```

Eine Node ID ist innerhalb einer Dateisysteminstanz eindeutig.

Für eine systemweit eindeutige Identifikation werden mindestens kombiniert:

```text
Filesystem Instance ID + Node ID + Generation
```

Die Node ID verleiht keine Zugriffsrechte.

## 9. Dateisystemtyp

```c
typedef struct np_filesystem_type {
    np_uuid_t type_id;
    const char* name;

    np_filesystem_type_operations_t operations;
    uint64_t feature_flags;
    uint32_t flags;
} np_filesystem_type_t;
```

Ein Dateisystemtyp beschreibt eine Implementierung wie:

- NovaFS,
- FAT,
- ISO 9660,
- RAMFS,
- Proc-ähnliches virtuelles Dateisystem,
- Netzwerkdateisystem,
- Recovery-Dateisystem.

## 10. Registrierung eines Dateisystemtyps

```c
np_status_t np_vfs_register_filesystem(
    const np_filesystem_type_descriptor_t* descriptor,
    np_filesystem_type_t** filesystem_type
);
```

Vor der Registrierung werden geprüft:

- eindeutige Typkennung,
- eindeutiger Name,
- ABI-Kompatibilität,
- erforderliche Operationen,
- Sicherheitsprofil,
- Modul- und Treiberlebenszeit.

Ein Typ darf nicht entfernt werden, solange aktive Instanzen bestehen.

## 11. Dateisysteminstanz

Eine Dateisysteminstanz repräsentiert ein konkret eingebundenes oder vorbereitetes Dateisystem.

```c
typedef struct np_filesystem_instance {
    np_kernel_object_t object;

    np_uuid_t instance_id;
    np_filesystem_type_t* type;

    np_vfs_node_t* root_node;
    np_block_device_t* backing_device;

    uint64_t features;
    uint32_t state;
    np_rwlock_t lock;
} np_filesystem_instance_t;
```

Mehrere Mounts können dieselbe Instanz referenzieren.

## 12. Dateisystemzustände

```c
typedef enum np_filesystem_state {
    NP_FILESYSTEM_CREATED,
    NP_FILESYSTEM_MOUNTING,
    NP_FILESYSTEM_ACTIVE,
    NP_FILESYSTEM_READ_ONLY,
    NP_FILESYSTEM_DEGRADED,
    NP_FILESYSTEM_UNMOUNTING,
    NP_FILESYSTEM_FAILED,
    NP_FILESYSTEM_UNMOUNTED
} np_filesystem_state_t;
```

Ein fehlerhaftes Dateisystem kann abhängig von der Fehlerart automatisch in den Read-only-Zustand wechseln.

## 13. VFS-Operationsschnittstelle

```c
typedef struct np_vfs_node_operations {
    np_status_t (*lookup)(
        np_vfs_node_t* directory,
        np_string_view_t name,
        np_vfs_node_t** result
    );

    np_status_t (*create)(
        np_vfs_node_t* directory,
        np_string_view_t name,
        const np_vfs_create_info_t* create_info,
        np_vfs_node_t** result
    );

    np_status_t (*read)(
        np_open_file_t* file,
        np_io_request_t* request
    );

    np_status_t (*write)(
        np_open_file_t* file,
        np_io_request_t* request
    );

    np_status_t (*remove)(
        np_vfs_node_t* directory,
        np_string_view_t name
    );
} np_vfs_node_operations_t;
```

Weitere Operationen werden über versionierte Erweiterungen bereitgestellt.

## 14. Mount

Ein Mount verbindet die Wurzel einer Dateisysteminstanz mit einem Punkt innerhalb eines Mount Namespace.

```c
typedef struct np_mount {
    np_kernel_object_t object;

    np_mount_id_t mount_id;
    np_filesystem_instance_t* filesystem;
    np_vfs_node_t* root_node;

    np_mount_t* parent_mount;
    np_vfs_node_t* mount_point;

    uint64_t flags;
    uint32_t state;
} np_mount_t;
```

Ein Mount hält Referenzen auf Dateisysteminstanz, Root Node und gegebenenfalls seinen Mount Point.

## 15. Mount-Flags

```c
typedef enum np_mount_flags {
    NP_MOUNT_READ_ONLY     = 1U << 0,
    NP_MOUNT_NO_EXEC       = 1U << 1,
    NP_MOUNT_NO_DEVICE     = 1U << 2,
    NP_MOUNT_NO_SET_ID     = 1U << 3,
    NP_MOUNT_SYNCHRONOUS   = 1U << 4,
    NP_MOUNT_NO_ACCESS_TIME = 1U << 5,
    NP_MOUNT_VERIFIED      = 1U << 6,
    NP_MOUNT_RECOVERY      = 1U << 7
} np_mount_flags_t;
```

Mount-Flags können Dateisystemrechte weiter einschränken, aber nicht unautorisiert erweitern.

## 16. Mount Namespace

Jeder Prozess besitzt eine Sicht auf einen Mount Namespace.

```c
typedef struct np_mount_namespace {
    np_kernel_object_t object;

    np_mount_t* root_mount;
    np_mount_tree_t mounts;

    uint64_t generation;
    np_rwlock_t lock;
} np_mount_namespace_t;
```

Mehrere Prozesse können denselben Namespace teilen oder eine kontrollierte Kopie erhalten.

## 17. Namespace-Isolation

Ein Mount in einem Namespace ist nicht automatisch in anderen Namespaces sichtbar.

Dies ermöglicht:

- Prozesssandboxes,
- Container,
- Recovery-Umgebungen,
- Anwendungspakete,
- eingeschränkte Dienstumgebungen,
- Live-Update-Generationen.

Capability-Prüfungen bleiben zusätzlich zum Namespace erforderlich.

## 18. Root- und Arbeitsverzeichnis

Jeder Prozess besitzt:

- ein Root-Verzeichnishandle,
- ein aktuelles Arbeitsverzeichnishandle,
- einen Mount Namespace.

Absolute Pfade werden relativ zum Prozess-Root aufgelöst.

Relative Pfade werden relativ zu einem expliziten Directory Handle oder dem aktuellen Arbeitsverzeichnis aufgelöst.

Ein Prozess darf sein Root nicht über `..` verlassen.

## 19. Pfadformat

NovaOS verwendet `/` als kanonischen Pfadtrenner.

Beispiele:

```text
/system/core/init
/users/matthias/documents
/device/storage/system
```

Pfade werden als UTF-8 übertragen.

Interne VFS-APIs bevorzugen längenbasierte Zeichenketten statt zwingend nullterminierter Puffer.

## 20. Pfadnormalisierung

Die Pfadauflösung verarbeitet:

- wiederholte Trenner,
- `.`-Segmente,
- `..`-Segmente,
- symbolische Links,
- Mount Points,
- Prozess-Root-Grenzen,
- Dateisystem-spezifische Namensregeln.

Die rein textuelle Normalisierung darf symbolische Links oder Mountgrenzen nicht sicherheitsrelevant vorwegnehmen.

Die endgültige Auflösung erfolgt segmentweise.

## 21. Sichere Pfadauflösung

```c
np_status_t np_vfs_resolve(
    np_handle_t base_directory,
    np_string_view_t path,
    uint32_t flags,
    np_vfs_path_result_t* result
);
```

Bei jedem Segment werden geprüft:

- Verzeichniszugriffsrecht,
- Namespace-Sicht,
- Mountübergang,
- symbolische Linkregeln,
- Sicherheitsrichtlinie,
- Objektlebenszeit.

Das Ergebnis hält eine gültige Referenz auf den aufgelösten Node.

## 22. Auflösungsflags

```c
typedef enum np_vfs_resolve_flags {
    NP_RESOLVE_FOLLOW_FINAL_LINK = 1U << 0,
    NP_RESOLVE_NO_SYMBOLIC_LINKS = 1U << 1,
    NP_RESOLVE_BENEATH_ROOT      = 1U << 2,
    NP_RESOLVE_NO_MOUNT_CROSSING = 1U << 3,
    NP_RESOLVE_REQUIRE_DIRECTORY = 1U << 4,
    NP_RESOLVE_REQUIRE_FILE      = 1U << 5
} np_vfs_resolve_flags_t;
```

Sicherheitskritische Dienste sollen restriktive Auflösungsflags verwenden.

## 23. Symbolische Links

Ein symbolischer Link speichert einen Zielpfad.

```c
np_status_t np_vfs_create_symbolic_link(
    np_handle_t directory,
    np_string_view_t name,
    np_string_view_t target
);
```

Die Auflösung muss:

- eine maximale Linktiefe besitzen,
- Schleifen erkennen,
- Root- und Namespace-Grenzen beachten,
- relative Ziele relativ zum Linkverzeichnis behandeln.

## 24. Hardlinks

Dateisysteme können mehrere Verzeichniseinträge auf denselben Node unterstützen.

Hardlinks auf Verzeichnisse sind über die öffentliche API standardmäßig unzulässig.

Hardlinks über verschiedene Dateisysteminstanzen hinweg sind nicht möglich.

Die endgültige Node-Freigabe erfolgt erst, wenn Linkanzahl und aktive Referenzen dies erlauben.

## 25. Öffnen einer Datei

```c
np_status_t np_file_open(
    np_handle_t directory,
    np_string_view_t path,
    uint32_t access,
    uint32_t open_flags,
    np_handle_t* file
);
```

Die Operation erzeugt eine Open File Description und ein darauf verweisendes Handle.

Pfadauflösung und Erzeugung müssen bei `CREATE` atomar gegen konkurrierende Namensoperationen erfolgen.

## 26. Zugriffsrechte

```c
typedef enum np_file_access {
    NP_FILE_READ_DATA       = 1U << 0,
    NP_FILE_WRITE_DATA      = 1U << 1,
    NP_FILE_APPEND_DATA     = 1U << 2,
    NP_FILE_READ_ATTRIBUTES = 1U << 3,
    NP_FILE_WRITE_ATTRIBUTES = 1U << 4,
    NP_FILE_EXECUTE         = 1U << 5,
    NP_FILE_DELETE          = 1U << 6,
    NP_FILE_LIST_DIRECTORY  = 1U << 7,
    NP_FILE_TRAVERSE        = 1U << 8
} np_file_access_t;
```

Die Rechte des File Handles werden beim Öffnen festgelegt und können ohne neue Autorisierung nur reduziert werden.

## 27. Open-Flags

```c
typedef enum np_file_open_flags {
    NP_FILE_OPEN_EXISTING   = 1U << 0,
    NP_FILE_CREATE          = 1U << 1,
    NP_FILE_CREATE_EXCLUSIVE = 1U << 2,
    NP_FILE_TRUNCATE        = 1U << 3,
    NP_FILE_APPEND          = 1U << 4,
    NP_FILE_DIRECTORY       = 1U << 5,
    NP_FILE_NO_FOLLOW       = 1U << 6,
    NP_FILE_NONBLOCKING     = 1U << 7,
    NP_FILE_SYNCHRONOUS     = 1U << 8,
    NP_FILE_CLOSE_ON_EXEC   = 1U << 9
} np_file_open_flags_t;
```

Widersprüchliche Flagkombinationen müssen abgelehnt werden.

## 28. Open File Description

```c
typedef struct np_open_file {
    np_kernel_object_t object;

    np_vfs_node_t* node;
    np_mount_t* mount;

    uint64_t offset;
    uint32_t access;
    uint32_t flags;

    void* filesystem_context;
    np_mutex_t lock;
} np_open_file_t;
```

Duplizierte Handles können dieselbe Open File Description und damit denselben Dateioffset teilen.

Separates Öffnen derselben Datei erzeugt unabhängige Open File Descriptions.

## 29. File Handles

File Handles werden durch den Handle Manager verwaltet.

Ein File Handle enthält logisch:

- Referenz auf eine Open File Description,
- erlaubte Rechte,
- Handle-Flags,
- Generationsschutz.

Das Schließen des letzten Handles gibt die Open File Description frei, sofern keine weiteren Referenzen bestehen.

## 30. Lesen und Schreiben

```c
np_status_t np_file_read(
    np_handle_t file,
    void* user_buffer,
    size_t buffer_size,
    size_t* bytes_read
);

np_status_t np_file_write(
    np_handle_t file,
    const void* user_buffer,
    size_t buffer_size,
    size_t* bytes_written
);
```

Partielle Erfolge müssen über die tatsächlich verarbeitete Datenmenge gemeldet werden.

Ein erfolgreicher Read mit null Byte zeigt bei regulären Dateien das Dateiende an.

## 31. Positionsbasierte I/O

```c
np_status_t np_file_read_at(
    np_handle_t file,
    uint64_t offset,
    void* user_buffer,
    size_t buffer_size,
    size_t* bytes_read
);

np_status_t np_file_write_at(
    np_handle_t file,
    uint64_t offset,
    const void* user_buffer,
    size_t buffer_size,
    size_t* bytes_written
);
```

Positionsbasierte Operationen verändern den gemeinsamen Dateioffset der Open File Description nicht.

## 32. Dateioffset

Der gemeinsame Dateioffset wird unter geeigneter Synchronisation aktualisiert.

Relative Seek-Operationen sind:

```c
typedef enum np_seek_origin {
    NP_SEEK_BEGIN,
    NP_SEEK_CURRENT,
    NP_SEEK_END,
    NP_SEEK_DATA,
    NP_SEEK_HOLE
} np_seek_origin_t;
```

Nicht alle Dateisysteme unterstützen `DATA` und `HOLE`.

## 33. Append-Semantik

Bei einem Handle im Append-Modus wird jeder Schreibvorgang atomar relativ zum aktuellen Dateiende positioniert.

Zwischen Ermittlung des Dateiende und Reservierung des Schreibbereichs darf kein konkurrierender Writer denselben Bereich erhalten.

Append garantiert nicht, dass ein großer Schreibvorgang in jeder Fehlerlage vollständig ausgeführt wird.

## 34. Verzeichnisse

Verzeichnisse werden über Directory Handles geöffnet.

```c
np_status_t np_directory_read(
    np_handle_t directory,
    np_directory_cursor_t* cursor,
    np_directory_entry_t* entries,
    size_t buffer_size,
    size_t* bytes_written
);
```

Die Verzeichnisreihenfolge ist dateisystemspezifisch und darf sich bei parallelen Änderungen verändern.

## 35. Directory Cursor

Ein Directory Cursor speichert die Fortsetzungsposition einer Aufzählung.

Er darf nicht als dauerhafte Node ID interpretiert werden.

Das Dateisystem muss erkennen können, wenn ein Cursor durch strukturelle Änderungen ungültig wurde.

In diesem Fall wird ein definierter Neustart- oder Stale-Cursor-Fehler geliefert.

## 36. Datei- und Verzeichnisoperationen

Das VFS stellt mindestens bereit:

```c
np_status_t np_file_create(...);
np_status_t np_directory_create(...);
np_status_t np_vfs_unlink(...);
np_status_t np_vfs_rename(...);
np_status_t np_vfs_link(...);
np_status_t np_vfs_symbolic_link(...);
```

Alle Namensoperationen müssen die Rechte auf dem Elternverzeichnis und gegebenenfalls auf dem Zielobjekt prüfen.

## 37. Rename

Ein Rename innerhalb derselben Dateisysteminstanz muss atomar erscheinen.

Es gilt:

- Quelle existiert vollständig am alten oder neuen Ort,
- keine halb sichtbare Zwischenform,
- Ersetzungsregeln werden geprüft,
- offene Handles bleiben gültig,
- Mount Points können nicht unkontrolliert überschrieben werden.

Cross-Filesystem-Rename liefert einen definierten Fehler und wird nicht automatisch als Copy-and-Delete ausgeführt.

## 38. Löschen

Das Entfernen eines Namenseintrags löscht nicht automatisch ein noch geöffnetes Dateiobjekt.

Die zugrunde liegenden Daten bleiben erhalten, solange:

- offene File Handles,
- Memory Mappings,
- interne Referenzen,
- weitere Hardlinks

bestehen.

Dateisysteme ohne diese Semantik müssen ihre Einschränkung ausdrücklich melden.

## 39. Dateiattribute

```c
typedef struct np_vfs_attributes {
    uint64_t size;
    uint64_t allocated_size;

    np_time_ns_t creation_time;
    np_time_ns_t modification_time;
    np_time_ns_t access_time;
    np_time_ns_t change_time;

    uint64_t flags;
    uint64_t link_count;
    np_vfs_node_type_t node_type;
} np_vfs_attributes_t;
```

Nicht unterstützte Attribute werden über Feature-Flags gekennzeichnet.

## 40. Attributflags

```c
typedef enum np_file_attribute_flags {
    NP_FILE_ATTRIBUTE_READ_ONLY = 1U << 0,
    NP_FILE_ATTRIBUTE_HIDDEN    = 1U << 1,
    NP_FILE_ATTRIBUTE_SYSTEM    = 1U << 2,
    NP_FILE_ATTRIBUTE_ARCHIVE   = 1U << 3,
    NP_FILE_ATTRIBUTE_TEMPORARY = 1U << 4,
    NP_FILE_ATTRIBUTE_SPARSE    = 1U << 5,
    NP_FILE_ATTRIBUTE_COMPRESSED = 1U << 6,
    NP_FILE_ATTRIBUTE_ENCRYPTED = 1U << 7,
    NP_FILE_ATTRIBUTE_IMMUTABLE = 1U << 8,
    NP_FILE_ATTRIBUTE_NO_DUMP   = 1U << 9
} np_file_attribute_flags_t;
```

Attribute ersetzen keine Capability- oder ACL-Prüfung.

## 41. Erweiterte Attribute

Dateisysteme können typisierte erweiterte Attribute unterstützen.

Namespaces für erweiterte Attribute können sein:

```text
user.*
system.*
security.*
trusted.*
application.*
```

Zugriffe auf `security.*` und `trusted.*` benötigen besondere Rechte.

Größe und Anzahl erweiterter Attribute müssen begrenzt sein.

## 42. ACLs

Das VFS unterstützt Access Control Lists zusätzlich zu Capabilities.

```c
typedef struct np_acl_entry {
    np_identity_id_t subject;
    np_acl_entry_type_t type;
    np_file_access_t rights;
    uint32_t inheritance_flags;
} np_acl_entry_t;
```

Die effektive Berechtigung ergibt sich aus:

- Handle-Rechten,
- Capability-Domain,
- ACL,
- Mount-Flags,
- Sandbox- und Systemrichtlinien.

Die restriktivste zwingende Grenze darf nicht überschritten werden.

## 43. ACL-Vererbung

Neue Dateien und Verzeichnisse können ACL-Einträge vom Elternverzeichnis erben.

Die Vererbung muss:

- beim Erzeugen atomar erfolgen,
- explizite Vererbungsflags beachten,
- Dateisystem- und Sicherheitsrichtlinien einhalten,
- keine administrativen Rechte unkontrolliert erzeugen.

## 44. File Cache

Der File Cache verwaltet dateibasierte Speicherseiten.

Er integriert:

- reguläres Lesen und Schreiben,
- Memory-Mapped Files,
- Read-ahead,
- Write-back,
- Dirty-Page-Verfolgung,
- Speicher-Reclaim.

Dieselbe Dateiseite soll möglichst nicht unabhängig mehrfach im Kernelcache vorhanden sein.

## 45. Buffer Cache

Der Buffer Cache verwaltet Block- und Metadatenpuffer.

Er wird insbesondere durch Dateisystemtreiber genutzt für:

- Superblöcke,
- Baumknoten,
- Journaldaten,
- Allokationsinformationen,
- Blockgerätezugriffe.

File Cache und Buffer Cache dürfen keine dauerhaft widersprüchlichen Ansichten derselben Daten erzeugen.

## 46. Memory-Mapped Files

Dateien können über den Virtual Memory Manager abgebildet werden.

```c
np_status_t np_file_map(
    np_handle_t file,
    uint64_t file_offset,
    uint64_t length,
    uint32_t protection,
    uint32_t flags,
    np_virtual_address_t* address
);
```

Die Ausführungsberechtigung wird zusätzlich durch:

- File-Handle-Rechte,
- Mount-Flags,
- Dateiattribute,
- Signatur- und Sicherheitsrichtlinien

begrenzt.

## 47. Synchrone I/O

Bei synchroner I/O kehrt der Systemaufruf nach Abschluss, Fehler, Timeout oder Unterbrechung zurück.

Ein synchroner Aufruf kann intern asynchron implementiert sein und auf ein Completion Object warten.

Kernelinterne Sperren dürfen nicht unzulässig über blockierende Geräte-I/O gehalten werden.

## 48. Asynchrone I/O

```c
np_status_t np_file_read_async(
    np_handle_t file,
    const np_async_io_args_t* arguments,
    np_handle_t* operation
);

np_status_t np_file_write_async(
    np_handle_t file,
    const np_async_io_args_t* arguments,
    np_handle_t* operation
);
```

Die Operation liefert ein wartbares Handle und kann an einen Event-Endpunkt gebunden werden.

Puffer bleiben bis zum Abschluss kontrolliert kopiert, referenziert oder gepinnt.

## 49. Flush und Synchronisation

```c
np_status_t np_file_flush(
    np_handle_t file,
    uint32_t flags
);

np_status_t np_filesystem_sync(
    np_handle_t filesystem,
    uint32_t flags
);
```

Zu unterscheiden sind:

- Übergabe an den File Cache,
- Übergabe an das Blockgerät,
- Bestätigung durch das physische Medium,
- Commit einer Dateisystemtransaktion.

Die API muss angeben, welche Dauerhaftigkeitsgarantie erreicht wurde.

## 50. Journaling Interface

Das VFS stellt ein generisches Transaktionsinterface für journalingfähige Dateisysteme bereit.

```c
np_status_t np_vfs_transaction_begin(
    np_filesystem_instance_t* filesystem,
    np_vfs_transaction_t** transaction
);

np_status_t np_vfs_transaction_commit(
    np_vfs_transaction_t* transaction
);

void np_vfs_transaction_abort(
    np_vfs_transaction_t* transaction
);
```

Die konkrete Journaling- oder Copy-on-Write-Implementierung bleibt Aufgabe des Dateisystemtreibers.

## 51. Mounten

```c
np_status_t np_vfs_mount(
    const np_mount_request_t* request,
    np_handle_t* mount
);
```

Vor dem Mount werden geprüft:

- Dateisystemtyp,
- Quellgerät oder Backend,
- Mount Point,
- Capabilities,
- Signatur- und Integritätsstatus,
- Mount-Flags,
- Namespace-Ziel,
- vorhandene Konflikte.

## 52. Aushängen

```c
np_status_t np_vfs_unmount(
    np_handle_t mount,
    uint32_t flags
);
```

Ein reguläres Unmount schlägt fehl, wenn nicht freigebbare aktive Referenzen bestehen.

Ein erzwungenes Unmount:

- sperrt neue Operationen,
- beendet oder fehlschlägt ausstehende I/O,
- invalidiert Mappings nach definierter Richtlinie,
- markiert Handles als nicht mehr nutzbar,
- erfordert administrative Rechte.

## 53. Root-Dateisystem

Während des Bootvorgangs wird zunächst ein Initial-RAM-Dateisystem als temporäres Root verwendet.

Nach erfolgreicher Prüfung wird das endgültige Root-Dateisystem gemountet und der System-Namespace kontrolliert umgeschaltet.

Fehlt das reguläre Root-Dateisystem, kann NovaOS:

- eine verifizierte ältere Systemgeneration starten,
- das Recovery-Subvolume verwenden,
- im Initial-RAM-Dateisystem verbleiben,
- den Bootvorgang sicher abbrechen.

## 54. Virtuelle Dateisysteme

Virtuelle Dateisysteme können dynamische Kernel- oder Systeminformationen darstellen.

Sie müssen:

- capability-gefilterte Ansichten erzeugen,
- keine ungeprüften Kerneladressen offenlegen,
- stabile versionierte Dateiformate verwenden,
- Seiteneffekte beim Lesen vermeiden, sofern nicht ausdrücklich dokumentiert.

Für strukturierte Systemsteuerung werden Kernel-Service-APIs gegenüber textbasierten Pseudodateien bevorzugt.

## 55. Geräteobjekte

Ausgewählte Geräte können über VFS-Nodes erreichbar sein.

Das Öffnen eines Device Nodes erzeugt eine Gerätesitzung beim Device Manager oder Driver Framework.

Ein Device Node verleiht nicht automatisch direkten MMIO-, DMA- oder Interruptzugriff.

Geräterechte werden unabhängig geprüft.

## 56. Netzwerkdateisysteme

Netzwerkdateisysteme werden über dieselbe VFS-Schnittstelle eingebunden.

Sie müssen zusätzlich behandeln:

- Verbindungsunterbrechungen,
- Reconnect,
- Cache-Kohärenz,
- Remote-Identitäten,
- Timeouts,
- Offline-Verhalten,
- Serveränderungen,
- Transportverschlüsselung.

Remote-Fehler müssen von lokalen Kernel- und Gerätefehlern unterscheidbar bleiben.

## 57. Dateisystemereignisse

Das VFS kann Beobachtungsobjekte bereitstellen.

```c
typedef enum np_vfs_watch_event {
    NP_VFS_WATCH_CREATED,
    NP_VFS_WATCH_DELETED,
    NP_VFS_WATCH_MODIFIED,
    NP_VFS_WATCH_RENAMED,
    NP_VFS_WATCH_ATTRIBUTES,
    NP_VFS_WATCH_MOUNTED,
    NP_VFS_WATCH_UNMOUNTED
} np_vfs_watch_event_t;
```

Beobachter sehen nur Objekte, für die sie Sicht- und Beobachtungsrechte besitzen.

## 58. Ereignisüberlauf

Dateisystemereignisqueues sind begrenzt.

Bei Überlauf wird ein eindeutiges Overflow-Ereignis erzeugt.

Der Client muss anschließend seinen Zustand neu synchronisieren.

Ein Überlauf darf nicht als vollständige Ereignishistorie dargestellt werden.

## 59. Capability-Rechte

Mindestens folgende Rechte werden definiert:

| Capability | Bedeutung |
|---|---|
| `CAP_VFS_LOOKUP` | Namen in einem Verzeichnis auflösen |
| `CAP_VFS_READ` | Dateidaten lesen |
| `CAP_VFS_WRITE` | Dateidaten schreiben |
| `CAP_VFS_CREATE` | neue Einträge erzeugen |
| `CAP_VFS_DELETE` | Einträge entfernen |
| `CAP_VFS_ATTRIBUTES` | Attribute lesen oder verändern |
| `CAP_VFS_EXECUTE` | Datei ausführbar abbilden |
| `CAP_VFS_MOUNT` | Dateisystem mounten |
| `CAP_VFS_UNMOUNT` | Mount entfernen |
| `CAP_VFS_WATCH` | Dateisystemereignisse beobachten |
| `CAP_VFS_ADMIN` | globale VFS-Richtlinien verwalten |

Rechte müssen auf konkrete Nodes, Subtrees, Mounts oder Namespaces begrenzbar sein.

## 60. Sicherheit

Das VFS muss sicherstellen, dass:

- Pfadauflösung Prozess-Root und Namespace-Grenzen einhält,
- Namen keine Capability-Prüfungen ersetzen,
- symbolische Links keine Sandbox-Grenzen unkontrolliert umgehen,
- File Handles keine Kernelzeiger enthalten,
- Mount-Flags Rechte nur einschränken,
- ausführbare Mappings W^X und Signaturrichtlinien beachten,
- Device Nodes keinen direkten Hardwarezugriff verleihen,
- TOCTOU-Risiken durch Handle-relative APIs reduziert werden,
- Dateisystemtreiber keine ungeprüften On-Disk-Daten verwenden,
- erzwungenes Unmount keine aktiven Kernelreferenzen unkontrolliert freigibt.

## 61. Datenschutz

Dateinamen, Pfade und Metadaten können personenbezogene Daten enthalten.

Daher gelten:

- Prozesse sehen nur ihre Namespaces und berechtigten Subtrees,
- VFS-Tracing enthält standardmäßig keine Dateiinhalte,
- sensible Pfade können in Diagnosen gekürzt oder pseudonymisiert werden,
- Dateiinhalte werden nicht durch generische Statistik-APIs ausgegeben,
- externe Telemetrie ist standardmäßig deaktiviert,
- Audit- und Diagnoseaufbewahrung muss konfigurierbar sein.

## 62. Diagnose

Das VFS stellt autorisierten Diagnosediensten bereit:

- aktive Dateisysteminstanzen,
- Mount-Hierarchien,
- offene File Handles,
- Cache-Auslastung,
- Dirty Pages,
- I/O-Latenzen,
- Pfadauflösungsfehler,
- Journal- und Sync-Status,
- Dateisystemfehler,
- erzwungene Unmounts.

Globale Handle- und Pfadansichten erfordern besondere Diagnoseberechtigungen.

## 63. Statistik

```c
typedef struct np_vfs_statistics {
    uint64_t active_filesystems;
    uint64_t active_mounts;
    uint64_t open_files;

    uint64_t read_operations;
    uint64_t write_operations;
    uint64_t bytes_read;
    uint64_t bytes_written;

    uint64_t path_lookups;
    uint64_t cache_hits;
    uint64_t cache_misses;

    uint64_t io_errors;
    uint64_t permission_denials;
} np_vfs_statistics_t;
```

Statistiken sollen pro Dateisystem, Mount, Prozess und System verfügbar sein.

## 64. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `NP_OK` | Operation erfolgreich |
| `NP_ERR_INVALID_ARGUMENT` | ungültiger Parameter |
| `NP_ERR_NOT_FOUND` | Datei oder Verzeichnis nicht gefunden |
| `NP_ERR_ALREADY_EXISTS` | Zielname existiert bereits |
| `NP_ERR_NOT_DIRECTORY` | Pfadsegment ist kein Verzeichnis |
| `NP_ERR_IS_DIRECTORY` | Operation ist für ein Verzeichnis unzulässig |
| `NP_ERR_ACCESS_DENIED` | erforderliches Recht fehlt |
| `NP_ERR_READ_ONLY` | Dateisystem oder Mount ist schreibgeschützt |
| `NP_ERR_NAME_TOO_LONG` | Name oder Pfad überschreitet die Grenze |
| `NP_ERR_SYMBOLIC_LINK_LOOP` | Linktiefe oder Schleife erkannt |
| `NP_ERR_CROSS_DEVICE` | Operation überschreitet Dateisystemgrenzen |
| `NP_ERR_FILE_BUSY` | Objekt wird noch verwendet |
| `NP_ERR_NO_SPACE` | kein Speicherplatz verfügbar |
| `NP_ERR_QUOTA` | Kontingent wurde überschritten |
| `NP_ERR_STALE_CURSOR` | Verzeichniscursor ist veraltet |
| `NP_ERR_IO` | allgemeiner I/O-Fehler |
| `NP_ERR_FILESYSTEM_CORRUPT` | Dateisystemintegrität verletzt |
| `NP_ERR_NOT_SUPPORTED` | Operation wird nicht unterstützt |

## 65. Fehlerbehandlung

Ein Fehler eines nicht kritischen Dateisystems soll auf seine Instanz begrenzt bleiben.

Mögliche Reaktionen sind:

- einzelne Operation ablehnen,
- beschädigten Node isolieren,
- Dateisystem auf Read-only umstellen,
- Journal wiederherstellen,
- Mount deaktivieren,
- Recovery- oder Scrub-Dienst starten.

Ein Fehler des Root-Dateisystems kann den Wechsel in eine verifizierte Recovery-Umgebung erfordern.

## 66. Testanforderungen

Mindestens folgende Tests sind erforderlich:

1. Registrierung eines Dateisystemtyps,
2. Mount einer Dateisysteminstanz,
3. mehrere Mounts derselben Instanz,
4. Mount-Namespace-Isolation,
5. absolute und relative Pfadauflösung,
6. Prozess-Root-Grenze,
7. `.`- und `..`-Segmente,
8. symbolischer Link,
9. symbolische Linkschleife,
10. Hardlink,
11. Datei erzeugen und öffnen,
12. exklusives Erzeugen,
13. Lesen und Schreiben,
14. partielle I/O,
15. positionsbasierte I/O,
16. atomarer Append,
17. Seek,
18. Verzeichnisaufzählung,
19. veralteter Directory Cursor,
20. atomarer Rename,
21. Cross-Filesystem-Rename,
22. Löschen einer geöffneten Datei,
23. Attribut- und ACL-Vererbung,
24. erweiterte Attribute,
25. File-Cache-Kohärenz,
26. Buffer-Cache-Kohärenz,
27. Memory-Mapped File,
28. Ausführungsschutz durch Mount-Flags,
29. synchrone und asynchrone I/O,
30. Abbruch einer I/O-Operation,
31. File Flush und Filesystem Sync,
32. Journaling-Transaktion,
33. reguläres Unmount,
34. Ablehnung eines belegten Mounts,
35. erzwungenes Unmount,
36. virtuelles Dateisystem,
37. Device Node,
38. Netzwerkdateisystem-Timeout,
39. Dateisystemereignisse und Queue-Überlauf,
40. beschädigtes Dateisystem,
41. Read-only-Fallback,
42. Recovery des Root-Dateisystems,
43. Capability- und Namespace-Prüfungen,
44. parallele VFS-Operationen auf mehreren CPUs,
45. korrekte Diagnose und Statistik.

## 67. Verbindliche Invarianten

1. Jede aktive Dateisystemressource ist ein Objekt im einheitlichen Kernelobjektmodell.
2. Ein File Handle enthält keinen direkt nutzbaren Kernelzeiger.
3. Pfadauflösung verlässt niemals unautorisiert den Prozess-Root oder Mount Namespace.
4. Ein Name oder Pfad verleiht keine Zugriffsrechte.
5. Symbolische Links werden mit begrenzter Tiefe und Schleifenerkennung aufgelöst.
6. Mount-Flags können Rechte einschränken, aber nicht erweitern.
7. Rename innerhalb einer Dateisysteminstanz erscheint atomar.
8. Offene Handles bleiben nach dem Entfernen eines Namenseintrags entsprechend der Dateisystemsemantik gültig.
9. Eine Open File Description wird erst nach Wegfall aller Referenzen freigegeben.
10. Dateisystemtreiber validieren alle nicht vertrauenswürdigen Metadaten.
11. Dirty Pages werden vor ihrer Freigabe geschrieben oder ausdrücklich verworfen.
12. VFS-Zustandsänderungen erfolgen unter geeigneter Synchronisation.

## 68. Referenzablauf einer Dateiöffnung

```text
Prozess ruft File Open auf
  -> Directory Handle und Pfad kopieren
  -> Handle-Rechte prüfen
  -> Pfad segmentweise auflösen
  -> Mount- und Namespace-Grenzen beachten
  -> symbolische Links nach Flags behandeln
  -> ACL-, Capability- und Mount-Rechte prüfen
  -> Node suchen oder atomar erzeugen
  -> Open File Description anlegen
  -> Dateisystem-Open-Operation ausführen
  -> File Handle mit begrenzten Rechten erzeugen
  -> Ergebnis an Prozess zurückgeben
```

## 69. Zusammenfassung

Das NovaOS Virtual File System vereinheitlicht lokale, virtuelle und netzwerkbasierte Dateisysteme unter einem objekt- und capability-basierten Modell.

VFS Nodes repräsentieren Dateisystemobjekte, während Open File Descriptions und generationsgeschützte File Handles den tatsächlichen Zugriff verwalten. Mount Namespaces, handle-relative Operationen und sichere symbolische Linkauflösung begrenzen Pfadangriffe und Sandbox-Ausbrüche.

File Cache, Buffer Cache, Memory-Mapped Files, asynchrone I/O und Journaling-Schnittstellen ermöglichen hohe Leistung, ohne Dateisystemintegrität und Sicherheitsgrenzen aufzugeben.