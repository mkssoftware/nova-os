# ADR-VFS-0005 – Buffer Cache

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0005 |
| Titel | Buffer Cache |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | Block-I/O, VFS und Dateisysteme |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-VFS-0001, ADR-VFS-0004, ADR-DEV-0001, ADR-DEV-0010, ADR-MM-0014 |
| Nachfolgende Spezifikation | NPSPEC-VFS-BUFFER-CACHE |

---

## 1. Kontext

Der File Cache speichert logische Dateiseiten. Dateisysteme und Blocktreiber benötigen zusätzlich einen Cache für blockorientierte Daten, die keiner gewöhnlichen Dateiabbildung entsprechen.

Dazu gehören beispielsweise:

- Superblöcke
- Allokationstabellen
- Bäume und Indizes
- Journaldaten
- Partitionsinformationen
- Bootsektoren
- Dateisystemmetadaten
- kontrollierte Raw-Block-Zugriffe

NovaOS benötigt dafür einen zentral koordinierten Buffer Cache.

---

## 2. Problemstellung

Ein unabhängiger Blockcache kann dieselben Nutzdaten wie der File Cache ein zweites Mal speichern. Dadurch entstehen:

- doppelter Speicherverbrauch
- inkonsistente Datenkopien
- unklare Writeback-Reihenfolgen
- schwer kontrollierbare Persistenz
- unnötige Kopieroperationen

Der Buffer Cache muss deshalb klar vom File Cache abgegrenzt und mit ihm koordiniert werden.

---

## 3. Entscheidung

NovaOS implementiert einen seitengestützten Buffer Cache für Blockgeräte und dateisysteminterne Metadaten.

Der Buffer Cache wird nicht als zweiter allgemeiner Cache für reguläre Dateiinhalte verwendet.

Es gilt:

| Datenart | Primärer Cache |
|---|---|
| reguläre Dateiinhalte | File Cache |
| Memory-Mapped Files | File Cache |
| Dateisystemmetadaten | Buffer Cache oder dateisystemspezifischer Metadatencache |
| Raw-Block-Daten | Buffer Cache |
| DMA-Transferpuffer | DMA Framework |
| Journaldaten | Buffer Cache mit Transaktionsbindung |

Dateisysteme mit eigener integrierter Cacheverwaltung dürfen den generischen Buffer Cache gezielt umgehen.

---

## 4. Architektur

```text
Dateisystemtreiber
       |
       v
  Buffer Cache
       |
       v
 Block-I/O-Layer
       |
       v
 Blockgerätetreiber
```

File Cache und Buffer Cache teilen sich gemeinsame Speicherrückgewinnungs- und I/O-Mechanismen, besitzen aber unterschiedliche Objektidentitäten.

---

## 5. Buffer-Identität

Ein Buffer wird mindestens identifiziert durch:

```text
Blockgeräte-ID
Gerätegeneration
logischer Blockoffset
Bufferlänge
```

Pfadnamen, temporäre Gerätenamen und Treiberzeiger werden nicht als Cache-Schlüssel verwendet.

Nach einem Medienwechsel muss sich die Gerätegeneration ändern, damit alte Buffer nicht für das neue Medium verwendet werden.

---

## 6. Buffer-Struktur

```c
typedef struct buffer_info {
    uint64_t buffer_id;
    uint64_t device_id;
    uint64_t block_offset;
    uint32_t length;
    uint32_t state;
    uint64_t generation;
    uint32_t flags;
} buffer_info_t;
```

Ein Buffer enthält zusätzlich:

- Referenzanzahl
- Speicher- oder Seitenreferenz
- Sperrzustand
- Dirty-Informationen
- Prüfsummenstatus
- Transaktionszuordnung
- I/O-Abschlussobjekt
- Fehlerstatus

---

## 7. Buffer-Zustände

| Zustand | Bedeutung |
|---|---|
| `EMPTY` | Buffer enthält keine gültigen Daten |
| `READING` | Daten werden vom Gerät gelesen |
| `CLEAN` | Daten entsprechen dem Medium |
| `DIRTY` | Daten wurden verändert |
| `WRITEBACK` | Daten werden geschrieben |
| `ERROR` | I/O-Operation ist fehlgeschlagen |
| `INVALID` | Buffer darf nicht weiterverwendet werden |

Zustandsübergänge erfolgen atomar.

Mehrere Anforderungen für denselben Block sollen denselben laufenden I/O-Vorgang verwenden.

---

## 8. Lesezugriff

Ein dateisysteminterner Block wird über den Buffer Cache angefordert:

```c
status_t buffer_cache_read(
    cap_handle_t block_device,
    uint64_t block_offset,
    uint32_t length,
    uint32_t flags,
    cap_handle_t *buffer_handle
);
```

Der Cache:

1. prüft Gerät, Generation und Bereich,
2. sucht einen vorhandenen Buffer,
3. startet bei Bedarf einen Block-I/O-Auftrag,
4. wartet oder liefert ein asynchrones Completion-Objekt,
5. gibt eine eingeschränkte Buffer-Capability zurück.

Ein erfolgreicher Cache-Treffer verursacht keinen erneuten Gerätezugriff.

---

## 9. Schreibzugriff

Änderungen erfolgen über kontrollierte Buffer-Operationen.

```c
status_t buffer_cache_mark_dirty(
    cap_handle_t buffer_handle,
    cap_handle_t transaction_handle
);
```

Dateisystemmetadaten sollen Dirty Buffers einer Dateisystemtransaktion zuordnen.

Ein Buffer darf nicht als erfolgreich persistent gelten, bevor Dateisystem und Blockgerät den notwendigen Abschluss bestätigt haben.

---

## 10. Writeback

Dirty Buffers werden geschrieben bei:

- Transaktions-Commit
- Überschreitung von Dirty-Limits
- explizitem Flush
- Speicherdruck
- Unmount
- Suspend oder Shutdown
- Erreichen eines Alterslimits

Die Writeback-Reihenfolge wird nicht allein vom Buffer Cache entschieden. Journaling- und Copy-on-Write-Dateisysteme geben die erforderlichen Abhängigkeiten vor.

Der Cache darf Metadaten nicht in einer Reihenfolge schreiben, die das Dateisystem inkonsistent macht.

---

## 11. Transaktionsintegration

Das VFS stellt eine Schnittstelle bereit, über die ein Dateisystem Buffer an eine Transaktion bindet.

Eine Transaktion kann festlegen:

- welche Buffer zusammengehören
- welche Reihenfolge erforderlich ist
- welche Schreibbarrieren benötigt werden
- wann Daten als persistent gelten
- welche Buffer nach einem Fehler verworfen werden dürfen

Der Buffer Cache ersetzt weder das Journal noch die Transaktionslogik des Dateisystems.

---

## 12. Blockgrößen

Buffergrößen müssen mit den Fähigkeiten des Blockgeräts kompatibel sein.

Berücksichtigt werden:

- logische Sektorgröße
- physische Sektorgröße
- optimale I/O-Größe
- maximale Transfergröße
- Ausrichtung
- Dateisystemblockgröße

Nicht ausgerichtete Zugriffe können durch Read-Modify-Write verarbeitet werden, sofern dies sicher möglich ist.

Dateisystemtreiber sollen nach Möglichkeit ausgerichtete Vollblockzugriffe verwenden.

---

## 13. Raw-Block-Zugriffe

Raw-Block-Zugriffe sind nur mit besonderen Capabilities zulässig.

Wenn ein Dateisystem auf einem Blockgerät gemountet ist, dürfen gewöhnliche Prozesse keine konkurrierenden Raw-Schreibzugriffe erhalten.

Andernfalls könnten Buffer Cache, File Cache und On-Disk-Zustand auseinanderlaufen.

Administrative Raw-Schreiboperationen erfordern:

- exklusiven Gerätezugriff oder
- einen ausdrücklich koordinierten Wartungsmodus.

---

## 14. Kohärenz mit dem File Cache

Reguläre Dateiinhalte dürfen nicht dauerhaft sowohl im File Cache als auch im Buffer Cache unabhängig gecacht werden.

Dateisystemtreiber müssen für Dateidaten:

- File-Cache-Seiten direkt an den Block-I/O-Layer übergeben oder
- eine eindeutig koordinierte temporäre Buffer-Ansicht verwenden.

Eine temporäre Ansicht darf keine zweite unabhängige Dirty-Kopie erzeugen.

Die kanonische Kopie gewöhnlicher Dateidaten bleibt im File Cache.

---

## 15. DMA und Block-I/O

Für Geräteübertragungen arbeitet der Buffer Cache mit dem DMA Framework zusammen.

Dabei können:

- Cache-Seiten direkt für DMA gemappt,
- Scatter/Gather-Listen erzeugt,
- Bounce Buffer verwendet,
- Cache-Synchronisationen ausgeführt werden.

Das DMA Framework erzeugt Geräteadressen. Der Buffer Cache verwaltet keine beliebigen physischen Adressen.

---

## 16. Cache-Rückgewinnung

Clean Buffers können bei Speicherdruck verworfen werden, sofern:

- keine aktive Referenz besteht,
- keine I/O-Operation läuft,
- keine Transaktion den Buffer benötigt,
- kein DMA-Mapping aktiv ist.

Dirty Buffers müssen vor der Freigabe geschrieben oder einem eindeutigen Fehlerzustand zugeführt werden.

Metadaten kritischer Transaktionen dürfen nicht willkürlich zurückgewonnen werden.

---

## 17. Read-Ahead und Write Combining

Der Buffer Cache darf benachbarte Blockzugriffe zusammenfassen.

Mögliche Optimierungen:

- Metadaten-Read-Ahead
- Zusammenführen benachbarter Lesezugriffe
- Zusammenführen kompatibler Schreibzugriffe
- Sortierung nach Geräteoffset
- Nutzung gerätespezifischer I/O-Queues

Transaktionsgrenzen und Schreibbarrieren dürfen dabei nicht verändert werden.

Optimierungen müssen auf SSDs, Festplatten und Netzwerkblockgeräten unterschiedlich gewichtet werden können.

---

## 18. Medienwechsel und Hot-Unplug

Bei einem Medienwechsel oder Geräteverlust:

1. wird die Gerätegeneration erhöht,
2. werden neue Zugriffe abgelehnt oder dem neuen Medium zugeordnet,
3. werden alte Buffer invalidiert,
4. werden laufende I/O-Operationen beendet,
5. werden Dirty Buffers als nicht sicher persistent markiert,
6. werden Dateisystem und Mount Manager informiert.

Alte Buffer dürfen niemals auf ein neu eingelegtes Medium geschrieben werden.

---

## 19. Fehlerbehandlung

| Fehler | Verhalten |
|---|---|
| Lesefehler | Buffer wechselt zu `ERROR` |
| Schreibfehler | Dirty- und Fehlerstatus bleiben erhalten |
| Gerät entfernt | Buffer wird invalidiert |
| Prüfsummenfehler | Dateisystem erhält Integritätsfehler |
| Timeout | I/O kann wiederholt oder abgebrochen werden |
| Speicherknappheit | Clean Buffers werden zurückgewonnen |
| Teilübertragung | nur vollständig bestätigte Bereiche gelten als gültig |

Ein Writeback-Fehler darf nicht durch späteres Lesen aus dem Cache verborgen werden.

---

## 20. Sicherheit

Für den Buffer Cache gelten folgende Regeln:

- Blockgeräte werden ausschließlich über Capabilities angesprochen.
- Bereichsgrenzen werden vor jeder I/O-Operation geprüft.
- Raw-Schreibzugriffe auf gemountete Geräte sind standardmäßig verboten.
- Wiederverwendete Seiten und Buffer werden bereinigt.
- Cache-Treffer ersetzen keine Zugriffsprüfung.
- Gerätegenerationen verhindern Zugriffe auf gewechselte Medien.
- DMA erfolgt ausschließlich über das DMA Framework.
- Buffer-Inhalte werden nicht standardmäßig protokolliert.

---

## 21. Ressourcenbegrenzung

Der Buffer Cache unterliegt dynamischen und festen Limits für:

- Gesamtspeicher
- Dirty Buffers
- gepinnte Buffer
- aktive I/O-Aufträge
- Buffer pro Blockgerät
- Buffer pro Dateisystemtransaktion
- Read-Ahead-Mengen
- Fehlerwiederholungen

Der File Cache und Buffer Cache müssen gemeinsam auf globalen Speicherdruck reagieren.

Keiner der beiden Caches darf den anderen oder kritische Kernelressourcen unkontrolliert verdrängen.

---

## 22. API

Die grundlegende API umfasst:

```c
status_t buffer_cache_get(
    cap_handle_t block_device,
    uint64_t offset,
    uint32_t length,
    uint32_t flags,
    cap_handle_t *buffer_handle
);

status_t buffer_cache_flush(
    cap_handle_t buffer_handle
);

status_t buffer_cache_invalidate(
    cap_handle_t block_device,
    uint64_t offset,
    uint64_t length
);

status_t buffer_cache_release(
    cap_handle_t buffer_handle
);
```

Administrative Invalidierungen dürfen keine aktiven Dirty Buffers ohne definierte Fehlerbehandlung verwerfen.

---

## 23. Audit und Diagnose

Lokale Diagnoseinformationen umfassen:

- Anzahl und Größe der Buffer
- Clean-, Dirty- und Writeback-Anteile
- Cache-Treffer
- I/O-Zusammenfassungen
- Fehler und Timeouts
- gepinnte Buffer
- Transaktionsbindungen
- Gerätegenerationen

Auditiert werden insbesondere:

- Raw-Schreibversuche auf gemountete Geräte
- verlorene Dirty Buffers
- wiederholte Gerätefehler
- administrative Cacheinvalidierung
- inkonsistente Transaktionsabschlüsse

Blockinhalte werden nicht protokolliert.

---

## 24. Alternative Ansätze

### 24.1 Vollständig getrennter allgemeiner Blockcache

**Vorteile:**

- einfache Blockgeräteabstraktion

**Nachteile:**

- doppelte Dateidaten
- Kohärenzprobleme mit dem File Cache

**Entscheidung:** Abgelehnt. Der Buffer Cache bleibt auf Metadaten und Raw-Blöcke begrenzt.

### 24.2 Kein Buffer Cache

**Vorteile:**

- einfache Kohärenz
- geringer Speicherverbrauch

**Nachteile:**

- wiederholte Metadatenzugriffe
- schlechte Leistung
- erschwerte Transaktionsbündelung

**Entscheidung:** Abgelehnt.

### 24.3 Dateisystemspezifischer Cache ohne gemeinsame Infrastruktur

**Vorteile:**

- maximale Optimierung je Dateisystem

**Nachteile:**

- doppelte Implementierungen
- keine gemeinsame Speicherrückgewinnung

**Entscheidung:** Dateisystemspezifische Erweiterungen sind erlaubt, müssen aber mit der globalen Speicherverwaltung kooperieren.

---

## 25. Konsequenzen

### Positive Konsequenzen

- schnelle Metadaten- und Raw-Block-Zugriffe
- zentrale Block-I/O-Koordination
- Transaktions- und Writeback-Unterstützung
- keine unabhängige zweite Kopie regulärer Dateidaten
- Integration mit DMA und Speicherverwaltung
- sichere Behandlung von Medienwechseln

### Negative Konsequenzen

- klare Trennung zum File Cache erhöht den Schnittstellenaufwand
- Transaktionsreihenfolgen bleiben dateisystemspezifisch
- Cacheinvalidierung ist komplex
- Raw-Block-Zugriffe benötigen strenge Exklusivität

### Risiken

- doppelte Dirty-Kopien
- Schreiben alter Buffer auf neue Medien
- fehlerhafte Transaktionsreihenfolge
- Dirty-Buffer-Stau
- nicht erkannte Writeback-Fehler
- Race Conditions bei Hot-Unplug

---

## 26. Implementierungsrichtlinien

- Reguläre Dateiinhalte verwenden primär den File Cache.
- Buffer-Identitäten enthalten Geräte-ID und Gerätegeneration.
- Dirty Buffers werden nicht ohne definierten Fehlerpfad verworfen.
- Transaktionsreihenfolgen werden vom Dateisystem vorgegeben.
- Raw-Schreibzugriffe auf gemountete Geräte sind standardmäßig verboten.
- DMA-Adressen werden ausschließlich durch das DMA Framework erzeugt.
- Medienwechsel invalidiert sämtliche Buffer der alten Generation.
- File Cache und Buffer Cache reagieren gemeinsam auf Speicherdruck.
- Zustandsübergänge und Referenzverwaltung erfolgen atomar.

---

## 27. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Cache-Treffer und Fehltreffer
- gleichzeitige Anforderung desselben Blocks
- Dirty Buffer und Writeback
- Transaktionsreihenfolge
- Blockgrößen und Ausrichtung
- Read-Modify-Write
- Kohärenz mit dem File Cache
- Raw-Zugriffsschutz
- DMA-Integration
- Medienwechsel und Hot-Unplug
- Writeback-Fehler
- Speicherdruck
- parallele Invalidierung
- Gerätegenerationen
- Dateisystem-Unmount

---

## 28. Verbindliche Invarianten

1. Der Buffer Cache ist kein zweiter allgemeiner File Cache.
2. Reguläre Dateiinhalte besitzen ihre kanonische Cache-Kopie im File Cache.
3. Buffer werden über Gerät, Generation, Offset und Länge identifiziert.
4. Dirty Buffers werden nicht stillschweigend verworfen.
5. Dateisysteme bestimmen transaktionskritische Writeback-Reihenfolgen.
6. Raw-Schreibzugriffe auf gemountete Geräte sind standardmäßig verboten.
7. Cache-Treffer umgehen keine Capability-Prüfung.
8. Medienwechsel invalidiert alle Buffer der vorherigen Gerätegeneration.
9. DMA wird ausschließlich über das DMA Framework ausgeführt.
10. Buffer-Inhalte werden nicht standardmäßig protokolliert.

---

## 29. Zusammenfassung

NovaOS verwendet einen zentral koordinierten Buffer Cache für Dateisystemmetadaten und blockorientierte I/O-Daten.

Reguläre Dateiinhalte verbleiben im File Cache, wodurch doppelte und möglicherweise inkonsistente Datenkopien vermieden werden. Der Buffer Cache ergänzt ihn um transaktionsgebundene Metadatenpuffer, Raw-Block-Zugriffe und koordinierte Blockgeräte-I/O.

Gerätegenerationen, Capability-Prüfungen und strenge Regeln für Raw-Zugriffe schützen vor veralteten oder unautorisierten Blockoperationen.