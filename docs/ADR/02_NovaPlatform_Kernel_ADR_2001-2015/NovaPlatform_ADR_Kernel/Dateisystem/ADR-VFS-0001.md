# ADR-VFS-0001 – Virtual File System

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0001 |
| Titel | Virtual File System |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Dateisysteme und Namensräume |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-KERNEL-0100, ADR-KERNEL-0102, ADR-KERNEL-0103, ADR-KERNEL-0105, ADR-IPC-0006 |
| Nachfolgende Spezifikation | NPSPEC-VFS-VIRTUAL-FILE-SYSTEM |

---

## 1. Kontext

NovaOS benötigt eine einheitliche Schnittstelle für unterschiedliche Dateisysteme und speicherähnliche Ressourcen.

Das Virtual File System soll unter anderem folgende Backends integrieren:

- NovaFS
- Boot-Dateisysteme
- FAT
- optische und entfernbare Datenträger
- Netzwerkdateisysteme
- temporäre Dateisysteme
- virtuelle Systemdateisysteme
- Kompatibilitätsdateisysteme
- verschlüsselte und schreibgeschützte Dateisysteme

Anwendungen sollen dieselben grundlegenden Dateioperationen verwenden können, unabhängig vom konkreten Dateisystem.

---

## 2. Problemstellung

Dateisysteme unterscheiden sich hinsichtlich:

- On-Disk-Format
- Namensregeln
- Berechtigungen
- Transaktionen
- Groß- und Kleinschreibung
- Links
- Streams und Attribute
- Konsistenzgarantien
- Netzwerk- und Fehlerverhalten

NovaOS benötigt eine Abstraktionsschicht, die gemeinsame Operationen vereinheitlicht, ohne spezielle Fähigkeiten einzelner Dateisysteme vollständig zu verbergen.

---

## 3. Entscheidung

NovaOS implementiert ein capability-basiertes Virtual File System als einheitliche Dateisystemschicht.

Das VFS verwaltet:

- Mount Namespaces
- Pfadauflösung
- offene Datei- und Verzeichnisobjekte
- Dateisysteminstanzen
- Caches
- Berechtigungsprüfungen
- dateisystemunabhängige Operationen
- versionierte Dateisystemtreiber-Schnittstellen

Jeder offene Zugriff wird durch eine Capability auf ein VFS-Objekt repräsentiert.

---

## 4. Architektur

```text
Anwendung
    |
    v
VFS API
    |
    +--> Pfadauflösung
    +--> Capability-Prüfung
    +--> Mount Namespace
    +--> Cache
    |
    v
Dateisystemtreiber
    |
    v
Blockgerät, Netzwerk oder Speicher
```

Dateisystemtreiber implementieren die eigentliche Speicherung. Das VFS stellt die gemeinsame Semantik und Sicherheitsgrenze bereit.

---

## 5. VFS-Objektmodell

Das VFS verwendet folgende grundlegende Objekte:

| Objekt | Bedeutung |
|---|---|
| File System Instance | Eingebundene Dateisysteminstanz |
| Mount | Verbindung einer Instanz mit einem Namespace |
| VNode | Dateisystemunabhängige Repräsentation eines Objekts |
| File Handle | Geöffneter Zugriff auf eine Datei |
| Directory Handle | Geöffneter Zugriff auf ein Verzeichnis |
| Path Context | Ausgangspunkt einer Pfadauflösung |
| Mount Namespace | Sichtbare Mount-Struktur eines Prozesses |

VNodes repräsentieren Identität und Metadaten. File Handles repräsentieren konkrete geöffnete Zugriffe und deren Zustand.

---

## 6. Unterstützte Objekttypen

Das native VFS unterstützt mindestens:

- reguläre Datei
- Verzeichnis
- symbolischer Link
- benannter Stream
- Socket- oder Serviceknoten für Kompatibilität
- spezielle Gerätedarstellung
- virtuelle Systemdatei

Geräte werden nativ weiterhin über Device Objects und Device Interfaces verwaltet. Eine Gerätedatei ist lediglich eine kontrollierte VFS-Ansicht auf eine solche Capability.

---

## 7. Mount Namespaces

Jeder Prozess verwendet einen Mount Namespace.

Ein Namespace bestimmt:

- sichtbare Dateisysteme
- Mountpunkte
- Root-Verzeichnis
- optionale private Mounts
- schreibgeschützte Bereiche
- Sichtbarkeit von System- und Benutzerdaten

Prozesse dürfen einen Namespace nur über entsprechende Capabilities verändern.

Job Objects, Anwendungscontainer und Recovery-Umgebungen können eigene Mount Namespaces erhalten.

---

## 8. Mount-Vorgang

Ein Dateisystem wird über eine kontrollierte Operation eingebunden:

```c
status_t vfs_mount(
    cap_handle_t namespace_handle,
    cap_handle_t source_handle,
    cap_handle_t target_directory,
    const char *filesystem_type,
    uint32_t flags,
    cap_handle_t *mount_handle
);
```

Mögliche Mount-Flags:

```c
#define VFS_MOUNT_READ_ONLY   (1u << 0)
#define VFS_MOUNT_NO_EXEC     (1u << 1)
#define VFS_MOUNT_NO_DEVICES  (1u << 2)
#define VFS_MOUNT_NO_SUID     (1u << 3)
#define VFS_MOUNT_PRIVATE     (1u << 4)
```

Das VFS validiert Quelle, Ziel, Dateisystemtyp und Sicherheitsrichtlinie vor der Veröffentlichung des Mounts.

---

## 9. Pfadmodell

NovaOS verwendet `/` als internen Pfadtrenner.

Beispiele:

```text
/system/bin
/users/matthias/documents
/apps/example/data
/devices
```

Ein absoluter Pfad beginnt am Root des aktuellen Mount Namespace. Ein relativer Pfad beginnt an einem expliziten Verzeichnis-Handle oder am Arbeitsverzeichnis des Prozesses.

Native APIs sollen Verzeichnis-Capabilities gegenüber globalen Pfadzeichenketten bevorzugen.

---

## 10. Pfadauflösung

Die Pfadauflösung erfolgt komponentenweise.

Dabei prüft das VFS:

- Gültigkeit jeder Namenskomponente
- Traversierungsrechte
- Mountübergänge
- symbolische Links
- Root-Grenzen
- Objektzustand
- Namens- und Längenlimits

`..` darf niemals oberhalb des Namespace-Roots oder eines festgelegten Capability-Roots auflösen.

Pfadauflösung und abschließendes Öffnen müssen so verbunden werden, dass keine Time-of-check-to-time-of-use-Lücke entsteht.

---

## 11. Dateinamen

NovaOS verwendet UTF-8 für native Dateinamen.

Das VFS definiert:

- eine maximale Komponentenlänge,
- eine maximale Pfadlänge,
- Verbot eingebetteter Nullzeichen,
- eine einheitliche interne Validierung,
- dateisystemspezifische Normalisierungsregeln.

Groß- und Kleinschreibung werden im nativen Modell unterschieden.

Ein Dateisystem mit anderer Semantik muss diese Eigenschaft über seine Capability- und Featureinformationen offenlegen.

---

## 12. Öffnen von Dateien

Eine Datei wird über ein Verzeichnis-Handle und einen relativen Pfad geöffnet:

```c
status_t vfs_open_at(
    cap_handle_t directory_handle,
    const char *path,
    uint32_t open_flags,
    uint32_t requested_rights,
    cap_handle_t *file_handle
);
```

Mögliche Flags:

```c
#define VFS_OPEN_CREATE       (1u << 0)
#define VFS_OPEN_EXCLUSIVE    (1u << 1)
#define VFS_OPEN_TRUNCATE     (1u << 2)
#define VFS_OPEN_DIRECTORY    (1u << 3)
#define VFS_OPEN_NO_FOLLOW    (1u << 4)
#define VFS_OPEN_CLOEXEC      (1u << 5)
```

Das Ergebnis ist eine Capability mit den tatsächlich gewährten Rechten.

---

## 13. Zugriffsrechte

File- und Directory-Capabilities unterstützen mindestens:

| Recht | Bedeutung |
|---|---|
| `VFS_RIGHT_READ_DATA` | Dateidaten lesen |
| `VFS_RIGHT_WRITE_DATA` | Dateidaten schreiben |
| `VFS_RIGHT_APPEND_DATA` | ausschließlich anhängen |
| `VFS_RIGHT_READ_METADATA` | Metadaten lesen |
| `VFS_RIGHT_WRITE_METADATA` | Metadaten ändern |
| `VFS_RIGHT_ENUMERATE` | Verzeichnis auflisten |
| `VFS_RIGHT_CREATE_CHILD` | untergeordnetes Objekt erzeugen |
| `VFS_RIGHT_DELETE_CHILD` | untergeordnetes Objekt entfernen |
| `VFS_RIGHT_EXECUTE` | Objekt ausführen |
| `VFS_RIGHT_TRANSFER` | Capability weitergeben |

Rechte dürfen bei Capability-Übertragung reduziert, aber nicht erweitert werden.

---

## 14. Dateioperationen

Das VFS stellt mindestens folgende Operationen bereit:

```c
status_t vfs_read(
    cap_handle_t file,
    void *buffer,
    uint64_t size,
    uint64_t *bytes_read
);

status_t vfs_write(
    cap_handle_t file,
    const void *buffer,
    uint64_t size,
    uint64_t *bytes_written
);

status_t vfs_read_at(
    cap_handle_t file,
    uint64_t offset,
    void *buffer,
    uint64_t size,
    uint64_t *bytes_read
);

status_t vfs_write_at(
    cap_handle_t file,
    uint64_t offset,
    const void *buffer,
    uint64_t size,
    uint64_t *bytes_written
);
```

Teiloperationen sind zulässig und müssen über die tatsächlich verarbeitete Byteanzahl gemeldet werden.

---

## 15. Verzeichnisoperationen

Verzeichnisse werden über stabile Cursor aufgelistet:

```c
status_t vfs_enumerate(
    cap_handle_t directory,
    uint64_t cursor,
    vfs_directory_entry_t *entries,
    uint32_t capacity,
    uint32_t *entry_count,
    uint64_t *next_cursor
);
```

Eine Verzeichnisänderung während der Auflistung kann einen Cursor ungültig machen. In diesem Fall liefert das VFS einen eindeutigen Neustartstatus.

Interne Dateisystemzeiger dürfen nicht als öffentliche Cursor verwendet werden.

---

## 16. Symbolische Links

Symbolische Links werden unterstützt, unterliegen aber festen Regeln:

- maximale Auflösungstiefe
- Schleifenerkennung
- optionales `NO_FOLLOW`
- Begrenzung auf den aktuellen Namespace
- erneute Rechteprüfung nach jedem Auflösungsschritt

Für sicherheitskritische Systemoperationen werden Handle-basierte Zugriffe ohne symbolische Linkauflösung bevorzugt.

---

## 17. Caching

Das VFS darf folgende Informationen zwischenspeichern:

- VNodes
- Verzeichniseinträge
- negative Namensauflösungen
- Metadaten
- Dateiseiten

Caches müssen durch Dateisystemänderungen, Mountwechsel und Netzwerkereignisse korrekt invalidiert werden.

Ein Cache darf niemals eine aktuelle Capability- oder Berechtigungsprüfung ersetzen.

---

## 18. Dateisystemtreiber

Dateisystemtreiber implementieren eine versionierte Schnittstelle:

```c
typedef struct vfs_filesystem_operations {
    status_t (*mount)(vfs_mount_context_t *context);
    status_t (*unmount)(vfs_instance_t *instance);
    status_t (*lookup)(vfs_node_t *directory, const char *name, vfs_node_t **result);
    status_t (*read)(vfs_node_t *node, uint64_t offset, void *buffer, uint64_t size);
    status_t (*write)(vfs_node_t *node, uint64_t offset, const void *buffer, uint64_t size);
    status_t (*sync)(vfs_instance_t *instance);
} vfs_filesystem_operations_t;
```

Die konkrete Schnittstelle wird durch Größen- und Versionsfelder erweiterbar gehalten.

Dateisystemtreiber dürfen abhängig von Sicherheits- und Leistungsanforderungen im Kernel oder in isolierten Userspace-Diensten laufen.

---

## 19. Synchronisation und Konsistenz

Das VFS stellt grundlegende Synchronisationsoperationen bereit:

```c
status_t vfs_flush(cap_handle_t file_handle);
status_t vfs_sync(cap_handle_t mount_handle);
```

`flush` fordert die Persistierung einer bestimmten Datei an.

`sync` fordert die Synchronisation einer vollständigen Dateisysteminstanz an.

Die konkrete Dauerhaftigkeitsgarantie hängt von den Fähigkeiten des zugrunde liegenden Dateisystems und Speichergeräts ab und muss über Featureinformationen abgefragt werden können.

---

## 20. Unmount

Ein Dateisystem kann nur sicher ausgehängt werden, wenn:

- keine nicht widerrufbaren offenen Zugriffe bestehen,
- ausstehende Schreibvorgänge abgeschlossen sind,
- abhängige Mounts entfernt wurden,
- das Dateisystem synchronisiert wurde.

Ein erzwungenes Unmount darf offene Handles widerrufen und laufende Operationen mit `STATUS_FILESYSTEM_UNMOUNTED` beenden.

Für das Root- und Systemdateisystem gelten zusätzliche Schutzrichtlinien.

---

## 21. Sicherheit

Für das VFS gelten folgende Regeln:

- Jeder Zugriff erfolgt über Capabilities.
- Pfadnamen stellen keine Berechtigungen dar.
- Berechtigungen werden bei der Auflösung und beim Öffnen geprüft.
- Mount Namespaces begrenzen die sichtbare Dateisystemstruktur.
- Ausführungsrechte sind von Leserechten getrennt.
- Dateisystemdaten und Metadaten gelten als nicht vertrauenswürdig.
- Dateisystemtreiber müssen Größen, Offsets und Strukturen validieren.
- Mount-Flags wie `NO_EXEC` dürfen nicht durch untergeordnete APIs umgangen werden.

Das Systemdateisystem kann zusätzlich durch unveränderliche Systemgenerationen und signierte Integritätsmetadaten geschützt werden.

---

## 22. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_NOT_FOUND` | Objekt existiert nicht |
| `STATUS_ALREADY_EXISTS` | Objekt existiert bereits |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_NOT_DIRECTORY` | Pfadkomponente ist kein Verzeichnis |
| `STATUS_IS_DIRECTORY` | Operation ist für Verzeichnisse ungeeignet |
| `STATUS_READ_ONLY` | Dateisystem oder Mount ist schreibgeschützt |
| `STATUS_NO_SPACE` | Kein Speicherplatz verfügbar |
| `STATUS_NAME_TOO_LONG` | Name oder Pfad ist zu lang |
| `STATUS_SYMLINK_LOOP` | Symbolische Linkschleife erkannt |
| `STATUS_FILESYSTEM_CORRUPT` | Dateisystemstruktur ist beschädigt |
| `STATUS_FILESYSTEM_UNMOUNTED` | Dateisystem wurde ausgehängt |
| `STATUS_WOULD_BLOCK` | Operation würde blockieren |
| `STATUS_NOT_SUPPORTED` | Funktion wird nicht unterstützt |

---

## 23. Alternative Ansätze

### 23.1 Direkte Dateisystem-APIs

**Vorteile:**

- Dateisystemfunktionen bleiben vollständig sichtbar
- geringe gemeinsame Abstraktionskomplexität

**Nachteile:**

- Anwendungen müssten jedes Dateisystem separat unterstützen
- keine gemeinsame Sicherheits- und Mountlogik

**Entscheidung:** Abgelehnt.

### 23.2 Ein globaler Mount Namespace

**Vorteile:**

- einfache Verwaltung

**Nachteile:**

- keine Isolation für Anwendungen, Jobs oder Recovery
- unnötige Sichtbarkeit fremder Dateisysteme

**Entscheidung:** Abgelehnt.

### 23.3 Ausschließlich pfadbasierte APIs

**Vorteile:**

- einfache Verwendung

**Nachteile:**

- anfällig für Race Conditions
- schwächere Capability-Integration

**Entscheidung:** Native APIs bevorzugen Verzeichnis- und Datei-Handles. Pfadbasierte Komfortfunktionen können darauf aufbauen.

---

## 24. Konsequenzen

### Positive Konsequenzen

- einheitliche API für unterschiedliche Dateisysteme
- capability-basierter Dateizugriff
- getrennte Mount Namespaces
- sichere Handle-basierte Pfadauflösung
- Unterstützung lokaler und entfernter Dateisysteme
- erweiterbare Dateisystemtreiber-ABI
- Integration in das NovaOS-Objektmodell

### Negative Konsequenzen

- zusätzliche Abstraktions- und Cachekomplexität
- unterschiedliche Dateisystemsemantiken lassen sich nicht vollständig vereinheitlichen
- Mount- und Namespaceverwaltung benötigt sorgfältige Synchronisation
- Userspace-Dateisysteme verursachen zusätzlichen IPC-Aufwand

### Risiken

- Pfadauflösungs-Race-Conditions
- fehlerhafte Cacheinvalidierung
- symbolische Linkschleifen
- offene Handles beim Unmount
- beschädigte oder bösartige Dateisystemmetadaten

---

## 25. Implementierungsrichtlinien

- Native APIs verwenden Verzeichnis-Capabilities als Ausgangspunkt.
- Pfadauflösung und Öffnen müssen gegen TOCTOU-Rennen geschützt sein.
- `..` darf Capability- und Namespace-Roots nicht überschreiten.
- Dateisystemtreiber validieren alle On-Disk- und Netzwerkdaten.
- Mount-Flags werden bei jeder betroffenen Operation durchgesetzt.
- Caches ersetzen keine Rechteprüfung.
- Symbolische Links besitzen eine feste Auflösungstiefe.
- File Handles und VNodes werden als getrennte Objekte verwaltet.
- Unmount und Mount-Ersetzung erfolgen atomar für den Namespace.

---

## 26. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Mount und Unmount
- getrennte Mount Namespaces
- absolute und relative Pfadauflösung
- Handle-basierte Zugriffe
- Capability-Rechte
- symbolische Links und Schleifen
- paralleles Erzeugen, Umbenennen und Löschen
- Cacheinvalidierung
- schreibgeschützte und `NO_EXEC`-Mounts
- beschädigte Dateisysteme
- erzwungenes Unmount
- Dateisystemtreiberabsturz
- 32-Bit-/64-Bit-ABI-Kompatibilität
- Netzwerkdateisystemfehler
- Fuzzing von Pfaden und Metadaten

---

## 27. Verbindliche Invarianten

1. Das VFS ist die einheitliche native Dateisystemschnittstelle von NovaOS.
2. Offene Dateien und Verzeichnisse werden durch Capabilities repräsentiert.
3. Pfade und Dateinamen stellen keine Zugriffsberechtigung dar.
4. Jeder Prozess arbeitet innerhalb eines Mount Namespace.
5. `..` darf den festgelegten Namespace- oder Capability-Root nicht überschreiten.
6. Pfadauflösung und Öffnen müssen gegen TOCTOU-Rennen geschützt sein.
7. Mount-Flags werden unabhängig vom Dateisystemtreiber erzwungen.
8. Dateisystemdaten gelten als nicht vertrauenswürdig.
9. VNodes und konkrete File Handles bleiben getrennte Objekte.
10. Dateisystemspezifische Fähigkeiten werden nicht fälschlich als allgemein garantiert dargestellt.

---

## 28. Zusammenfassung

NovaOS implementiert ein capability-basiertes Virtual File System als gemeinsame Schnittstelle für lokale, virtuelle und netzwerkbasierte Dateisysteme.

Das VFS verwaltet Mount Namespaces, Pfadauflösung, VNodes, offene Handles, Caches und versionierte Dateisystemtreiber. Native APIs verwenden nach Möglichkeit Verzeichnis-Capabilities statt ausschließlich globaler Pfade.

Dadurch bleiben Dateizugriffe isolierbar, erweiterbar und mit dem einheitlichen Objekt- und Sicherheitsmodell von NovaOS vereinbar.