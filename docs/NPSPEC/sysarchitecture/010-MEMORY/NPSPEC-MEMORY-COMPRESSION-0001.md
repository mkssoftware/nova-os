# NPSPEC-MEMORY-COMPRESSION-0001 – Nova Memory Compression

## Status

Entwurf

## Kategorie

Memory / Compression / Reclamation / Memory Pressure

## Zweck

NovaOS definiert Memory Compression als optionalen Mechanismus, bei dem selten oder weniger aktiv verwendete Speicherseiten komprimiert im RAM gehalten werden.

```text
Normal Page
    ↓
Compress
    ↓
Compressed Memory
    ↓
Physical Frames released
```

Bei erneutem Zugriff wird der Inhalt dekomprimiert und wieder als normale Page bereitgestellt.

Memory Compression soll Memory Pressure reduzieren und teureres Swap-I/O vermeiden oder verzögern.

## Grundprinzip

Komprimierter Speicher tauscht CPU-Arbeit gegen geringeren RAM-Verbrauch.

```text
RAM Usage
   ↓
Compression
   ↓
Less RAM Usage
   +
CPU Cost
```

Dabei gilt:

```text
Compressed Memory ≠ Swap
Compressed Memory ≠ Free Memory
Compression ≠ Reclamation
```

Komprimierte Daten benötigen weiterhin physischen Speicher.

## Compression Pool

Komprimierte Pages werden in einem kontrollierten Compression Pool gespeichert.

```text
Physical Page
     ↓
Compression
     ↓
Compression Pool
```

Der Pool besitzt mindestens:

```text
Capacity
Used
Available
CompressedObjects
State
```

Der Pool muss begrenzt sein und darf nicht unkontrolliert wachsen.

## Compression Entry

Eine komprimierte Page wird logisch durch einen Eintrag repräsentiert.

```text
CompressionEntry
├── ObjectID
├── CompressedSize
├── OriginalSize
├── Algorithm
└── State
```

Optional:

```text
Checksum
NUMANode
AccessStatistics
BackingReference
```

Die konkrete Speicherstruktur bleibt Implementierungsdetail.

## Compression

Der grundlegende Ablauf lautet:

```text
Candidate Page
     ↓
Validate
     ↓
Compress
     ↓
Compression worthwhile?
   ┌──────┴──────┐
  Yes            No
   ↓              ↓
Store           Keep Page
   ↓
Release Original Frame
```

Der ursprüngliche Frame darf erst freigegeben werden, wenn eine gültige komprimierte Repräsentation existiert.

## Dekompression

Bei erneutem Zugriff:

```text
Page Fault
    ↓
Compressed Entry
    ↓
Allocate Frame
    ↓
Decompress
    ↓
Validate
    ↓
Install Mapping
    ↓
Resume Task
```

Die Dekompression wird in den normalen Page-Fault- und Demand-Paging-Pfad integriert.

## Kandidatenauswahl

Nicht jede Page eignet sich für Compression.

Die Auswahl darf berücksichtigen:

```text
Access Frequency
Last Access
Compressibility
Compression Cost
Decompression Cost
Page Priority
Memory Pressure
Resource Budget
Latency Requirements
```

Aktiv verwendete Pages sollen nicht unnötig komprimiert werden.

## Kompressionsnutzen

Vor oder während der Kompression darf NovaOS bewerten:

```text
Memory Saved
──────────────
CPU + Latency Cost
```

Ist die erzielte Größenreduktion zu gering, soll die Page unkomprimiert bleiben.

```text
Poor Compression Ratio
        ↓
Abort Compression
```

## Algorithmen

NovaOS darf mehrere Kompressionsalgorithmen unterstützen.

Die Auswahl kann abhängig sein von:

```text
Compression Ratio
Compression Speed
Decompression Speed
CPU Load
Energy
Hardware Support
Memory Pressure
```

Die Algorithmen sollen über eine austauschbare interne Schnittstelle angebunden werden.

## Memory Pressure

Compression ist Teil der Memory-Pressure-Strategie.

```text
Memory Pressure
      ↓
Reclaim Evaluation
      ↓
Compressible Page
      ↓
Compression
      ↓
Physical Memory Gain
```

Abhängig von Kosten und Systemzustand kann NovaOS zwischen:

```text
Keep
Discard
Compress
Swap
Reconstruct
```

entscheiden.

## Verhältnis zu Swap

Memory Compression soll nach Möglichkeit langsames Storage-I/O vermeiden.

Konzeptionell kann gelten:

```text
RAM
 ↓
Compressed RAM
 ↓
Swap
```

Diese Reihenfolge ist jedoch keine universelle Vorgabe.

Resource Policy, Latenz, Energie, Workload und Hardware können eine andere Entscheidung rechtfertigen.

## Anonymous Memory

Anonymous Pages eignen sich besonders für Compression, wenn sie:

```text
Inactive
Compressible
Not Pinned
Not Realtime-Protected
```

sind.

Dadurch kann physischer Speicher freigegeben werden, ohne die Daten auf Storage auszulagern.

## File-backed Memory

Clean file-backed Pages sollen normalerweise verworfen werden, wenn sie günstig aus ihrem Backing Object rekonstruiert werden können.

```text
Clean File Page
      ↓
Discard
```

statt:

```text
Clean File Page
      ↓
Compress
```

Compression ist sinnvoll, wenn Rekonstruktion teurer als die komprimierte Speicherung ist.

## NUMA

Compression Pools dürfen NUMA-lokal organisiert werden.

```text
NUMA Node
   ↓
Local Compression Pool
```

Dekompression soll nach Möglichkeit geeignete lokale Frames verwenden.

Remote Compression darf nur erfolgen, wenn ihre Kosten gegenüber anderen Optionen vertretbar sind.

## Realtime

Hard-Realtime-Speicher darf nicht unkontrolliert komprimiert werden.

```text
Compressed Page
      ↓
Access
      ↓
Decompression Latency
```

Da diese Latenz nicht grundsätzlich deterministisch ist, müssen Realtime-geschützte Pages resident und unkomprimiert gehalten werden können.

## Thrashing

NovaOS soll Compression Thrashing erkennen können.

```text
Compress
   ↓
Immediate Decompress
   ↓
Compress
   ↓
...
```

Bei wiederholtem Thrashing sollen Kandidatenauswahl oder Policies angepasst werden.

## Parallelisierung

Kompression und Dekompression dürfen parallel ausgeführt werden.

```text
CPU 0 → Compression
CPU 1 → Compression
CPU 2 → Decompression
```

Die Parallelisierung darf Memory Pressure jedoch nicht durch übermäßige temporäre Buffer verschärfen.

## Sicherheit

Komprimierte Speicherinhalte behalten den Sicherheitskontext ihrer ursprünglichen Daten.

Compression Pools dürfen keine Möglichkeit bieten, Isolation oder Zugriffsrechte zu umgehen.

Freigegebene Compression Entries müssen entsprechend der Security Policy behandelt werden.

## Fehlerbehandlung

Mögliche Fehler umfassen:

```text
Compression Failure
Decompression Failure
Corrupted Entry
Allocation Failure
Invalid Metadata
```

Der ursprüngliche Frame darf bei fehlgeschlagener Kompression nicht verloren gehen.

Ein Dekompressionsfehler muss kontrolliert als Speicher- oder Datenintegritätsfehler behandelt werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Compression Pool Size
Compressed Pages
Original Bytes
Compressed Bytes
Compression Ratio
Compression Operations
Decompression Operations
Average Latency
Rejected Candidates
Compression Thrashing
Failures
```

## Normative Anforderungen

1. NovaOS MUSS auch ohne Memory Compression vollständig funktionsfähig sein.
2. Memory Compression MUSS als optionale Reclamation-Strategie behandelt werden.
3. Nur explizit geeignete Pages DÜRFEN komprimiert werden.
4. Der ursprüngliche Frame DARF erst nach erfolgreicher Erstellung der komprimierten Repräsentation freigegeben werden.
5. Compression Pools MÜSSEN begrenzbar sein.
6. Kompression SOLL nur eingesetzt werden, wenn ein sinnvoller Speichergewinn erwartet wird.
7. Dekompression MUSS in den Page-Fault- und Demand-Paging-Pfad integrierbar sein.
8. Realtime-geschützter und gepinnter Speicher DARF NICHT unkontrolliert komprimiert werden.
9. NUMA-Lokalität SOLL bei Compression und Dekompression berücksichtigt werden.
10. NovaOS SOLL Compression Thrashing erkennen und darauf reagieren können.
11. Kompressionsfehler DÜRFEN NICHT zum Verlust der einzigen gültigen Speicherkopie führen.
12. Compression Ratio, Speichergewinn, Latenzen und Fehler SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-RECLAIM-0001`
- `NPSPEC-MEMORY-OOM-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-DEMANDPAGING-0001`
- `NPSPEC-MEMORY-SWAP-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `ADR-MEMORY-0019`

## Ergebnis

```text
Memory Pressure
      ↓
Candidate Selection
      ↓
Compression
      ↓
Compressed Memory Pool
      ↓
Release Physical Frames
      ↓
Later Access
      ↓
Decompress
      ↓
Restore Mapping
```

NovaOS erhält damit eine optionale Memory-Compression-Schicht, die RAM durch CPU-Arbeit effizienter nutzen kann und kontrolliert mit Reclamation, Demand Paging, NUMA, Swap, Realtime und OOM zusammenarbeitet.