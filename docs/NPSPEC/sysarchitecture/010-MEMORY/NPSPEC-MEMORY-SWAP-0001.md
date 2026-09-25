# NPSPEC-MEMORY-SWAP-0001 – Nova Swap Management

## Status

Angenommen

## Kategorie

Memory / Swap / Virtual Memory / Memory Pressure

## Zweck

NovaOS definiert Swap als optionalen Mechanismus, bei dem selten benötigte Speicherinhalte aus dem physischen RAM in einen sekundären Speicher ausgelagert werden können.

```text
Physical Memory
      ↓
Memory Pressure
      ↓
Select Page
      ↓
Swap Out
      ↓
Swap Storage
```

Bei erneutem Zugriff wird die Seite wieder in den physischen Speicher geladen.

```text
Access
  ↓
Page Fault
  ↓
Swap In
  ↓
Physical Frame
```

Swap erweitert damit die Möglichkeiten des Memory-Reclamation-Systems, ist jedoch keine Voraussetzung für den Betrieb von NovaOS.

## Grundprinzip

Swap ist ein Backing-Mechanismus für auslagerbare Speicherinhalte.

```text
Virtual Page
    ↓
RAM or Swap
```

Dabei gilt:

```text
Swap ≠ Physical Memory
Swap ≠ Reclamation
Swap ≠ Persistent Storage
Swap ≠ Memory Guarantee
```

NovaOS muss vollständig ohne konfigurierten Swap funktionieren können.

## Swap Object

Swap-Speicher wird als verwaltete Ressource modelliert.

Ein Swap Provider beschreibt mindestens:

```text
SwapProviderID
Capacity
Used
Available
State
```

Optional:

```text
Priority
PerformanceClass
Encryption
Device
FailureDomain
```

Mehrere Swap Provider dürfen gleichzeitig verwendet werden.

## Swap Provider

Mögliche Provider sind beispielsweise:

```text
Dedicated Partition
Swap File
Compressed RAM
Persistent Memory
Other Authorized Storage
```

Die konkrete Implementierung darf unterschiedliche Provider kombinieren.

## Swap-Out

Unter Memory Pressure darf der Reclaim-Mechanismus geeignete Pages zur Auslagerung auswählen.

```text
Reclaim Candidate
      ↓
Validate Swappability
      ↓
Reserve Swap Slot
      ↓
Write Page
      ↓
Update Mapping State
      ↓
Release Physical Frame
```

Der physische Frame darf erst freigegeben werden, wenn der notwendige Speicherinhalt erfolgreich gesichert wurde.

## Swap-In

Beim Zugriff auf eine ausgelagerte Page:

```text
Page Fault
    ↓
Swap Entry detected
    ↓
Allocate Frame
    ↓
Read Swap Data
    ↓
Validate
    ↓
Install Mapping
    ↓
Resume Task
```

Der Fault wird als spezieller Demand-Paging-Fall behandelt.

## Swappability

Nicht jeder Speicher darf ausgelagert werden.

Typischerweise nicht auslagerbar sind:

```text
Kernel-critical Memory
Page Tables
Pinned Memory
Active DMA Buffers
Emergency Reserves
Hardware-owned Memory
Realtime-locked Memory
```

Die Swappability muss explizit bestimmbar sein.

## Auswahl

Swap-Out soll nicht allein anhand des Alters einer Page erfolgen.

Die Auswahl darf berücksichtigen:

```text
Recent Access
Access Frequency
Reconstruction Cost
Swap I/O Cost
NUMA Locality
Priority
Resource Budget
Latency Requirements
Memory Pressure
```

Ziel ist die Freigabe physischen Speichers mit möglichst geringer negativer Auswirkung.

## File-backed Pages

Unveränderte file-backed Pages müssen normalerweise nicht in Swap geschrieben werden.

```text
Clean File-backed Page
        ↓
Discard
        ↓
Reload from Backing Object
```

Veränderte private Pages können dagegen Swap-Backing benötigen.

```text
Modified Private Page
        ↓
Swap
```

Damit soll unnötiges Swap-I/O vermieden werden.

## Anonymous Memory

Anonymous Memory ist ein primärer Kandidat für Swap, sofern sie:

```text
Reclaimable
Not Pinned
Not Realtime-Protected
```

ist.

Die Zuordnung zwischen virtueller Page und Swap-Inhalt muss erhalten bleiben.

## Swap Slot

Ein ausgelagerter Speicherinhalt erhält eine eindeutige Referenz auf seinen Swap-Speicherort.

Konzeptionell:

```text
SwapEntry
├── ProviderID
├── Slot / Offset
└── State
```

Die konkrete Repräsentation darf architektur- und implementierungsabhängig sein.

## Verschlüsselung

Swap kann sensible Prozessdaten enthalten.

Persistenter Swap soll deshalb standardmäßig verschlüsselt werden können.

```text
Memory Page
    ↓
Encryption
    ↓
Swap Storage
```

Schlüssel müssen entsprechend der NovaOS-Sicherheitsarchitektur verwaltet werden.

Unberechtigter Zugriff auf den Swap Provider darf keine Klartext-Speicherinhalte offenlegen.

## Kompression

NovaOS darf Pages vor einer Auslagerung komprimieren.

```text
Page
 ↓
Compress
 ↓
Compressed Memory / Swap
```

Die Entscheidung muss Kosten berücksichtigen:

```text
CPU Cost
Memory Saved
I/O Saved
Latency
Energy
```

Kompression ist eine Optimierung und keine Voraussetzung.

## NUMA

Swap-Out soll NUMA-Lokalität berücksichtigen.

Ein lokaler NUMA-Engpass darf nicht automatisch unnötiges systemweites Swapping verursachen, wenn geeigneter physischer Speicher auf einem zulässigen anderen Node verfügbar ist.

```text
Local Pressure
    ↓
Local Reclaim
    ↓
Allowed Remote Memory
    ↓
Swap
```

Die tatsächliche Reihenfolge wird durch die geltende Memory Policy bestimmt.

## Realtime

Hard-Realtime-Speicher darf nicht unkontrolliert ausgelagert werden.

```text
Hard Realtime Memory
        ↓
Resident / Protected
```

Ein späterer Swap-In würde nichtdeterministische I/O-Latenz verursachen.

Realtime-Reservierungen müssen daher Swap-Schutz besitzen können.

## Thrashing

NovaOS muss erkennen können, wenn häufiges Swap-In und Swap-Out keinen nachhaltigen Speichergewinn erzeugt.

```text
Swap Out
   ↓
Immediate Swap In
   ↓
Swap Out
   ↓
Thrashing
```

Bei erkanntem Thrashing sollen Strategien angepasst werden, beispielsweise:

```text
Reduce Swap Activity
Change Victim Selection
Throttle Consumers
Reduce Optional Work
Trigger Stronger Reclamation
Escalate Memory Pressure
```

## OOM

Swap darf OOM verzögern, aber nicht dessen Semantik ersetzen.

```text
RAM exhausted
     ↓
Reclaim + Swap
     ↓
Still insufficient
     ↓
OOM
```

Ist kein Swap vorhanden oder dessen Nutzung nicht erlaubt, muss das OOM-System weiterhin korrekt funktionieren.

## Fehlerbehandlung

Swap-I/O kann fehlschlagen.

Mögliche Fehler:

```text
Provider Offline
I/O Error
Corrupted Data
No Swap Space
Decryption Failure
```

Ein fehlgeschlagener Swap-Out darf die einzige gültige Kopie einer Page nicht verlieren.

Ein nicht wiederherstellbarer Swap-In-Fehler muss als Daten- beziehungsweise Speicherfehler behandelt werden.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Swap Capacity
Swap Usage
Swap-In Pages
Swap-Out Pages
Swap I/O
Compression Ratio
Swap Fault Latency
Provider State
Thrashing Indicators
Swap Failures
```

## Normative Anforderungen

1. NovaOS MUSS auch ohne Swap vollständig funktionsfähig sein.
2. Swap MUSS als optionaler Backing-Mechanismus und nicht als garantierter physischer Speicher behandelt werden.
3. Nur explizit auslagerbare Pages DÜRFEN geswappt werden.
4. Pinned, hardwaregebundener und geschützter Realtime-Speicher DARF NICHT unkontrolliert ausgelagert werden.
5. Ein physischer Frame DARF erst nach erfolgreicher Sicherung seines notwendigen Inhalts freigegeben werden.
6. Clean file-backed Pages SOLLEN ohne unnötiges Swap-Out verworfen werden können.
7. Swap-In MUSS über den Page-Fault- und Demand-Paging-Pfad integrierbar sein.
8. Persistenter Swap SOLL verschlüsselt werden können.
9. Swap-Fehler DÜRFEN nicht zum unbemerkten Verlust der einzigen gültigen Speicherkopie führen.
10. NovaOS SOLL Swap-Thrashing erkennen und darauf reagieren können.
11. Swap DARF die OOM- und Memory-Pressure-Semantik NICHT ersetzen.
12. Swap-Nutzung, Latenzen, Provider und Fehler SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-RECLAIM-0001`
- `NPSPEC-MEMORY-OOM-0001`
- `NPSPEC-MEMORY-VAS-0001`
- `NPSPEC-MEMORY-PAGING-0001`
- `NPSPEC-MEMORY-DEMANDPAGING-0001`
- `NPSPEC-MEMORY-MAPPING-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-MEMORY-0018`

## Ergebnis

```text
Memory Pressure
      ↓
Reclamation
      ↓
Swappable Page
      ↓
Swap Out
      ↓
Release Physical Frame
      ↓
Later Access
      ↓
Page Fault
      ↓
Swap In
      ↓
Restore Mapping
```

NovaOS erhält damit einen optionalen Swap-Mechanismus, der physische Speicherknappheit kontrolliert abfedern kann, ohne Swap als Voraussetzung für den Betrieb oder als Ersatz für Reclamation, Resource Budgets und OOM-Management zu behandeln.