# ADR-VFS-0006 – Journaling Interface

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-VFS-0006 |
| Titel | Journaling Interface |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-31 |
| Bereich | VFS, Dateisystemtransaktionen und Konsistenz |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-VFS-0001, ADR-VFS-0004, ADR-VFS-0005, ADR-6015, ADR-SEC-0008 |
| Nachfolgende Spezifikation | NPSPEC-VFS-JOURNALING-INTERFACE |

---

## 1. Kontext

Dateisysteme müssen nach Abstürzen, Stromausfällen und Gerätefehlern einen konsistenten Zustand wiederherstellen können.

Journaling-Dateisysteme protokollieren zusammengehörige Änderungen, bevor sie endgültig in ihre Hauptstrukturen übernommen werden. NovaOS benötigt dafür eine gemeinsame VFS-Schnittstelle.

Die Schnittstelle soll unter anderem von folgenden Komponenten verwendet werden:

- NovaFS
- klassischen Journaling-Dateisystemen
- Dateisystemtreibern
- Buffer Cache
- File Cache
- Mount Manager
- Recovery-System
- lokaler Diagnose und Auditierung

---

## 2. Problemstellung

Jedes Dateisystem besitzt eine eigene On-Disk-Struktur und Transaktionslogik. Das VFS darf diese nicht vereinheitlichen, muss aber gemeinsame Abläufe koordinieren können:

- Transaktion beginnen
- geänderte Buffer und Metadaten zuordnen
- Abhängigkeiten festlegen
- Commit anfordern
- Persistenz bestätigen
- Fehler weitergeben
- Recovery-Status abfragen
- Unmount und Shutdown koordinieren

Das VFS darf einen erfolgreichen Commit erst melden, wenn das Dateisystem seine zugesicherte Dauerhaftigkeit tatsächlich erreicht hat.

---

## 3. Entscheidung

NovaOS definiert ein versioniertes Journaling Interface zwischen VFS, Cache-Schichten und Dateisystemtreibern.

Das Interface standardisiert Lebenszyklus und Status einer Transaktion. Es schreibt kein bestimmtes On-Disk-Journalformat vor.

Dateisysteme ohne klassisches Journal, beispielsweise Copy-on-Write-Dateisysteme, dürfen dieselbe Transaktionsschnittstelle mit einem alternativen atomaren Commit-Backend implementieren.

---

## 4. Architektur

```text
VFS-Operation
     |
     v
VFS-Transaktion
     |
     +--> File Cache
     +--> Buffer Cache
     +--> Dateisystemtreiber
     |
     v
Journal oder CoW-Commit
     |
     v
Speichergerät
```

Das Dateisystem bleibt verantwortlich für:

- Journalformat
- Metadatenreihenfolge
- Recovery-Algorithmus
- Prüfsummen
- Superblock-Aktualisierung
- endgültige Commit-Garantie

---

## 5. Transaktionsobjekt

Jede laufende Dateisystemtransaktion wird durch ein internes capability-geschütztes Objekt repräsentiert.

```c
typedef struct journal_transaction_info {
    uint64_t transaction_id;
    uint64_t filesystem_instance_id;
    uint32_t state;
    uint32_t flags;
    uint64_t dirty_bytes;
    uint64_t generation;
} journal_transaction_info_t;
```

Ein Transaktionsobjekt enthält mindestens:

- Transaktions-ID
- Dateisysteminstanz
- Zustand
- geänderte Objekte und Buffer
- Abhängigkeiten
- Fehlerstatus
- Commit-Generation
- Referenzanzahl

---

## 6. Transaktionszustände

| Zustand | Bedeutung |
|---|---|
| `OPEN` | Änderungen dürfen hinzugefügt werden |
| `PREPARING` | Änderungen und Abhängigkeiten werden abgeschlossen |
| `COMMITTING` | Transaktion wird dauerhaft geschrieben |
| `COMMITTED` | Commit wurde erfolgreich bestätigt |
| `ABORTING` | Transaktion wird abgebrochen |
| `ABORTED` | Änderungen wurden verworfen oder zurückgerollt |
| `FAILED` | Commit oder Recovery ist fehlgeschlagen |

Zustandsübergänge erfolgen atomar und grundsätzlich nur vorwärts.

---

## 7. Transaktion beginnen

```c
status_t journal_transaction_begin(
    cap_handle_t filesystem_handle,
    uint32_t flags,
    cap_handle_t *transaction_handle
);
```

Mögliche Flags:

```c
#define JOURNAL_TX_SYNC        (1u << 0)
#define JOURNAL_TX_METADATA    (1u << 1)
#define JOURNAL_TX_DATA        (1u << 2)
#define JOURNAL_TX_CRITICAL    (1u << 3)
#define JOURNAL_TX_NO_MERGE    (1u << 4)
```

Das Dateisystem darf kompatible kleine Transaktionen intern zusammenfassen, sofern deren sichtbare Semantik erhalten bleibt.

---

## 8. Änderungen registrieren

Geänderte Cache-Objekte werden einer Transaktion zugeordnet:

```c
status_t journal_attach_buffer(
    cap_handle_t transaction_handle,
    cap_handle_t buffer_handle,
    uint32_t flags
);

status_t journal_attach_file_range(
    cap_handle_t transaction_handle,
    cap_handle_t file_handle,
    uint64_t offset,
    uint64_t length,
    uint32_t flags
);
```

Ein Dirty Buffer darf nicht gleichzeitig widersprüchlichen Transaktionen zugeordnet sein.

Das Dateisystem entscheidet, ob Dateidaten, Metadaten oder beide journalisiert werden.

---

## 9. Journaling-Modi

Das Interface unterstützt mindestens folgende logische Modi:

| Modus | Verhalten |
|---|---|
| Metadata | Nur Metadaten werden journalisiert |
| Ordered | Dateidaten werden vor zugehörigen Metadaten persistiert |
| Full Data | Daten und Metadaten werden journalisiert |
| Copy-on-Write | Neue Strukturen werden atomar veröffentlicht |
| External Journal | Journal liegt auf einer getrennten Quelle |

Der aktive Modus muss über die Dateisystemfähigkeiten abfragbar sein.

Das VFS darf keine stärkere Garantie an Anwendungen melden, als der aktive Modus bereitstellt.

---

## 10. Abhängigkeiten und Reihenfolge

Eine Transaktion kann Schreibabhängigkeiten enthalten.

Beispiele:

```text
Dateidaten vor Dateigröße
neuer Verzeichnisblock vor Verzeichniseintrag
Journalinhalt vor Commit-Record
neuer Baumknoten vor Root-Umschaltung
```

Das Dateisystem übergibt dem Buffer Cache die zulässige Writeback-Reihenfolge.

Blockgeräte-Barrieren und Flush-Befehle werden verwendet, wenn sie zur Dauerhaftigkeitsgarantie erforderlich sind.

---

## 11. Commit

```c
status_t journal_transaction_commit(
    cap_handle_t transaction_handle,
    uint64_t timeout_ns
);
```

Ein synchroner Commit ist erst erfolgreich, wenn:

1. alle erforderlichen Daten geschrieben wurden,
2. die vorgeschriebenen Barrieren abgeschlossen sind,
3. der Commit-Record oder atomare Root-Wechsel persistent ist,
4. das Speichergerät den Abschluss entsprechend seiner Fähigkeiten bestätigt hat.

Asynchrone Commits liefern ein wartbares Completion-Objekt.

---

## 12. Abbruch

Eine noch nicht dauerhaft veröffentlichte Transaktion kann abgebrochen werden:

```c
status_t journal_transaction_abort(
    cap_handle_t transaction_handle,
    status_t reason
);
```

Ein Abbruch:

- verhindert weitere Änderungen,
- löst temporäre Ressourcen,
- invalidiert nicht veröffentlichte Cachezustände,
- informiert wartende Operationen,
- hinterlässt das Dateisystem in einem definierten Zustand.

Eine bereits dauerhaft bestätigte Transaktion kann nicht durch `abort` rückgängig gemacht werden. Dafür ist eine neue Gegen- oder Recovery-Transaktion erforderlich.

---

## 13. Verschachtelte Transaktionen

Die native VFS-Schnittstelle erlaubt keine unabhängigen, frei verschachtelten Dateisystemtransaktionen.

Eine Operation innerhalb einer bestehenden Transaktion kann:

- der aktuellen Transaktion beitreten,
- eine untergeordnete logische Operation registrieren,
- eine separate Transaktion erst nach Abschluss beginnen.

Dadurch werden widersprüchliche Commit- und Rollback-Erwartungen vermieden.

Dateisysteminterne Verschachtelung bleibt möglich, darf aber nicht als unabhängige VFS-Garantie sichtbar werden.

---

## 14. Writeback-Integration

Der Buffer Cache darf transaktionsgebundene Dirty Buffers nicht unabhängig in einer gefährlichen Reihenfolge schreiben.

Der File Cache muss Dirty File Pages entsprechend dem Journaling-Modus behandeln:

- bei `Metadata` nach den normalen Writeback-Regeln,
- bei `Ordered` vor dem Metadaten-Commit,
- bei `Full Data` als Teil des Journals,
- bei `Copy-on-Write` nach den CoW-Regeln des Dateisystems.

Speicherdruck darf Transaktionsgarantien nicht umgehen.

---

## 15. Fehlerbehandlung

Mögliche Commit-Fehler sind:

| Fehler | Bedeutung |
|---|---|
| `STATUS_JOURNAL_FULL` | Journal besitzt keinen freien Platz |
| `STATUS_IO_ERROR` | Speicheroperation fehlgeschlagen |
| `STATUS_TIMEOUT` | Commit wurde nicht rechtzeitig abgeschlossen |
| `STATUS_DEVICE_REMOVED` | Speichermedium wurde entfernt |
| `STATUS_FILESYSTEM_CORRUPT` | Inkonsistente Strukturen erkannt |
| `STATUS_TRANSACTION_ABORTED` | Transaktion wurde abgebrochen |
| `STATUS_READ_ONLY` | Dateisystem wurde schreibgeschützt |
| `STATUS_PERSISTENCE_UNCERTAIN` | Dauerhaftigkeit kann nicht bestätigt werden |

Bei `STATUS_PERSISTENCE_UNCERTAIN` darf NovaOS den Vorgang nicht als eindeutig fehlgeschlagen oder erfolgreich darstellen.

---

## 16. Fehlerstrategie des Dateisystems

Bei einem schwerwiegenden Journalfehler kann das Dateisystem:

- die aktuelle Transaktion abbrechen,
- weitere Schreibzugriffe blockieren,
- in einen schreibgeschützten Zustand wechseln,
- einen kontrollierten Remount anfordern,
- Recovery beim nächsten Start verlangen,
- das Volume als fehlerhaft markieren.

Ein beschädigtes Journal darf nicht stillschweigend weiterverwendet werden.

Bereits erkannte Writeback-Fehler bleiben sichtbar, bis sie ausdrücklich behandelt wurden.

---

## 17. Recovery

Beim Mounten eines nicht sauber geschlossenen Dateisystems meldet der Treiber einen Recovery-Bedarf.

Der Mount Manager kann daraufhin:

1. Dateisystem schreibgeschützt öffnen,
2. Journal validieren,
3. vollständige Transaktionen wiederholen,
4. unvollständige Transaktionen verwerfen oder zurückrollen,
5. Metadaten und Prüfsummen prüfen,
6. einen neuen sauberen Zustand schreiben,
7. normalen Mount freigeben.

Recovery erfolgt vor der Veröffentlichung des beschreibbaren Mounts.

---

## 18. Recovery-Zustände

| Zustand | Bedeutung |
|---|---|
| `CLEAN` | Keine Recovery notwendig |
| `REPLAY_REQUIRED` | Journal muss abgespielt werden |
| `REPLAYING` | Recovery läuft |
| `RECOVERED` | Recovery erfolgreich |
| `RECOVERY_FAILED` | Wiederherstellung fehlgeschlagen |
| `MANUAL_REPAIR_REQUIRED` | Automatische Recovery nicht ausreichend |

Bei fehlgeschlagener Recovery wird das Dateisystem standardmäßig nur lesbar oder gar nicht eingebunden.

---

## 19. Unmount und Shutdown

Vor einem regulären Unmount oder Shutdown müssen:

- offene Transaktionen abgeschlossen oder abgebrochen,
- Dirty File Pages verarbeitet,
- Dirty Buffers geschrieben,
- erforderliche Barrieren ausgeführt,
- ein sauberer Dateisystemzustand bestätigt werden.

Ein Emergency Shutdown darf diese Schritte verkürzen, muss das Dateisystem dann aber als möglicherweise Recovery-pflichtig markieren.

Ein normaler Shutdown darf keinen sauberen Zustand melden, wenn der Commit nicht bestätigt wurde.

---

## 20. Capability-Rechte

| Recht | Bedeutung |
|---|---|
| `JOURNAL_RIGHT_BEGIN` | Transaktion beginnen |
| `JOURNAL_RIGHT_ATTACH` | Änderungen zuordnen |
| `JOURNAL_RIGHT_COMMIT` | Commit anfordern |
| `JOURNAL_RIGHT_ABORT` | Transaktion abbrechen |
| `JOURNAL_RIGHT_INSPECT` | Status und Metadaten lesen |
| `JOURNAL_RIGHT_RECOVER` | Recovery ausführen |
| `JOURNAL_RIGHT_ADMIN` | Journal verwalten |

Gewöhnliche Anwendungen erhalten keine direkten Journal-Capabilities. Sie verwenden VFS-Operationen wie `fsync`, die intern Transaktionen auslösen.

---

## 21. Dateisystemtreiber-Schnittstelle

Ein journalfähiger Dateisystemtreiber implementiert mindestens:

```c
typedef struct vfs_journal_operations {
    uint32_t structure_size;
    uint32_t version;

    status_t (*begin)(vfs_instance_t *, journal_tx_t *);
    status_t (*prepare)(journal_tx_t *);
    status_t (*commit)(journal_tx_t *);
    status_t (*abort)(journal_tx_t *);
    status_t (*recover)(vfs_instance_t *);
    status_t (*query_state)(vfs_instance_t *, journal_state_t *);
} vfs_journal_operations_t;
```

Optionale Funktionen werden über Versions- und Featureinformationen erkannt.

---

## 22. Sicherheit und Integrität

Für das Journaling Interface gelten folgende Regeln:

- Journal- und Recovery-Daten gelten als nicht vertrauenswürdig.
- Größen, Offsets, Sequenznummern und Prüfsummen werden validiert.
- Alte Journalbereiche dürfen keine bereits neueren Zustände überschreiben.
- Transaktions-IDs müssen vor Wiederverwendung sicher getrennt werden.
- Recovery darf keine Daten außerhalb des zugehörigen Dateisystems verändern.
- Externe Journale benötigen eine eindeutige Bindung an ihr Dateisystem.
- Journalinhalte werden nicht standardmäßig protokolliert.

Kryptografische Integrität kann zusätzlich durch das jeweilige Dateisystem bereitgestellt werden.

---

## 23. Audit und Diagnose

Diagnoseinformationen können umfassen:

- aktuelle und letzte Transaktions-ID
- Journaling-Modus
- Dirty-Datenmenge
- Commit-Latenzen
- Journalbelegung
- Writeback-Fehler
- Recovery-Status
- letzter sauberer Commit

Auditiert werden insbesondere:

- fehlgeschlagene kritische Commits
- Recovery-Vorgänge
- beschädigte Journalstrukturen
- Wechsel in den Read-only-Modus
- erzwungene Transaktionsabbrüche
- unklarer Persistenzstatus

Datei- und Journalinhalte werden dabei nicht aufgezeichnet.

---

## 24. Alternative Ansätze

### 24.1 Einheitliches On-Disk-Journal für alle Dateisysteme

**Vorteile:**

- identische Recovery-Logik
- zentrale Implementierung

**Nachteile:**

- inkompatibel mit unterschiedlichen Dateisystemstrukturen
- verhindert spezielle CoW- und Netzwerkmodelle

**Entscheidung:** Abgelehnt.

### 24.2 Journaling vollständig vor dem VFS verbergen

**Vorteile:**

- einfacheres VFS

**Nachteile:**

- keine koordinierte Flush- und Unmount-Semantik
- Cache-Schichten kennen keine Transaktionsgrenzen

**Entscheidung:** Abgelehnt.

### 24.3 Journaling für jedes Dateisystem erzwingen

**Vorteile:**

- einheitliches Absturzmodell

**Nachteile:**

- ungeeignet für schreibgeschützte, temporäre oder CoW-Dateisysteme

**Entscheidung:** Abgelehnt. Das Interface unterstützt alternative atomare Commit-Modelle.

---

## 25. Konsequenzen

### Positive Konsequenzen

- einheitlicher Transaktionslebenszyklus
- koordinierte File- und Buffer-Cache-Operationen
- ehrliche Commit- und Persistenzsemantik
- standardisierte Recovery-Integration
- Unterstützung klassischer Journale und Copy-on-Write
- kontrolliertes Verhalten bei Unmount und Shutdown

### Negative Konsequenzen

- zusätzliche Schnittstelle zwischen VFS und Dateisystemtreibern
- On-Disk-Recovery bleibt dateisystemspezifisch
- Transaktionsabhängigkeiten erhöhen die Writeback-Komplexität
- Persistenzgarantien hängen weiterhin von Speicherhardware ab

### Risiken

- falsche Writeback-Reihenfolge
- Journalüberlauf
- unbemerkte Geräte-Cache-Probleme
- wiederholtes Abspielen alter Transaktionen
- Deadlocks zwischen Cache und Transaktionssperren
- fälschlich bestätigte Persistenz

---

## 26. Implementierungsrichtlinien

- Das Interface standardisiert den Ablauf, nicht das On-Disk-Format.
- Transaktionszustände dürfen nur vorwärts wechseln.
- Dirty Buffers werden eindeutig einer Transaktion zugeordnet.
- Der Buffer Cache beachtet dateisystemspezifische Schreibabhängigkeiten.
- Ein Commit wird erst nach bestätigter Dauerhaftigkeit erfolgreich gemeldet.
- Ein bereits bestätigter Commit kann nicht durch `abort` aufgehoben werden.
- Recovery findet vor einem beschreibbaren Mount statt.
- Fehler und unklare Persistenzzustände müssen an Anwendungen weitergegeben werden.
- Journaling darf durch Speicherdruck nicht umgangen werden.

---

## 27. Testanforderungen

Die Implementierung benötigt mindestens Tests für:

- Begin, Prepare, Commit und Abort
- synchrone und asynchrone Commits
- Metadata-, Ordered-, Full-Data- und CoW-Modi
- Writeback-Reihenfolgen
- Journalüberlauf
- I/O-Fehler während jeder Commit-Phase
- Stromausfall-Simulation
- Recovery und wiederholtes Replay
- beschädigte Journalstrukturen
- Unmount mit offenen Transaktionen
- Geräteentfernung
- unklaren Persistenzstatus
- parallele Transaktionen
- Cache- und Transaktions-Deadlocks

---

## 28. Verbindliche Invarianten

1. Das VFS schreibt kein bestimmtes Journalformat vor.
2. Jede Transaktion besitzt einen eindeutig definierten Zustand.
3. Transaktionszustände wechseln grundsätzlich nur vorwärts.
4. Dirty Buffer und Dateibereiche werden eindeutig einer Transaktion zugeordnet.
5. Writeback-Reihenfolgen des Dateisystems müssen eingehalten werden.
6. Ein Commit gilt erst nach bestätigter Dauerhaftigkeit als erfolgreich.
7. Ein bestätigter Commit kann nicht durch einen Abbruch rückgängig gemacht werden.
8. Recovery erfolgt vor Veröffentlichung eines beschreibbaren Mounts.
9. Unklare Persistenz darf nicht als Erfolg dargestellt werden.
10. Journalinhalte werden nicht standardmäßig protokolliert.

---

## 29. Zusammenfassung

NovaOS definiert ein versioniertes Journaling Interface zwischen VFS, Cache-Schichten und Dateisystemtreibern.

Die Schnittstelle vereinheitlicht Transaktionsbeginn, Änderungszuordnung, Commit, Abbruch, Fehlerweitergabe und Recovery, ohne ein bestimmtes On-Disk-Journal vorzuschreiben. Damit können klassische Journaling-Dateisysteme ebenso wie Copy-on-Write-Dateisysteme integriert werden.

Der wichtigste Grundsatz ist eine ehrliche Persistenzsemantik: Ein Commit wird nur dann als erfolgreich gemeldet, wenn das Dateisystem und das Speichergerät die zugesicherte Dauerhaftigkeit tatsächlich bestätigt haben.