# ADR-VFS-0007 – File Handles

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0007 |
| Titel | File Handles |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Virtual File System und Ressourcenverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-VFS-0001, ADR-VFS-0003, ADR-VFS-0004, ADR-KERNEL-0102, ADR-KERNEL-0103 |
| Nachfolgende Spezifikation | NPSPEC-VFS-FILE-HANDLES |

---

## 1. Kontext

Nach dem Öffnen einer Datei benötigt ein Prozess eine stabile Referenz für weitere Operationen.

Ein Pfadname ist dafür ungeeignet, da eine Datei während ihrer Verwendung:

- umbenannt,
- verschoben,
- gelöscht,
- durch einen anderen Mount verdeckt,
- über mehrere Namen erreichbar,
- innerhalb eines anderen Namespace unsichtbar werden kann.

NovaOS verwendet deshalb File Handles als capability-geschützte Referenzen auf geöffnete VFS-Objekte.

---

## 2. Problemstellung

Das VFS muss eindeutig unterscheiden zwischen:

- Identität einer Datei,
- konkretem geöffneten Zugriff,
- Zugriffsrechten,
- aktueller Dateiposition,
- Öffnungsoptionen,
- Prozessreferenz auf den Zugriff.

File Handles müssen außerdem sicher übertragbar, widerrufbar und mit parallelen Operationen kompatibel sein.

---

## 3. Entscheidung

NovaOS repräsentiert jeden geöffneten Dateizugriff durch ein File Handle Kernel Object.

Der Prozess erhält eine Capability für dieses Objekt. Die Capability bestimmt, welche Operationen der Prozess ausführen darf.

Ein File Handle enthält keine global sichtbare Dateideskriptor-Nummer. Eine POSIX-Kompatibilitätsschicht darf numerische File Descriptors auf native File-Handle-Capabilities abbilden.

---

## 4. Abgrenzung der Objekte

| Objekt | Bedeutung |
|---|---|
| VNode | Identität und Metadaten eines Dateisystemobjekts |
| Open File Description | Zustand eines konkreten Öffnungsvorgangs |
| File Handle Capability | Berechtigter Prozesszugriff auf die Open File Description |
| File Descriptor | Numerischer Eintrag einer Kompatibilitätsschicht |

Mehrere Capabilities können dieselbe Open File Description referenzieren und dadurch eine gemeinsame Dateiposition besitzen.

Unabhängig geöffnete Handles besitzen getrennte Dateipositionen.

---

## 5. File-Handle-Struktur

```c
typedef struct vfs_file_handle_info {
    uint64_t handle_id;
    uint64_t vnode_id;
    uint64_t filesystem_instance_id;
    uint64_t current_offset;
    uint32_t open_flags;
    uint32_t granted_rights;
    uint64_t generation;
} vfs_file_handle_info_t;
```

Kernelintern enthält das Objekt zusätzlich:

- Referenz auf den VNode
- Mount- und Dateisystemreferenz
- aktuelle Dateiposition
- Status- und Fehlerzustand
- Sperrinformationen
- Writeback-Fehler
- Referenzanzahl
- optionale asynchrone I/O-Zustände

---

## 6. Öffnen

Ein File Handle wird durch eine atomare Open-Operation erzeugt:

```c
status_t vfs_open_at(
    cap_handle_t directory_handle,
    const char *path,
    uint32_t open_flags,
    uint32_t requested_rights,
    cap_handle_t *file_handle
);
```

Der Kernel prüft:

1. Verzeichnis-Capability,
2. Pfadauflösung,
3. Mount Namespace,
4. symbolische Links,
5. Dateisystemberechtigungen,
6. Mount-Flags,
7. angeforderte Capability-Rechte,
8. Öffnungsoptionen.

Das Handle wird erst nach erfolgreicher Gesamtprüfung zurückgegeben.

---

## 7. Öffnungsflags

Unterstützte Flags sind mindestens:

```c
#define VFS_OPEN_CREATE       (1u << 0)
#define VFS_OPEN_EXCLUSIVE    (1u << 1)
#define VFS_OPEN_TRUNCATE     (1u << 2)
#define VFS_OPEN_APPEND       (1u << 3)
#define VFS_OPEN_DIRECTORY    (1u << 4)
#define VFS_OPEN_NO_FOLLOW    (1u << 5)
#define VFS_OPEN_SYNCHRONOUS  (1u << 6)
#define VFS_OPEN_CLOEXEC      (1u << 7)
```

Öffnungsflags beschreiben das Verhalten des Handles. Sie dürfen keine Rechte gewähren, die durch die Capability oder das Dateisystem nicht erlaubt sind.

---

## 8. File-Handle-Rechte

| Recht | Bedeutung |
|---|---|
| `FILE_RIGHT_READ` | Dateidaten lesen |
| `FILE_RIGHT_WRITE` | Dateidaten schreiben |
| `FILE_RIGHT_APPEND` | Daten atomar anhängen |
| `FILE_RIGHT_SEEK` | Dateiposition verändern |
| `FILE_RIGHT_READ_METADATA` | Metadaten lesen |
| `FILE_RIGHT_WRITE_METADATA` | Metadaten verändern |
| `FILE_RIGHT_MAP` | Datei in den Adressraum einblenden |
| `FILE_RIGHT_EXECUTE` | Datei als Programm oder Codequelle verwenden |
| `FILE_RIGHT_SYNC` | Persistenz anfordern |
| `FILE_RIGHT_DUPLICATE` | Capability duplizieren |
| `FILE_RIGHT_TRANSFER` | Capability an einen anderen Prozess übertragen |

Leserechte beinhalten nicht automatisch Ausführungs- oder Mappingrechte.

---

## 9. Lesen und Schreiben

Positionsabhängige Operationen verwenden den aktuellen Handle-Offset:

```c
status_t vfs_read(
    cap_handle_t file_handle,
    void *buffer,
    uint64_t size,
    uint64_t *bytes_read
);

status_t vfs_write(
    cap_handle_t file_handle,
    const void *buffer,
    uint64_t size,
    uint64_t *bytes_written
);
```

Nach erfolgreicher Operation wird der Offset um die tatsächlich übertragene Byteanzahl erhöht.

Teilweise Lese- und Schreiboperationen sind zulässig und müssen eindeutig gemeldet werden.

---

## 10. Positionsunabhängige Operationen

Explizit positionierte Operationen verändern den gemeinsamen Handle-Offset nicht:

```c
status_t vfs_read_at(
    cap_handle_t file_handle,
    uint64_t offset,
    void *buffer,
    uint64_t size,
    uint64_t *bytes_read
);

status_t vfs_write_at(
    cap_handle_t file_handle,
    uint64_t offset,
    const void *buffer,
    uint64_t size,
    uint64_t *bytes_written
);
```

Diese Operationen werden für parallele I/O und Datenbanken bevorzugt.

Alle Offset- und Größenberechnungen müssen gegen Integerüberläufe geprüft werden.

---

## 11. Seek

Die aktuelle Dateiposition kann verändert werden:

```c
status_t vfs_seek(
    cap_handle_t file_handle,
    int64_t offset,
    uint32_t origin,
    uint64_t *new_position
);
```

Unterstützte Ursprünge:

| Ursprung | Bedeutung |
|---|---|
| `VFS_SEEK_BEGIN` | relativ zum Dateianfang |
| `VFS_SEEK_CURRENT` | relativ zur aktuellen Position |
| `VFS_SEEK_END` | relativ zum aktuellen Dateiende |
| `VFS_SEEK_DATA` | nächster Datenbereich |
| `VFS_SEEK_HOLE` | nächster Sparse-Bereich |

Nicht alle Objekttypen unterstützen Seek.

---

## 12. Append-Semantik

Bei einem mit `VFS_OPEN_APPEND` geöffneten Handle wird jeder Schreibvorgang atomar am jeweils aktuellen Dateiende positioniert.

Die Anwendung darf Append nicht durch ein separates Seek zum Dateiende nachbilden, da zwischen Seek und Write andere Schreiber aktiv werden können.

Bei verteilten Dateisystemen gilt die atomare Append-Garantie nur, wenn das Backend sie ausdrücklich unterstützt.

---

## 13. Duplizierung

Eine File-Handle-Capability kann mit reduzierten Rechten dupliziert werden:

```c
status_t vfs_handle_duplicate(
    cap_handle_t source_handle,
    uint32_t requested_rights,
    uint32_t flags,
    cap_handle_t *new_handle
);
```

Standardmäßig verweist das Duplikat auf dieselbe Open File Description. Dadurch werden geteilt:

- aktuelle Dateiposition,
- bestimmte Öffnungsflags,
- asynchroner Fehlerzustand.

Eine unabhängige Dateiposition erfordert ein erneutes Öffnen oder eine ausdrücklich unterstützte Clone-Operation.

---

## 14. Übertragung und Vererbung

File Handles können:

- bei der Prozesserzeugung gezielt vererbt,
- über Message Passing übertragen,
- einem Standard-Stream zugewiesen,
- an einen isolierten Dienst delegiert werden.

Übertragene Rechte dürfen die Rechte des Absenders nicht überschreiten.

Sandbox- und Namespace-Grenzen prüfen, ob das Handle Zugriff auf ansonsten nicht sichtbare Dateisystembereiche ermöglichen würde.

Eine solche Übertragung muss ausdrücklich autorisiert werden.

---

## 15. Close-on-Execute

Handles mit `VFS_OPEN_CLOEXEC` werden beim erfolgreichen Start eines neuen Programmbilds automatisch geschlossen.

Dies verhindert, dass sensible Handles unbeabsichtigt an ein anderes Programm weitergegeben werden.

NovaOS verwendet für sicherheitskritische Handles standardmäßig Close-on-Execute, sofern ihre Vererbung nicht ausdrücklich angefordert wurde.

---

## 16. Löschen und Umbenennen

Ein File Handle bleibt nach einer Umbenennung gültig, da es den VNode und nicht den ursprünglichen Pfad referenziert.

Wird der letzte Verzeichniseintrag einer geöffneten Datei entfernt:

- verschwindet die Datei aus neuen Pfadauflösungen,
- bleiben bestehende File Handles gültig,
- werden die Speicherressourcen erst nach Freigabe der letzten notwendigen Referenz zurückgewonnen.

Dateisysteme mit anderer Semantik müssen dies als abweichende Fähigkeit offenlegen.

---

## 17. Mount und Unmount

Ein File Handle hält grundsätzlich eine Referenz auf die zugehörige Dateisysteminstanz.

Bei regulärem Unmount verhindern offene Handles den Abschluss oder werden im Lazy-Modus weitergeführt.

Bei Force- oder Emergency-Unmount können Handles widerrufen werden. Weitere Operationen liefern dann:

```text
STATUS_FILESYSTEM_UNMOUNTED
```

Handles dürfen nach Medienwechsel oder Hot-Unplug nicht auf ein neu eingelegtes Medium umgelenkt werden.

---

## 18. Synchronisation und Persistenz

Ein Handle kann Writeback und Persistenz anfordern:

```c
status_t vfs_handle_flush(
    cap_handle_t file_handle
);

status_t vfs_handle_fsync(
    cap_handle_t file_handle
);
```

`flush` übergibt Dirty Data an das Dateisystem.

`fsync` wartet auf die durch Dateisystem und Speichergerät zugesicherte Dauerhaftigkeit von Dateidaten und erforderlichen Metadaten.

Frühere Writeback-Fehler müssen über das Handle sichtbar bleiben.

---

## 19. Asynchrone I/O

File Handles dürfen als Quelle oder Ziel asynchroner I/O-Operationen verwendet werden.

Eine asynchrone Operation hält eigene Referenzen auf:

- File Handle
- Buffer oder Memory Object
- Completion-Objekt
- Prozess- und Sicherheitskontext

Das Schließen der Prozess-Capability bricht eine bereits angenommene I/O-Operation nicht automatisch ab.

Abbruch und Lebenszeit werden über die asynchrone I/O-Schnittstelle geregelt.

---

## 20. Thread-Sicherheit

Mehrere Threads dürfen dasselbe File Handle verwenden.

Dabei gelten folgende Regeln:

- Aktualisierungen der gemeinsamen Dateiposition erfolgen atomar.
- `read_at` und `write_at` verwenden keinen gemeinsamen Offset.
- Handle-Zustandsänderungen werden synchronisiert.
- Close und I/O dürfen keine Use-after-free-Zustände erzeugen.
- Append-Schreibvorgänge werden entsprechend der Dateisystemgarantie atomar positioniert.

Für komplexe Anwendungsprotokolle bleibt zusätzliche Synchronisation Aufgabe der Anwendung.

---

## 21. Widerruf

NovaOS unterstützt den Widerruf eines File Handles bei:

- Force-Unmount
- Verlust des Datenträgers
- Sicherheitsrichtlinienänderung
- administrativer Isolation
- beschädigter Dateisysteminstanz
- Ablauf einer delegierten Berechtigung

Ein Widerruf beendet neue Operationen und weckt blockierte Aufrufe mit einem eindeutigen Fehler.

Bereits vom Speichergerät bestätigte Operationen bleiben erfolgt.

---

## 22. Sicherheit

Für File Handles gelten folgende Regeln:

- Handles werden ausschließlich über Capabilities angesprochen.
- Numerische IDs oder Pfade erteilen keine Rechte.
- Rechte werden beim Öffnen festgelegt und können danach nur reduziert werden.
- Read, Write, Execute und Map sind getrennte Rechte.
- Namespace-Wechsel erweitert keine Handle-Rechte.
- Übertragung und Vererbung müssen ausdrücklich erlaubt sein.
- Handles dürfen keine Kernelzeiger offenlegen.
- Wiederverwendete Handle-IDs müssen gegen veraltete Referenzen geschützt sein.

Ein gültiges Handle kann bewusst Zugriff außerhalb des aktuellen Namespace ermöglichen. Es ist deshalb selbst eine Sicherheitsberechtigung.

---

## 23. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_INVALID_HANDLE` | Capability ist ungültig |
| `STATUS_ACCESS_DENIED` | erforderliches Recht fehlt |
| `STATUS_END_OF_FILE` | Dateiende wurde erreicht |
| `STATUS_INVALID_OFFSET` | Offset ist nicht zulässig |
| `STATUS_NOT_SEEKABLE` | Objekt unterstützt Seek nicht |
| `STATUS_READ_ONLY` | Schreiben ist nicht erlaubt |
| `STATUS_BROKEN_HANDLE` | zugrunde liegendes Objekt ist ausgefallen |
| `STATUS_FILESYSTEM_UNMOUNTED` | Dateisystem wurde ausgehängt |
| `STATUS_DEVICE_REMOVED` | Speichermedium wurde entfernt |
| `STATUS_IO_ERROR` | I/O-Operation ist fehlgeschlagen |
| `STATUS_REVOKED` | Handle wurde widerrufen |

Ein gewöhnlicher Lesezugriff darf das Dateiende alternativ als erfolgreichen Zugriff mit null gelesenen Bytes darstellen. Die API-Spezifikation muss dies pro Operation eindeutig festlegen.

---

## 24. Audit und Diagnose

Audit-Ereignisse können erzeugt werden bei:

- Übertragung privilegierter File Handles,
- Ausführung über ein Datei-Handle,
- Widerruf sicherheitskritischer Handles,
- Zugriff auf geschützte Systemdateien,
- Umgehungsversuchen von Namespace-Grenzen,
- Force-Unmount mit offenen Handles.

Dateiinhalte werden nicht standardmäßig protokolliert.

Diagnosewerkzeuge dürfen autorisiert Rechte, Flags, Offset, Dateisysteminstanz und Fehlerstatus anzeigen.

---

## 25. Alternative Ansätze

### 25.1 Ausschließlich numerische File Descriptors

**Vorteile:**

- bekannte POSIX-Semantik
- kompakte Prozessdarstellung

**Nachteile:**

- schwächere native Capability-Identität
- zusätzliche Prüfung über prozesslokale Tabellen notwendig

**Entscheidung:** Numerische Deskriptoren werden nur durch Kompatibilitätsschichten angeboten.

### 25.2 Pfad bei jeder Operation neu auflösen

**Vorteile:**

- keine offenen Handle-Objekte notwendig

**Nachteile:**

- langsam
- Race Conditions bei Umbenennung und Löschung
- keine stabile Objektidentität

**Entscheidung:** Abgelehnt.

### 25.3 Gemeinsame Dateiposition für alle Öffnungen derselben Datei

**Vorteile:**

- geringer Zustandsaufwand

**Nachteile:**

- unerwartete Interaktionen zwischen unabhängigen Prozessen
- ungeeignet für parallelen Zugriff

**Entscheidung:** Abgelehnt.

---

## 26. Konsequenzen

### Positive Konsequenzen

- stabile Referenz unabhängig vom Pfad
- feingranulare Capability-Rechte
- sichere Übertragung und Vererbung
- getrennte oder bewusst gemeinsame Dateipositionen
- Unterstützung paralleler und asynchroner I/O
- klare Unmount- und Widerrufssemantik
- Grundlage für POSIX-Kompatibilität

### Negative Konsequenzen

- zusätzlicher Kernelzustand pro geöffnetem Zugriff
- Handle-Lebenszeiten können Unmount verhindern
- gemeinsame Offsets benötigen Synchronisation
- Handle-Übertragung kann Namespace-Isolation bewusst überbrücken

### Risiken

- unbeabsichtigte Handle-Vererbung
- Zugriff außerhalb einer Sandbox
- Race Conditions zwischen Close und I/O
- übersehene Writeback-Fehler
- Ressourcenerschöpfung durch viele offene Handles

---

## 27. Implementierungsrichtlinien

- VNode, Open File Description und Capability bleiben getrennte Objekte.
- Rechte werden beim Öffnen festgelegt und niemals nachträglich erweitert.
- Native APIs bevorzugen File-Handle-Capabilities.
- Positionsunabhängige I/O verändert den gemeinsamen Offset nicht.
- Append wird atomar durch das VFS oder Dateisystem umgesetzt.
- Close-on-Execute ist für sensible Handles Standard.
- Handle-Übertragung über Namespace-Grenzen wird ausdrücklich geprüft.
- Unmount und Medienwechsel dürfen Handles nicht auf neue Objekte umleiten.
- Writeback-Fehler bleiben über das Handle sichtbar.
- Close und parallele I/O verwenden sichere Referenzzählung.

---

## 28. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Öffnen und Schließen
- Rechtebegrenzung
- Lesen, Schreiben und Seek
- `read_at` und `write_at`
- atomaren Append
- Handle-Duplikation
- gemeinsame und getrennte Dateipositionen
- Übertragung an andere Prozesse
- Close-on-Execute
- Umbenennung und Löschen geöffneter Dateien
- Lazy- und Force-Unmount
- Medienwechsel
- parallele I/O
- asynchrone Operationen
- Widerruf und Ressourcenlimits

---

## 29. Verbindliche Invarianten

1. Ein File Handle ist eine capability-geschützte Referenz auf einen geöffneten Zugriff.
2. VNode, Open File Description und Handle-Capability bleiben getrennt.
3. Pfade und Handle-IDs stellen keine Zugriffsberechtigung dar.
4. Handle-Rechte dürfen nach dem Öffnen nur reduziert werden.
5. Unabhängige Öffnungen besitzen getrennte Dateipositionen.
6. Duplikate derselben Open File Description teilen standardmäßig die Dateiposition.
7. `read_at` und `write_at` verändern die Dateiposition nicht.
8. Append-Schreibvorgänge werden atomar am aktuellen Dateiende positioniert.
9. Namespace-Wechsel oder Unmount dürfen ein Handle nicht auf ein anderes Objekt umleiten.
10. Unbeabsichtigte Vererbung wird durch Close-on-Execute und Allowlisting verhindert.

---

## 30. Zusammenfassung

NovaOS verwendet capability-geschützte File Handles als stabile Referenzen auf geöffnete VFS-Objekte.

File Handles bleiben unabhängig von Pfadänderungen gültig, besitzen explizite Rechte und unterstützen gemeinsame oder unabhängige Dateipositionen, parallele I/O und kontrollierte Übertragung zwischen Prozessen.

Numerische File Descriptors bleiben einer POSIX-Kompatibilitätsschicht vorbehalten. Das native NovaOS-Modell arbeitet direkt mit versionierten Kernelobjekten und Capabilities.