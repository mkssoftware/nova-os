# ADR-VFS-0004 – File Cache

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0004 |
| Titel | File Cache |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Virtual File System und Speicherverwaltung |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-VFS-0001, ADR-VFS-0002, ADR-VFS-0003, ADR-MM-0001, ADR-MM-0012 |
| Nachfolgende Spezifikation | NPSPEC-VFS-FILE-CACHE |

---

## 1. Kontext

Dateizugriffe gehören zu den häufigsten I/O-Operationen eines Betriebssystems. Das wiederholte Lesen unveränderter oder häufig verwendeter Daten vom Speichermedium verursacht unnötige Latenzen und Gerätezugriffe.

NovaOS benötigt einen zentralen File Cache für:

- Dateiinhalte
- ausführbare Programme
- Bibliotheken
- Dateisystemmetadaten
- Verzeichniseinträge
- Memory-Mapped Files
- Schreibpufferung

Der Cache muss eng mit VFS, Memory Manager und Dateisystemtreibern zusammenarbeiten.

---

## 2. Problemstellung

Ein File Cache verbessert die Leistung, kann aber Speicher belegen, der von Anwendungen benötigt wird. Schreibpufferung birgt außerdem das Risiko, dass bestätigte Daten bei einem Absturz noch nicht dauerhaft gespeichert wurden.

NovaOS muss daher festlegen:

- welche Daten zwischengespeichert werden,
- wie Cache-Seiten identifiziert werden,
- wie Lese- und Memory-Mappings dieselben Seiten verwenden,
- wann geänderte Seiten geschrieben werden,
- wie Speicher zurückgewonnen wird,
- wie Caches invalidiert werden,
- welche Persistenzgarantien gelten.

---

## 3. Entscheidung

NovaOS implementiert einen global koordinierten, seitengestützten File Cache.

Dateizugriffe und Memory-Mapped Files verwenden nach Möglichkeit dieselben physischen Cache-Seiten. Dadurch werden doppelte Kopien derselben Dateidaten vermieden.

Der Cache ist global koordiniert, aber Ressourcenverbrauch, Sicherheit und Rückgewinnung werden pro Prozess, Job, Dateisystem und NUMA-Knoten berücksichtigt.

---

## 4. Architektur

```text
VFS-Lesezugriff ─────┐
                     v
Memory Mapping ──> File Cache ──> Dateisystemtreiber
                     |
                     v
               Memory Manager
```

Das VFS verwaltet Dateiidentität und Berechtigungen. Der Memory Manager verwaltet die physischen Seiten. Der Dateisystemtreiber liest und schreibt die dauerhaften Daten.

---

## 5. Cache-Identität

Eine Cache-Seite wird mindestens durch folgende Werte identifiziert:

```text
Dateisysteminstanz
Dateiobjekt-ID
Dateioffset
Systemgeneration
```

Pfadnamen werden nicht als Cache-Schlüssel verwendet, da Dateien umbenannt oder über mehrere Namen erreichbar sein können.

Snapshots und verschiedene Dateisystemgenerationen müssen getrennte Cache-Identitäten erhalten.

---

## 6. Cache-Seitenzustände

Eine Cache-Seite kann folgende Zustände besitzen:

| Zustand | Bedeutung |
|---|---|
| `EMPTY` | Seite enthält noch keine gültigen Daten |
| `LOADING` | Daten werden eingelesen |
| `CLEAN` | Daten entsprechen dem Dateisystem |
| `DIRTY` | Daten wurden verändert |
| `WRITEBACK` | Daten werden geschrieben |
| `ERROR` | Lesen oder Schreiben ist fehlgeschlagen |
| `INVALID` | Seite darf nicht mehr verwendet werden |

Zustandsübergänge müssen atomar erfolgen.

Mehrere Leser derselben noch nicht geladenen Seite warten auf denselben I/O-Vorgang.

---

## 7. Lesezugriff

Bei einem Dateilesezugriff:

1. ermittelt das VFS die betroffenen Cache-Seiten,
2. sucht der Cache vorhandene Seiten,
3. lädt fehlende Seiten über den Dateisystemtreiber,
4. wartet bei bereits laufendem Laden,
5. kopiert oder mappt die angeforderten Daten,
6. aktualisiert Nutzungsinformationen.

Ein Cache-Treffer vermeidet einen erneuten Zugriff auf das Speichermedium.

Teilseiten am Dateiende werden außerhalb der gültigen Dateigröße mit null behandelt und nicht als fremde Daten offengelegt.

---

## 8. Schreibzugriff

Schreibvorgänge verändern die zugehörigen Cache-Seiten und markieren sie als `DIRTY`.

Abhängig vom Öffnungs- und Mountmodus kann ein Schreibvorgang:

- verzögert zurückgeschrieben werden,
- sofortiges Writeback anfordern,
- synchron bis zur dauerhaften Speicherung warten,
- auf einem schreibgeschützten Mount abgelehnt werden.

Ein erfolgreiches gewöhnliches `write()` garantiert nur die Annahme durch das VFS und nicht zwingend die physische Persistenz.

---

## 9. Writeback

Dirty Pages werden zurückgeschrieben bei:

- Überschreitung eines Dirty-Limits,
- Erreichen eines Alterslimits,
- explizitem `flush` oder `sync`,
- Speicherdruck,
- Unmount,
- Suspend oder Shutdown,
- dateisystemspezifischem Transaktionsabschluss.

Das Writeback erfolgt bevorzugt gebündelt und in einer für das Dateisystem geeigneten Reihenfolge.

Dateisysteme mit Transaktionen oder Copy-on-Write bestimmen die endgültige Persistenzreihenfolge.

---

## 10. Flush- und Sync-Semantik

NovaOS unterscheidet:

| Operation | Garantie |
|---|---|
| `write()` | Daten wurden vom VFS angenommen |
| `flush()` | Dirty Pages der Datei wurden an das Dateisystem übergeben |
| `fsync()` | Dateidaten und erforderliche Metadaten wurden dauerhaft bestätigt |
| `sync()` | Dirty Data einer Dateisysteminstanz wurde synchronisiert |

Die tatsächliche Persistenzgarantie hängt zusätzlich von den Fähigkeiten des Speichergeräts und des Dateisystems ab.

Fehler beim Writeback müssen an den betroffenen File Handles und Dateisystemstatus weitergegeben werden.

---

## 11. Memory-Mapped Files

Memory-Mapped Files verwenden dieselben Cache-Seiten wie gewöhnliche Datei-I/O.

Dadurch gilt:

- Änderungen eines schreibbaren gemeinsamen Mappings werden im File Cache sichtbar.
- Lesezugriffe sehen konsistente Cache-Daten.
- private Copy-on-Write-Mappings erhalten bei Änderungen eigene Seiten.
- Dateiverkürzungen invalidieren Mappings außerhalb der neuen Größe.
- `msync` fordert die Synchronisation eines Mappingbereichs an.

Die genauen Sichtbarkeitsregeln werden durch die Memory-Mapping-Spezifikation festgelegt.

---

## 12. Read-Ahead

Der File Cache darf sequenzielle Zugriffsmuster erkennen und zusätzliche Seiten vorab laden.

Read-Ahead berücksichtigt:

- Zugriffsmuster
- Dateigröße
- Speicherdruck
- Gerätegeschwindigkeit
- Prozesspriorität
- Energieprofil
- Netzwerk- oder lokale Quelle

Bei zufälligen Zugriffen oder hohem Speicherdruck wird Read-Ahead reduziert.

Vorab geladene Daten dürfen aktiv angeforderte I/O-Operationen nicht unangemessen verdrängen.

---

## 13. Cache-Rückgewinnung

Clean Pages können bei Speicherdruck ohne Writeback verworfen werden.

Dirty Pages müssen zunächst geschrieben oder kontrolliert einer Fehlerbehandlung zugeführt werden.

Die Auswahl berücksichtigt:

- letzte Nutzung
- Zugriffshäufigkeit
- Dateityp
- Prozess- und Job-Priorität
- NUMA-Lokalität
- Kosten eines erneuten Ladens
- Dirty- und Writeback-Zustand
- aktive Mappings

Der File Cache darf nicht dauerhaft Speicher blockieren, den das System für wichtigere Aufgaben benötigt.

---

## 14. NUMA

Auf NUMA-Systemen werden Cache-Seiten bevorzugt auf dem Knoten angelegt, auf dem sie zuerst aktiv verwendet werden.

Der Memory Manager darf Seiten verschieben oder replizieren, wenn:

- Zugriffsmuster dies rechtfertigen,
- die Dateisemantik erhalten bleibt,
- keine aktiven DMA-Bindungen entgegenstehen,
- Speicherdruck ausgeglichen werden muss.

Mehrere NUMA-Kopien dürfen nicht zu auseinanderlaufenden Dateidaten führen.

---

## 15. Metadaten- und Verzeichniscache

Zusätzlich zu Dateiinhalten kann das VFS zwischenspeichern:

- VNodes
- Inode-ähnliche Metadaten
- Verzeichniseinträge
- negative Namensauflösungen
- Dateiattribute
- symbolische Linkziele

Metadatencaches besitzen eigene Invalidierungs- und Lebenszeitregeln.

Negative Einträge müssen bei Dateierzeugung oder externen Änderungen zuverlässig invalidiert werden.

---

## 16. Cache-Invalidierung

Cache-Einträge werden invalidiert bei:

- Dateiverkürzung
- Dateilöschung
- extern erkannter Änderung
- Snapshot- oder Generationswechsel
- Unmount
- Dateisystemfehler
- Verlust eines Netzwerkservers
- Medienwechsel
- Widerruf der Dateisysteminstanz

Dirty Pages dürfen nicht stillschweigend verworfen werden, sofern kein ausdrücklicher Fehler- oder Emergency-Pfad aktiv ist.

---

## 17. Netzwerkdateisysteme

Bei Netzwerkdateisystemen hängt Cache-Kohärenz vom verwendeten Protokoll ab.

Unterstützte Strategien können sein:

- Leasing
- Versionsnummern
- Änderungs-Tokens
- zeitbasierte Gültigkeit
- explizite Invalidierungsnachrichten
- vollständig ungecachter Zugriff

Das VFS darf keine stärkere Kohärenz garantieren, als das Netzwerkdateisystem tatsächlich bereitstellt.

Verbindungsfehler und unklare Writeback-Ergebnisse müssen sichtbar bleiben.

---

## 18. Ressourcenbegrenzung

Der File Cache verwendet dynamische Grenzen abhängig von:

- verfügbarem Arbeitsspeicher
- Systemlast
- NUMA-Verteilung
- Dirty-Page-Anteil
- Job- und Prozessrichtlinien
- Dateisystemtyp
- I/O-Geschwindigkeit

Feste Mindestmengen für Kernel und kritische Dienste dürfen nicht durch Cache-Wachstum verdrängt werden.

Prozesse dürfen über gezielte Dateizugriffe keine unbegrenzte Anzahl gepinnter Cache-Seiten erzwingen.

---

## 19. Sicherheit und Datenschutz

Für den File Cache gelten folgende Regeln:

- Jede neue Dateioperation benötigt weiterhin eine Capability-Prüfung.
- Cache-Treffer umgehen keine Zugriffsrechte.
- Freigegebene Cache-Seiten werden vor anderer Verwendung bereinigt.
- Daten verschiedener Sicherheitsdomänen dürfen nicht versehentlich gemeinsam sichtbar werden.
- Verschlüsselte Dateisysteme speichern nur die durch die Dateisystemschicht freigegebene Klartextdarstellung im geschützten RAM.
- Cache-Inhalte werden nicht standardmäßig protokolliert.
- Dateisystem- und Snapshotgenerationen müssen getrennt bleiben.

---

## 20. Fehlerbehandlung

Mögliche Fehler sind:

| Fehler | Reaktion |
|---|---|
| Lesefehler | Seite erhält `ERROR`, wartende Leser werden informiert |
| Writeback-Fehler | Dirty-Zustand und Fehler bleiben nachvollziehbar |
| Medium entfernt | betroffene Seiten und Handles werden invalidiert |
| Netzwerkverbindung verloren | Operationen liefern transportbezogene Fehler |
| Dateisystem beschädigt | weitere Schreibvorgänge können blockiert werden |
| Speicherknappheit | Read-Ahead stoppen und Clean Pages freigeben |

Ein Writeback-Fehler darf nicht durch einen späteren erfolgreichen Cache-Lesezugriff verborgen werden.

---

## 21. API

Zusätzlich zu normalen VFS-Operationen werden mindestens bereitgestellt:

```c
status_t vfs_file_flush(
    cap_handle_t file_handle
);

status_t vfs_file_sync(
    cap_handle_t file_handle
);

status_t vfs_cache_advise(
    cap_handle_t file_handle,
    uint64_t offset,
    uint64_t length,
    uint32_t advice
);
```

Mögliche Hinweise:

```c
#define VFS_ADVICE_NORMAL      0
#define VFS_ADVICE_SEQUENTIAL  1
#define VFS_ADVICE_RANDOM      2
#define VFS_ADVICE_WILL_NEED   3
#define VFS_ADVICE_DONT_NEED   4
```

Hinweise sind Optimierungen und dürfen die Dateisemantik nicht verändern.

---

## 22. Audit und Diagnose

Der File Cache stellt lokale Statistiken bereit:

- belegter Cache-Speicher
- Clean und Dirty Pages
- Cache-Treffer und Fehltreffer
- Read-Ahead-Aktivität
- Writeback-Mengen und -Latenzen
- Fehler
- Rückgewinnung
- NUMA-Verteilung

Dateinamen und Inhalte werden nicht standardmäßig in globalen Cache-Statistiken erfasst.

Sicherheitsrelevante Writeback- oder Integritätsfehler können auditiert werden.

---

## 23. Alternative Ansätze

### 23.1 Getrennter Cache für Datei-I/O und Memory Mappings

**Vorteile:**

- einfachere getrennte Implementierung

**Nachteile:**

- doppelte Speichernutzung
- schwierige Kohärenz
- unterschiedliche Sicht auf dieselben Daten

**Entscheidung:** Abgelehnt.

### 23.2 Keine Schreibpufferung

**Vorteile:**

- einfache Persistenzsemantik
- geringeres Datenverlustrisiko

**Nachteile:**

- schlechte Schreibleistung
- viele kleine Gerätezugriffe

**Entscheidung:** Abgelehnt. Synchrone Schreibmodi bleiben optional.

### 23.3 Feste Cache-Größe

**Vorteile:**

- vorhersehbarer Speicherverbrauch

**Nachteile:**

- schlechte Anpassung an unterschiedliche Hardware und Lasten

**Entscheidung:** Abgelehnt. Der Cache reagiert dynamisch auf Speicherdruck.

---

## 24. Konsequenzen

### Positive Konsequenzen

- deutlich schnellere wiederholte Dateizugriffe
- gemeinsame Seiten für Datei-I/O und Memory Mappings
- reduzierte Gerätezugriffe
- gebündeltes Writeback
- dynamische Speicherrückgewinnung
- NUMA- und Read-Ahead-Unterstützung
- einheitliche Cache-Semantik für alle Dateisysteme

### Negative Konsequenzen

- komplexe Zustands- und Synchronisationslogik
- Schreibfehler können zeitversetzt auftreten
- zusätzlicher Speicherbedarf
- Netzwerkdateisysteme benötigen besondere Kohärenzregeln

### Risiken

- Datenverlust bei missverstandener Flush-Semantik
- fehlerhafte Cacheinvalidierung
- Dirty-Page-Staus
- doppelte oder veraltete NUMA-Kopien
- Speichererschöpfung durch gepinnte Seiten
- unbemerkte Writeback-Fehler

---

## 25. Implementierungsrichtlinien

- Datei-I/O und Memory Mappings verwenden denselben File Cache.
- Cache-Schlüssel verwenden Objektidentität statt Pfadnamen.
- Cache-Treffer ersetzen keine Capability-Prüfung.
- Dirty Pages dürfen außerhalb eines definierten Emergency-Pfads nicht stillschweigend verworfen werden.
- Writeback-Fehler müssen an Anwendungen weitergegeben werden.
- Read-Ahead wird bei Speicherdruck reduziert.
- Cache-Seiten werden vor sicherheitsdomänenfremder Wiederverwendung bereinigt.
- Dateisystemgenerationen und Snapshots erhalten getrennte Cache-Identitäten.
- Zustandsübergänge einer Cache-Seite erfolgen atomar.

---

## 26. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Cache-Treffer und Fehltreffer
- paralleles Laden derselben Seite
- gewöhnliches und positionsbasiertes I/O
- Dirty Pages und Writeback
- `flush`, `fsync` und `sync`
- Memory-Mapped Files
- Dateiverkürzung
- Datei- und Mount-Invalidierung
- Speicherdruck
- NUMA-Verhalten
- Read-Ahead
- Netzwerkdateisystemfehler
- Writeback-Fehler
- Medienentfernung
- konkurrierendes Lesen und Schreiben

---

## 27. Verbindliche Invarianten

1. Datei-I/O und Memory-Mapped Files verwenden einen gemeinsamen File Cache.
2. Cache-Seiten werden über Dateiidentität, Offset und Generation adressiert.
3. Pfadnamen werden nicht als primäre Cache-Identität verwendet.
4. Cache-Treffer umgehen keine Zugriffsprüfung.
5. Dirty Pages werden nicht ohne definierten Fehlerpfad verworfen.
6. Writeback-Fehler bleiben für betroffene Anwendungen sichtbar.
7. Dateiverkürzung invalidiert Seiten außerhalb der neuen Dateigröße.
8. Cache-Speicher ist unter Speicherdruck grundsätzlich rückgewinnbar.
9. Snapshot- und Systemgenerationen bleiben im Cache getrennt.
10. Dateiinhalte werden nicht standardmäßig diagnostisch protokolliert.

---

## 28. Zusammenfassung

NovaOS implementiert einen global koordinierten, seitengestützten File Cache für Dateiinhalte, Metadaten und Memory-Mapped Files.

Gewöhnliche Dateioperationen und Speicherabbildungen verwenden dieselben Cache-Seiten. Dirty Pages werden kontrolliert zurückgeschrieben, während Clean Pages bei Speicherdruck freigegeben werden können.

Klare Flush-Garantien, sichere Invalidierung und die Weitergabe von Writeback-Fehlern sorgen dafür, dass der Leistungsgewinn nicht auf Kosten einer unehrlichen Persistenz- oder Konsistenzsemantik entsteht.