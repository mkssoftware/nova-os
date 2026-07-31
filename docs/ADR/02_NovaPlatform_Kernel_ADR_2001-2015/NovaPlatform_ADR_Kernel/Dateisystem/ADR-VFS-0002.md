# ADR-VFS-0002 – Mount Manager

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0002 |
| Titel | Mount Manager |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Dateisysteme und Mount-Verwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-VFS-0001, ADR-DEV-0001, ADR-KERNEL-0100, ADR-KERNEL-0103, ADR-IPC-0006 |
| Nachfolgende Spezifikation | NPSPEC-VFS-MOUNT-MANAGER |

---

## 1. Kontext

NovaOS benötigt eine zentrale Komponente für das Einbinden, Aushängen und Verwalten von Dateisysteminstanzen.

Dateisysteme können aus unterschiedlichen Quellen stammen:

- interne Datenträger
- entfernbare Speichermedien
- Netzwerkdateisysteme
- verschlüsselte Volumes
- temporäre Dateisysteme
- Recovery-Systeme
- virtuelle Systemdateisysteme
- unveränderliche Systemgenerationen

Der Mount Manager verbindet diese Dateisysteminstanzen kontrolliert mit den Mount Namespaces des VFS.

---

## 2. Problemstellung

Mount-Vorgänge verändern die für Prozesse sichtbare Dateisystemstruktur und sind daher sicherheitskritisch.

NovaOS muss festlegen:

- wer Dateisysteme einbinden darf,
- wie Dateisystemtypen erkannt werden,
- wie Mountpunkte reserviert werden,
- wie private Namespaces behandelt werden,
- wie offene Dateien beim Unmount reagieren,
- wie Hotplug und verschlüsselte Volumes integriert werden,
- wie System- und Recovery-Mounts geschützt werden.

---

## 3. Entscheidung

NovaOS implementiert einen zentralen Mount Manager als privilegierten Systemdienst mit enger VFS-Integration.

Der Mount Manager verwaltet:

- Dateisystemerkennung
- Mount- und Unmount-Transaktionen
- Mount Namespaces
- Mountabhängigkeiten
- Sicherheitsrichtlinien
- persistente Mountdefinitionen
- automatische Mounts
- Fehler- und Recovery-Verhalten

Die endgültige Veröffentlichung eines Mounts erfolgt atomar durch das VFS.

---

## 4. Architektur

```text
Datenträger oder Quelle
          |
          v
    Mount Manager
      /    |     \
     v     v      v
Erkennung Policy Dateisystemtreiber
          |
          v
      VFS Mount
          |
          v
   Mount Namespace
```

Anwendungen greifen nicht direkt auf den Mount Manager zu, sofern sie keine besonderen Namespace- oder Verwaltungsrechte besitzen.

---

## 5. Mount-Objekt

Jeder aktive Mount wird durch ein Kernelobjekt repräsentiert.

```c
typedef struct mount_info {
    uint64_t mount_id;
    uint64_t namespace_id;
    uint64_t filesystem_instance_id;
    uint64_t parent_mount_id;
    uint32_t flags;
    uint32_t state;
    uint64_t generation;
} mount_info_t;
```

Ein Mount enthält mindestens:

- Dateisysteminstanz
- Mountpunkt
- Mount Namespace
- Parent-Mount
- Mount-Flags
- Lebenszykluszustand
- Sicherheitskontext
- Referenz- und Abhängigkeitsinformationen

---

## 6. Mount-Zustände

Ein Mount durchläuft folgende Zustände:

```text
DISCOVERED
    |
    v
PREPARING
    |
    v
MOUNTED
    |
    v
UNMOUNTING
    |
    v
UNMOUNTED
```

Fehlerhafte Mount-Vorgänge wechseln in `FAILED` und werden nicht teilweise im Namespace sichtbar.

Ein Mount darf erst nach erfolgreicher Initialisierung des Dateisystems veröffentlicht werden.

---

## 7. Mount-Quellen

Unterstützte Quellen sind mindestens:

| Quelle | Beispiel |
|---|---|
| Block Device | Partition oder vollständiger Datenträger |
| Memory Object | RAM-basiertes Dateisystem |
| Netzwerk-Endpunkt | NovaFS Network, NFS oder andere Gateways |
| System Image | schreibgeschütztes Systemabbild |
| Virtuelle Quelle | Prozess-, Geräte- oder Diagnoseansicht |
| Subvolume | NovaFS-Subvolume oder Snapshot |

Jede Quelle wird über eine Capability übergeben.

Gerätenamen oder Pfadzeichenketten allein erteilen keine Berechtigung zum Mounten.

---

## 8. Dateisystemerkennung

Der Mount Manager erkennt Dateisysteme anhand kontrollierter Erkennungsregeln:

- explizit angegebener Dateisystemtyp
- Partitionsmetadaten
- signierte Volume-Metadaten
- bekannte Superblock-Signaturen
- Container- oder Verschlüsselungsheader
- administrativ definierte Richtlinien

Die Erkennung liest nur eine begrenzte Datenmenge und behandelt alle Metadaten als nicht vertrauenswürdig.

Bei mehrdeutiger Erkennung wird nicht automatisch geraten. Der Mount-Vorgang wird abgelehnt oder benötigt eine ausdrückliche Typangabe.

---

## 9. Mount-Flags

Der Mount Manager unterstützt mindestens:

```c
#define MOUNT_READ_ONLY       (1u << 0)
#define MOUNT_NO_EXEC         (1u << 1)
#define MOUNT_NO_DEVICES      (1u << 2)
#define MOUNT_NO_SUID         (1u << 3)
#define MOUNT_PRIVATE         (1u << 4)
#define MOUNT_NO_AUTOMOUNT    (1u << 5)
#define MOUNT_SYNCHRONOUS     (1u << 6)
#define MOUNT_RECOVERY        (1u << 7)
```

Sicherheitsflags können durch untergeordnete Mounts verschärft, aber ohne entsprechende Berechtigung nicht abgeschwächt werden.

---

## 10. Mount Namespaces

Mounts existieren immer innerhalb eines Mount Namespace.

NovaOS unterstützt:

- System-Namespace
- Session-Namespace
- Benutzer-Namespace
- Job-Namespace
- Anwendungs-Namespace
- Recovery-Namespace

Ein neuer Namespace kann von einem bestehenden Namespace abgeleitet werden.

Anschließende Änderungen können je nach Richtlinie:

- privat bleiben,
- ausgewählte Mounts übernehmen,
- kontrolliert weitergegeben werden.

Unkontrollierte globale Mount-Propagation ist nicht zulässig.

---

## 11. Mount-API

```c
status_t mount_create(
    cap_handle_t namespace_handle,
    cap_handle_t source_handle,
    cap_handle_t target_directory,
    const char *filesystem_type,
    uint32_t flags,
    cap_handle_t *mount_handle
);
```

Der Ablauf umfasst:

1. Rechte prüfen.
2. Quelle validieren.
3. Dateisystemtyp bestimmen.
4. Treiber auswählen.
5. Dateisysteminstanz initialisieren.
6. Mountpunkt reservieren.
7. Sicherheitsrichtlinie anwenden.
8. Mount atomar veröffentlichen.

Schlägt ein Schritt fehl, werden alle temporären Ressourcen freigegeben.

---

## 12. Mountpunkte

Ein Mountpunkt ist ein vorhandenes Verzeichnis innerhalb des Ziel-Namespace.

Während des Mounts wird der bisherige Verzeichnisinhalt nicht gelöscht. Er wird lediglich durch den eingebundenen Mount verdeckt.

Nach dem Unmount wird der ursprüngliche Inhalt wieder sichtbar.

Ein Verzeichnis kann innerhalb desselben Namespace nicht gleichzeitig durch mehrere konkurrierende Mounts belegt werden.

---

## 13. Automatisches Mounten

Entfernbare Datenträger können automatisch erkannt, aber nicht zwangsläufig sofort vollständig eingebunden werden.

Der automatische Ablauf kann umfassen:

1. Gerät erkennen.
2. Partitionen untersuchen.
3. Dateisystemtyp bestimmen.
4. Sicherheitsprüfung durchführen.
5. optional Verschlüsselung entsperren.
6. Mount mit eingeschränkten Flags erzeugen.
7. Benutzer oder Session benachrichtigen.

Externe Datenträger werden standardmäßig mit restriktiven Flags wie `NO_EXEC`, `NO_DEVICES` und `NO_SUID` eingebunden.

---

## 14. Persistente Mountdefinitionen

System- und Benutzermounts können deklarativ gespeichert werden.

Eine Definition enthält:

- stabile Volume-ID
- gewünschten Mountpunkt
- Dateisystemtyp
- Mount-Flags
- Abhängigkeiten
- Entsperrrichtlinie
- Fehlerstrategie
- zuständigen Namespace

Flüchtige Gerätepfade wie Busnummern oder Anschlussreihenfolgen dürfen nicht als alleinige dauerhafte Identität verwendet werden.

---

## 15. Abhängigkeiten

Mounts können voneinander abhängen.

Beispiele:

- ein Subvolume benötigt sein Basisdateisystem,
- ein Netzwerk-Mount benötigt das Netzwerk,
- ein verschlüsseltes Dateisystem benötigt einen entsperrten Container,
- ein Overlay benötigt mehrere darunterliegende Mounts,
- ein Benutzer-Mount benötigt die aktive Benutzersitzung.

Abhängigkeiten bestimmen Mount-, Unmount-, Suspend- und Shutdown-Reihenfolgen.

Zyklische Abhängigkeiten werden abgelehnt.

---

## 16. Unmount

Ein regulärer Unmount wird über folgende API angefordert:

```c
status_t mount_unmount(
    cap_handle_t mount_handle,
    uint32_t flags,
    uint64_t timeout_ns
);
```

Vor dem Unmount prüft der Mount Manager:

- offene Handles
- untergeordnete Mounts
- laufende I/O
- ausstehende Schreibvorgänge
- aktuelle Arbeitsverzeichnisse
- Memory Mappings
- aktive Systemabhängigkeiten

Ist der Mount noch in Verwendung, liefert die Operation standardmäßig `STATUS_MOUNT_BUSY`.

---

## 17. Erzwungener und verzögerter Unmount

Unterstützte Sondermodi sind:

| Modus | Verhalten |
|---|---|
| Graceful | wartet auf sichere Freigabe |
| Lazy | entfernt Mount aus dem Namespace, hält bestehende Referenzen |
| Force | widerruft Zugriffe und bricht Operationen ab |
| Emergency | minimale sichere Trennung bei Geräteausfall |

Ein Force- oder Emergency-Unmount benötigt besondere Rechte.

Bereits bestätigte Schreibvorgänge dürfen nicht fälschlich als dauerhaft gespeichert dargestellt werden.

---

## 18. Hot-Unplug

Wird ein Datenträger überraschend entfernt:

1. markiert der Mount Manager die Quelle als verloren,
2. stoppt das VFS neue Operationen,
3. beendet laufende Zugriffe mit Fehler,
4. invalidiert betroffene Caches,
5. entfernt oder deaktiviert abhängige Mounts,
6. informiert Anwendungen und Benutzer,
7. erfasst den Zustand für Diagnose und Recovery.

Das System darf nicht versuchen, weiter auf nicht mehr vorhandene Hardware zuzugreifen.

---

## 19. System- und Recovery-Mounts

Kritische Mounts erhalten besondere Schutzklassen:

- Boot-Mount
- System-Mount
- Benutzer-Daten-Mount
- Recovery-Mount
- temporärer Update-Mount

Systemgenerationen werden bevorzugt schreibgeschützt und `NO_DEVICES` eingebunden.

Der Recovery-Mount muss von gewöhnlichen Benutzer- und Anwendungs-Namespaces getrennt bleiben.

Änderungen an kritischen Mounts benötigen System- oder Recovery-Capabilities.

---

## 20. Sicherheit

Für den Mount Manager gelten folgende Regeln:

- Mount- und Unmount-Operationen benötigen Capabilities.
- Jede Quelle wird vor der Verwendung validiert.
- Externe Dateisystemmetadaten gelten als nicht vertrauenswürdig.
- Mount-Flags dürfen nicht stillschweigend abgeschwächt werden.
- Anwendungen erhalten keine automatische Sicht auf alle Datenträger.
- Mount Namespaces isolieren System-, Benutzer- und Anwendungsbereiche.
- Ausführbare Inhalte externer Medien sind standardmäßig blockiert.
- Dateisystemtreiber laufen nach Möglichkeit isoliert.

Ein erfolgreicher Mount bedeutet nicht, dass alle Inhalte des Dateisystems vertrauenswürdig sind.

---

## 21. Fehlercodes

| Fehlercode | Bedeutung |
|---|---|
| `STATUS_OK` | Operation erfolgreich |
| `STATUS_ACCESS_DENIED` | Erforderliches Recht fehlt |
| `STATUS_INVALID_SOURCE` | Mount-Quelle ist ungültig |
| `STATUS_FILESYSTEM_UNKNOWN` | Dateisystemtyp wurde nicht erkannt |
| `STATUS_FILESYSTEM_CORRUPT` | Dateisystem ist beschädigt |
| `STATUS_MOUNT_POINT_BUSY` | Mountpunkt ist bereits belegt |
| `STATUS_MOUNT_BUSY` | Mount wird noch verwendet |
| `STATUS_DEPENDENCY_FAILED` | Notwendige Abhängigkeit fehlt |
| `STATUS_DEVICE_REMOVED` | Quelldatenträger wurde entfernt |
| `STATUS_READ_ONLY` | Schreibender Mount ist nicht möglich |
| `STATUS_ALREADY_MOUNTED` | Quelle ist bereits entsprechend eingebunden |
| `STATUS_TIMEOUT` | Mount- oder Unmount-Vorgang überschritt das Zeitlimit |

---

## 22. Audit und Diagnose

Auditiert werden insbesondere:

- Mounten externer Datenträger
- Mounts mit Ausführungsrechten
- Force- und Emergency-Unmounts
- Änderungen kritischer System-Mounts
- fehlgeschlagene Integritätsprüfungen
- Entsperren verschlüsselter Volumes
- unerwarteter Verlust eines Datenträgers
- administrative Namespaceänderungen

Normale Dateiinhalte werden nicht protokolliert.

Diagnoseinformationen enthalten Mountstatus, Dateisystemtyp, Flags, Abhängigkeiten und Fehlerursachen.

---

## 23. Alternative Ansätze

### 23.1 Mount-Verwaltung vollständig im Kernel

**Vorteile:**

- kurze interne Datenwege
- einfache Kontrolle zentraler Strukturen

**Nachteile:**

- größere Kernel-Angriffsfläche
- komplexe Richtlinienlogik im Kernel

**Entscheidung:** Das VFS veröffentlicht Mounts im Kernel; Richtlinien und Orchestrierung liegen im privilegierten Mount-Manager-Dienst.

### 23.2 Ein globaler Mount-Baum

**Vorteile:**

- einfache Administration

**Nachteile:**

- keine Isolation für Anwendungen, Benutzer oder Recovery
- unnötige Sichtbarkeit von Datenträgern

**Entscheidung:** Abgelehnt.

### 23.3 Jedes erkannte Medium automatisch beschreibbar mounten

**Vorteile:**

- hohe Benutzerfreundlichkeit

**Nachteile:**

- Sicherheits- und Datenintegritätsrisiken
- unerwünschte automatische Ausführung

**Entscheidung:** Abgelehnt. Externe Medien erhalten zunächst restriktive Mount-Flags.

---

## 24. Konsequenzen

### Positive Konsequenzen

- zentrale und konsistente Mount-Verwaltung
- getrennte Mount Namespaces
- sichere automatische Medienerkennung
- transaktionale Mount-Vorgänge
- definierte Hot-Unplug-Behandlung
- Schutz kritischer System- und Recovery-Mounts
- nachvollziehbare Abhängigkeiten

### Negative Konsequenzen

- zusätzliche Kommunikation zwischen VFS und Systemdienst
- komplexe Namespace- und Abhängigkeitsverwaltung
- Force-Unmount kann Anwendungen unerwartet unterbrechen
- automatische Erkennung benötigt robuste Dateisystemparser

### Risiken

- Race Conditions zwischen Mount und Pfadauflösung
- offene Handles beim Unmount
- zyklische Abhängigkeiten
- beschädigte externe Dateisysteme
- falsche automatische Dateisystemerkennung
- Datenverlust durch unerwartetes Entfernen

---

## 25. Implementierungsrichtlinien

- Mounts werden erst nach vollständiger Initialisierung atomar veröffentlicht.
- Fehlgeschlagene Mount-Vorgänge müssen vollständig zurückgerollt werden.
- Mount-Quellen werden ausschließlich über Capabilities angesprochen.
- Externe Datenträger erhalten restriktive Standardflags.
- Abhängigkeiten müssen vor dem Mount validiert werden.
- Unmount verarbeitet untergeordnete Mounts zuerst.
- Cacheinvalidierung muss vor Abschluss eines Force-Unmount erfolgen.
- System- und Recovery-Mounts erhalten gesonderte Schutzrichtlinien.
- Mount-Flags werden vom VFS unabhängig vom Dateisystemtreiber erzwungen.

---

## 26. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Mount und Unmount
- Mount Namespaces
- parallele Mount-Vorgänge
- belegte Mountpunkte
- Dateisystemerkennung
- beschädigte Volumes
- automatische externe Mounts
- `NO_EXEC` und Read-only
- Lazy-, Force- und Emergency-Unmount
- Hot-Unplug
- offene Handles und Memory Mappings
- Mountabhängigkeiten
- System- und Recovery-Mounts
- Treiberabstürze
- transaktionales Rollback

---

## 27. Verbindliche Invarianten

1. Mount-Verwaltung erfolgt zentral durch den Mount Manager und das VFS.
2. Jede Mount-Quelle wird über eine Capability referenziert.
3. Mounts existieren immer innerhalb eines Mount Namespace.
4. Ein Mount wird erst nach erfolgreicher Initialisierung sichtbar.
5. Fehlgeschlagene Mounts hinterlassen keinen teilweise sichtbaren Zustand.
6. Sicherheitsflags dürfen ohne besondere Berechtigung nicht abgeschwächt werden.
7. Untergeordnete Mounts werden vor ihren Parent-Mounts entfernt.
8. Externe Medien werden standardmäßig restriktiv eingebunden.
9. Hot-Unplug beendet weitere Zugriffe auf die entfernte Quelle.
10. Recovery-Mounts bleiben von gewöhnlichen Anwendungs-Namespaces getrennt.

---

## 28. Zusammenfassung

NovaOS verwendet einen zentralen Mount Manager zur Erkennung, Einbindung und Entfernung von Dateisysteminstanzen.

Der Mount Manager orchestriert Dateisystemtreiber, Sicherheitsrichtlinien, Abhängigkeiten und Mount Namespaces. Das VFS veröffentlicht erfolgreich vorbereitete Mounts atomar und setzt deren Schutzflags durch.

Dadurch können lokale, entfernbare, verschlüsselte, virtuelle und netzwerkbasierte Dateisysteme kontrolliert in unterschiedliche System- und Anwendungsumgebungen eingebunden werden.