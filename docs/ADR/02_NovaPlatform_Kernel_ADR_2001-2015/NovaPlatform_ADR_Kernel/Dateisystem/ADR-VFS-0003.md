# ADR-VFS-0003 – Namespace

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0003 |
| Titel | VFS Namespace |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Dateisysteme und Namensraumisolation |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-VFS-0001, ADR-VFS-0002, ADR-PROC-0009, ADR-PROC-0010, ADR-KERNEL-0103 |
| Nachfolgende Spezifikation | NPSPEC-VFS-NAMESPACE |

---

## 1. Kontext

NovaOS benötigt getrennte Dateisystemansichten für unterschiedliche Prozesse, Benutzer, Sitzungen, Anwendungen und Systemumgebungen.

Nicht jeder Prozess soll dieselben Mounts, Verzeichnisse oder Systemressourcen sehen. Dies gilt insbesondere für:

- Anwendungsisolierung
- Benutzer- und Sessiontrennung
- Systemdienste
- Job Objects
- Entwicklungsumgebungen
- Kompatibilitätsumgebungen
- Updates
- Recovery
- Container

Das VFS benötigt deshalb ein natives Namespace-Modell.

---

## 2. Problemstellung

Ein einziger globaler Dateisystemnamensraum würde allen Prozessen grundsätzlich dieselbe Mount-Struktur zeigen.

Das erschwert:

- Zugriffsbeschränkungen
- Anwendungs-Sandboxing
- getrennte Benutzerumgebungen
- sichere Systemupdates
- Recovery-Operationen
- private temporäre Mounts
- Kompatibilitätsumgebungen

Zugleich müssen Namespaces effizient erzeugt werden, ohne bei jeder Ableitung den vollständigen Mount-Baum zu kopieren.

---

## 3. Entscheidung

NovaOS implementiert capability-geschützte VFS Namespaces.

Ein Namespace definiert:

- den sichtbaren Root,
- den Mount-Baum,
- sichtbare Dateisysteminstanzen,
- Namespace-spezifische Mounts,
- Pfadauflösungsgrenzen,
- Propagationsregeln,
- Sicherheitsrichtlinien.

Jeder Prozess besitzt zu jeder Zeit genau einen aktiven VFS Namespace, kann aber zusätzliche Namespace-Capabilities halten.

---

## 4. Namespace-Typen

NovaOS verwendet keine technisch vollständig getrennten Namespace-Klassen. Stattdessen werden Namespaces für verschiedene Rollen konfiguriert:

| Rolle | Zweck |
|---|---|
| System Namespace | vollständige Sicht des laufenden Systems |
| Service Namespace | begrenzte Sicht eines Systemdienstes |
| Session Namespace | gemeinsame Ressourcen einer Anmeldung |
| User Namespace | benutzerspezifische Dateisystemansicht |
| Job Namespace | gemeinsame Sicht einer Prozessgruppe |
| Application Namespace | isolierte Anwendungsumgebung |
| Recovery Namespace | getrennte Wiederherstellungsumgebung |

Die Rollen bestimmen Standardrichtlinien, sind aber kein Ersatz für Capability-Prüfungen.

---

## 5. Namespace-Objekt

Ein Namespace ist ein Kernelobjekt:

```c
typedef struct vfs_namespace_info {
    uint64_t namespace_id;
    uint64_t generation;
    uint64_t root_mount_id;
    uint64_t parent_namespace_id;
    uint32_t flags;
    uint32_t mount_count;
} vfs_namespace_info_t;
```

Der Namespace enthält mindestens:

- Namespace-ID
- Root-Mount
- Mount-Baum
- Generationsnummer
- optionalen Parent-Namespace
- Propagationsrichtlinien
- Sicherheitskontext
- Referenzanzahl

---

## 6. Prozesszuordnung

Bei der Prozesserzeugung erhält der neue Prozess standardmäßig den Namespace seines Elternprozesses oder einen durch den Process Manager vorgegebenen Namespace.

Mögliche Varianten:

- Namespace gemeinsam verwenden
- Namespace ableiten
- vollständig neuen Namespace erzeugen
- durch Job Object vorgegebenen Namespace verwenden

Der Namespace-Wechsel eines laufenden Prozesses benötigt eine ausdrückliche Capability und darf nur an einem sicheren Prozesszustand erfolgen.

---

## 7. Namespace-Erzeugung

Ein neuer Namespace kann von einem bestehenden Namespace abgeleitet werden:

```c
status_t vfs_namespace_clone(
    cap_handle_t source_namespace,
    uint32_t flags,
    cap_handle_t *new_namespace
);
```

Mögliche Flags:

```c
#define VFS_NS_CLONE_PRIVATE_MOUNTS  (1u << 0)
#define VFS_NS_READ_ONLY             (1u << 1)
#define VFS_NS_RESTRICTED_ROOT       (1u << 2)
#define VFS_NS_NO_PROPAGATION        (1u << 3)
```

Die Ableitung verwendet Copy-on-Write-Metadaten für den Mount-Baum. Unveränderte Teile können sicher gemeinsam referenziert werden.

---

## 8. Namespace-Root

Jeder Namespace besitzt einen eigenen logischen Root.

Dieser Root begrenzt:

- absolute Pfadauflösung
- `..`-Navigation
- symbolische Links
- Mountsichtbarkeit
- Prozessarbeitsverzeichnisse

Ein Prozess darf weder über `..` noch über symbolische Links oder Mounttraversierung oberhalb seines Namespace-Roots gelangen.

Der Namespace-Root ist eine Sicherheitsgrenze und nicht nur eine kosmetische Pfadänderung.

---

## 9. Mount-Sichtbarkeit

Ein Mount ist nur in Namespaces sichtbar, in denen er ausdrücklich veröffentlicht wurde.

Derselbe Mount kann:

- in mehreren Namespaces sichtbar sein,
- dort an unterschiedlichen Pfaden erscheinen,
- in einem Namespace schreibbar und in einem anderen schreibgeschützt erscheinen,
- in einzelnen Namespaces vollständig verborgen bleiben.

Die zugrunde liegende Dateisysteminstanz kann gemeinsam verwendet werden, während Mount-Flags und Sichtbarkeit Namespace-spezifisch bleiben.

---

## 10. Private Mounts

Ein privater Mount ist ausschließlich im aktuellen Namespace und gegebenenfalls seinen ausdrücklich abgeleiteten Namespaces sichtbar.

Typische Anwendungen:

- temporäre Build-Umgebungen
- Anwendungsdaten
- Testdateisysteme
- Paketinstallation
- Updatevorbereitung
- Recovery-Arbeiten

Private Mounts verändern den Parent-Namespace nicht.

---

## 11. Mount-Propagation

NovaOS unterstützt kontrollierte Propagation zwischen verwandten Namespaces.

| Modus | Verhalten |
|---|---|
| `PRIVATE` | Änderungen bleiben lokal |
| `INHERIT_ON_CLONE` | Mount wird beim Ableiten übernommen |
| `SHARED_GROUP` | Änderungen werden innerhalb einer autorisierten Gruppe verteilt |
| `SLAVE` | Änderungen werden nur in einer Richtung übernommen |
| `LOCKED` | Mountstruktur darf nicht verändert werden |

Der Standardmodus für Anwendungs-Namespaces ist `PRIVATE`.

Propagation benötigt eine explizite Beziehung und darf nicht allein aufgrund gemeinsamer Abstammung erfolgen.

---

## 12. Namespace-Wechsel

Ein Prozess kann seinen aktiven Namespace über eine kontrollierte Operation wechseln:

```c
status_t vfs_namespace_enter(
    cap_handle_t namespace_handle
);
```

Vor dem Wechsel prüft der Kernel:

- Namespace-Capability
- Prozess- und Job-Richtlinie
- aktuelles Arbeitsverzeichnis
- Root-Verzeichnis
- aktive Pfadauflösungen
- sicherheitskritische Dateihandles

Bereits geöffnete File Handles bleiben grundsätzlich gültig, da sie eigene Capabilities sind. Eine Sandbox darf den Wechsel deshalb nur erlauben, wenn keine unzulässigen Handles mitgeführt werden.

---

## 13. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `VFS_NS_RIGHT_ENTER` | Namespace als aktive Ansicht verwenden |
| `VFS_NS_RIGHT_CLONE` | Namespace ableiten |
| `VFS_NS_RIGHT_MOUNT` | Mount hinzufügen |
| `VFS_NS_RIGHT_UNMOUNT` | Mount entfernen |
| `VFS_NS_RIGHT_SET_ROOT` | logischen Root festlegen |
| `VFS_NS_RIGHT_PROPAGATE` | Propagationsbeziehungen verwalten |
| `VFS_NS_RIGHT_INSPECT` | Namespace-Metadaten lesen |
| `VFS_NS_RIGHT_ADMIN` | Namespace vollständig verwalten |

Der Besitz einer Namespace-Capability erteilt keine automatischen Rechte auf alle darin sichtbaren Dateien.

---

## 14. Handle-Vererbung

Offene Handles können Namespace-Grenzen überbrücken.

Deshalb gelten folgende Regeln:

- Handles werden nur nach Prozessrichtlinie vererbt.
- `CLOEXEC`-Handles werden beim Programmstart geschlossen.
- Sandbox-Starts verwenden eine explizite Handle-Allowlist.
- Handles außerhalb des neuen Namespace-Roots müssen entfernt oder ausdrücklich genehmigt werden.
- Namespace-Wechsel erweitert niemals bestehende Capability-Rechte.

Die reine Pfadisolation reicht ohne kontrollierte Handle-Vererbung nicht als Sicherheitsgrenze aus.

---

## 15. Pfadauflösung

Jede Pfadauflösung verwendet einen unveränderlichen Namespace-Snapshot oder eine konsistente Generationssicht.

Dadurch darf eine parallele Mountänderung nicht zu einer teilweise alten und teilweise neuen Auflösung führen.

Ändert sich eine relevante Namespace-Generation während einer nicht atomar fortsetzbaren Operation, wird die Auflösung sicher wiederholt oder mit einem eindeutigen Fehler beendet.

---

## 16. Systemupdates

NovaOS kann Namespaces verwenden, um eine neue Systemgeneration vorzubereiten.

Ein möglicher Ablauf:

1. neuen privaten Update-Namespace erzeugen,
2. neue Systemgeneration einbinden,
3. Integrität und Dienste prüfen,
4. System-Mounts atomar umschalten,
5. alte Generation für Rollback verfügbar halten.

Der Namespace-Wechsel ersetzt nicht die notwendigen Prüfungen für ABI-, Prozess- und Dateisystemkonsistenz.

---

## 17. Recovery

Die Recovery-Umgebung erhält einen getrennten Namespace mit:

- eigenem Root,
- schreibgeschütztem Recovery-System,
- kontrolliert eingebundenen Datenträgern,
- eingeschränkter Sicht auf Benutzerdaten,
- unabhängigen Diagnose- und Reparaturwerkzeugen.

Gewöhnliche Prozesse dürfen nicht in den Recovery-Namespace wechseln.

Ein beschädigter System-Namespace darf den verifizierten Recovery-Root nicht verändern können.

---

## 18. Lebenszeit

Ein Namespace bleibt bestehen, solange mindestens eine Referenz existiert durch:

- einen Prozess
- eine Namespace-Capability
- eine Mount- oder Propagationsbeziehung
- einen Systemdienst
- eine interne Kerneloperation

Das Beenden des erzeugenden Prozesses zerstört einen weiterhin verwendeten Namespace nicht automatisch.

Beim endgültigen Zerstören werden private Mounts in definierter Reihenfolge ausgehängt.

---

## 19. Sicherheit

Für VFS Namespaces gelten folgende Regeln:

- Namespace-Zugriffe benötigen Capabilities.
- Ein Namespace begrenzt Pfadsichtbarkeit, ersetzt aber keine Dateirechte.
- `..` und symbolische Links dürfen den Root nicht verlassen.
- Mount-Propagation ist standardmäßig deaktiviert.
- Handle-Vererbung wird gesondert kontrolliert.
- Anwendungen dürfen keine System-Mounts in ihren Namespace importieren.
- Recovery- und System-Namespaces besitzen zusätzliche Schutzrichtlinien.
- Namespace-IDs stellen keine Zugriffsberechtigung dar.

---

## 20. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_INVALID_HANDLE` | Ungültige Namespace-Capability |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_NAMESPACE_BUSY` | Namespace wird noch verwendet |
| `STATUS_INVALID_ROOT` | Root-Verzeichnis ist ungültig |
| `STATUS_PROPAGATION_DENIED` | Propagation ist nicht erlaubt |
| `STATUS_MOUNT_CONFLICT` | Mountstruktur kollidiert |
| `STATUS_HANDLE_ESCAPE` | Unzulässiges Handle würde die Isolation umgehen |
| `STATUS_GENERATION_CHANGED` | Namespace wurde während der Operation verändert |
| `STATUS_LIMIT_EXCEEDED` | Namespace- oder Mountlimit überschritten |

---

## 21. Audit und Diagnose

Auditiert werden insbesondere:

- Erzeugung privilegierter Namespaces
- Wechsel in System- oder Recovery-Namespaces
- Änderungen des Namespace-Roots
- Mount-Propagation
- Import geschützter Mounts
- abgelehnte Handle-Übernahmen
- administrative Namespace-Zerstörung

Diagnosewerkzeuge dürfen anzeigen:

- Namespace-Hierarchie
- Mountstruktur
- Root-Mount
- Propagationsmodi
- zugeordnete Prozesse
- Generationsnummer
- Sicherheitsrichtlinie

Dateiinhalte werden nicht durch die Namespace-Diagnose erfasst.

---

## 22. Alternative Ansätze

### 22.1 Ein globaler VFS Namespace

**Vorteile:**

- einfache Verwaltung
- geringe Metadatenkosten

**Nachteile:**

- keine Dateisystemisolation
- ungeeignet für Anwendungen, Recovery und Updates

**Entscheidung:** Abgelehnt.

### 22.2 Reine Pfadpräfix-Sandbox

**Vorteile:**

- einfache Implementierung

**Nachteile:**

- anfällig für Links, Mountwechsel und offene Handles
- keine echte Mount-Isolation

**Entscheidung:** Abgelehnt.

### 22.3 Vollständige Kopie jedes Mount-Baums

**Vorteile:**

- einfache Isolation

**Nachteile:**

- hoher Speicher- und Verwaltungsaufwand
- schlechte Skalierung

**Entscheidung:** Abgelehnt. NovaOS verwendet Copy-on-Write-Metadaten.

---

## 23. Konsequenzen

### Positive Konsequenzen

- getrennte Dateisystemansichten
- Grundlage für Anwendungs-Sandboxing
- sichere Recovery-Umgebung
- kontrollierte Mount-Propagation
- effiziente Namespace-Ableitung
- Unterstützung atomarer Systemgenerationen
- Integration in Capabilities und Job Objects

### Negative Konsequenzen

- zusätzliche Komplexität bei Pfadauflösung und Mountverwaltung
- offene Handles können Isolation umgehen, wenn sie falsch übergeben werden
- Propagationsbeziehungen benötigen sorgfältige Synchronisation
- Diagnose mehrerer Namespaces wird aufwendiger

### Risiken

- Handle-Leaks über Namespace-Grenzen
- inkonsistente Mountsicht bei parallelen Änderungen
- falsch konfigurierte Propagation
- unzulässige Root-Überschreitung
- Ressourcenerschöpfung durch viele Namespaces

---

## 24. Implementierungsrichtlinien

- Jeder Prozess besitzt genau einen aktiven VFS Namespace.
- Namespaces werden ausschließlich über Capabilities verwaltet.
- Der Namespace-Root muss bei jedem Pfadübergang durchgesetzt werden.
- Mount-Propagation ist standardmäßig `PRIVATE`.
- Ableitungen verwenden Copy-on-Write-Metadaten.
- Namespace-Wechsel prüfen vorhandene File Handles.
- Pfadauflösungen verwenden eine konsistente Generationssicht.
- Mountänderungen werden atomar veröffentlicht.
- Recovery-Namespaces dürfen nicht aus gewöhnlichen Prozessen betreten werden.

---

## 25. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Namespace-Erzeugung und Ableitung
- getrennte Mountsicht
- private und propagierte Mounts
- `..` und symbolische Links
- Namespace-Wechsel
- Handle-Vererbung
- Copy-on-Write-Mountbäume
- parallele Mountänderungen
- Namespace-Zerstörung
- Job- und Anwendungs-Namespaces
- Recovery-Isolation
- atomaren Systemwechsel
- Ressourcenlimits
- Generationswechsel

---

## 26. Verbindliche Invarianten

1. Jeder Prozess besitzt genau einen aktiven VFS Namespace.
2. Ein Namespace definiert Root und sichtbaren Mount-Baum.
3. Namespace-IDs stellen keine Zugriffsberechtigung dar.
4. `..`, symbolische Links und Mountübergänge dürfen den Root nicht umgehen.
5. Mount-Propagation ist standardmäßig deaktiviert.
6. Namespace-Ableitungen verwenden isolierte Copy-on-Write-Metadaten.
7. Bereits geöffnete Handles bleiben eigenständige Capabilities.
8. Handle-Vererbung muss bei Sandbox- und Namespace-Wechseln geprüft werden.
9. Namespace-Isolation ersetzt keine Datei- und Capability-Rechteprüfung.
10. Recovery-Namespaces bleiben von gewöhnlichen Anwendungen getrennt.

---

## 27. Zusammenfassung

NovaOS verwendet capability-geschützte VFS Namespaces für getrennte Dateisystemansichten.

Jeder Prozess arbeitet innerhalb eines aktiven Namespace mit eigenem Root und sichtbarem Mount-Baum. Namespaces können effizient über Copy-on-Write-Metadaten abgeleitet und für Anwendungen, Jobs, Sessions, Updates oder Recovery eingeschränkt werden.

Die Isolation berücksichtigt neben Pfaden auch Mount-Propagation und bereits geöffnete Handles, damit Namespace-Grenzen nicht durch vorhandene Capabilities umgangen werden.