# NPSPEC-STORAGE-CHECKSUM-0001 – Nova Storage Checksums

## Status

Entwurf

## Kategorie

Storage / Integrity / Checksum / Error Detection

## Zweck

NovaOS definiert Checksums zur Erkennung unbeabsichtigter Veränderungen und beschädigter gespeicherter Daten.

```text
Data
 ↓
Checksum
 ↓
Store
 ↓
Read
 ↓
Verify
```

Checksums bilden eine grundlegende Integritätsschicht für Payloads, Metadaten und Storage-Strukturen.

## Grundprinzip

```text
Checksum ≠ Encryption
Checksum ≠ Authentication
Checksum ≠ Backup
```

Eine gültige Checksum zeigt, dass Daten mit dem erwarteten Zustand übereinstimmen. Sie garantiert nicht automatisch Herkunft, Berechtigung oder Schutz vor gezielter Manipulation.

## Checksum-Modell

Ein Checksum-Eintrag enthält mindestens:

```text
Algorithm
Checksum Value
Protected Range
Scope
```

Optional:

```text
ObjectID
VersionID
ContentID
BlockID
Generation
```

## Prüfebenen

NovaOS muss Checksums auf unterschiedlichen Ebenen unterstützen können:

```text
Block
Extent
Payload
Metadata
Object Version
Filesystem Structure
```

Das konkrete Dateisystem darf bestimmen, welche Ebenen nativ verwendet werden.

## Schreiben

Bei einer Änderung wird die zugehörige Checksum aktualisiert.

```text
Modified Data
     ↓
Calculate Checksum
     ↓
Store Data + Checksum
     ↓
Commit
```

Daten und zugehörige Checksum sollen konsistent aktualisiert werden.

## Lesen

Beim Lesen darf NovaOS die Daten gegen ihre gespeicherte Checksum prüfen.

```text
Read Data
   ↓
Calculate
   ↓
Compare
   ↓
Valid / Corrupt
```

Eine Abweichung muss als Integritätsfehler behandelt werden.

## Verifikation

Die Prüfstrategie darf abhängig von Datenklasse und Storage-Policy variieren:

```text
On Read
Periodic Scrub
On Demand
Before Recovery
Before Migration
```

Kritische Systemdaten sollen strengere Prüfregeln besitzen können.

## Fehlerbehandlung

Bei einer fehlerhaften Checksum darf NovaOS nicht stillschweigend von gültigen Daten ausgehen.

Mögliche Reaktionen:

```text
Report
Retry
Read Alternate Copy
Restore Known-Good Version
Quarantine
Mark Corrupt
```

Eine Reparatur darf nur erfolgen, wenn eine verifizierbare alternative Datenquelle vorhanden ist.

## Redundanz

Checksums können mit redundanter Speicherung kombiniert werden.

```text
Corrupt Copy
     ↓
Checksum Failure
     ↓
Verified Alternate Copy
     ↓
Repair
```

Die Checksum selbst stellt keine redundante Kopie der Daten dar.

## Content Addressing

Bei Content-Addressed Storage darf die `ContentID` zusätzlich zur Integritätsprüfung verwendet werden.

```text
Payload
   ↓
Hash
   ↓
ContentID
```

Content Addressing und Storage Checksums bleiben jedoch logisch getrennte Mechanismen.

## Metadaten

Nicht nur Payloads, sondern auch kritische Metadaten sollen geschützt werden können.

Dazu gehören insbesondere:

```text
Object Metadata
Version References
Directory Structures
Allocation Metadata
Snapshot Metadata
```

## Algorithmus

Checksum-Algorithmen müssen versionierbar und austauschbar sein.

NovaOS darf je nach Zweck unterschiedliche Algorithmen verwenden.

```text
Fast Error Detection
Strong Integrity Verification
Hardware Accelerated Checksum
```

Ein Algorithmus darf nicht dauerhaft als fester Bestandteil des Storage-Formats vorausgesetzt werden.

## Scrubbing

NovaOS soll periodische Integritätsprüfungen unterstützen können.

```text
Storage
   ↓
Scrub
   ↓
Verify Checksums
   ↓
Detect Latent Corruption
```

Scrubbing soll ressourcenbewusst und unter Berücksichtigung von I/O-Last, Energie und Prioritäten erfolgen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Checksum Algorithm
Integrity State
Last Verification
Corrupt Range
Repair State
Verification Errors
```

## Normative Anforderungen

1. NovaOS MUSS Checksums für persistente Daten unterstützen können.
2. Payload und kritische Storage-Metadaten MÜSSEN prüfbar sein können.
3. Daten und zugehörige Checksums SOLLEN konsistent aktualisiert werden.
4. Eine Checksum-Abweichung MUSS als Integritätsfehler behandelt werden.
5. Beschädigte Daten DÜRFEN NICHT stillschweigend als gültig behandelt werden.
6. Automatische Reparatur MUSS eine verifizierbare alternative Datenquelle voraussetzen.
7. Checksum-Algorithmen MÜSSEN versionierbar und austauschbar sein.
8. NovaOS SOLL On-Read- und periodische Integritätsprüfung unterstützen können.
9. Checksums DÜRFEN NICHT als Ersatz für Backup, Authentifizierung oder Verschlüsselung behandelt werden.
10. ContentID und Storage Checksum MÜSSEN logisch getrennt bleiben.
11. Integritätsprüfung SOLL ressourcenbewusst ausführbar sein.
12. Integritäts- und Reparaturzustände SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-STORAGE-0010`

## Ergebnis

```text
Stored Data
    ↓
Checksum Verification
    ↓
Valid ─────────→ Use
    ↓
Corrupt
    ↓
Detect → Recover / Quarantine
```

NovaOS erhält damit eine einheitliche Integritätsschicht, die beschädigte Storage-Daten zuverlässig erkennt und mit Versionierung, Snapshots und Recovery kombiniert werden kann.