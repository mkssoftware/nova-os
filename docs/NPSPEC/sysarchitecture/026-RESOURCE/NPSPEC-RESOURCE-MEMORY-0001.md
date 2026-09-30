# NPSPEC-RESOURCE-MEMORY-0001 – Nova Memory Resource Model

## Status

Angenommen

## Kategorie

Resource / Memory / Resource Management

## Zweck

NovaOS definiert Arbeitsspeicher als explizite, budgetierbare, messbare und kontrollierbare Systemressource.

```text
Memory Demand
     ↓
Budget + Policy
     ↓
Memory Allocation
     ↓
Usage
     ↓
Accounting + Reclaim
```

Das Modell verbindet Memory Management mit Resource Economy, Execution Contracts, NUMA, Memory Pressure und Capability Security.

## Grundprinzipien

```text
Available Memory ≠ Authorized Memory
Allocated Memory ≠ Resident Memory
Reserved Memory ≠ Consumed Memory
Virtual Memory ≠ Physical Memory
Shared Memory ≠ Shared Authority
Memory Mapping ≠ Object Authority
Memory Budget ≠ Guaranteed Allocation
Free Memory ≠ Wasted Memory
```

## Memory Resource

Eine Speicherressource wird semantisch beschrieben durch:

```text
MemoryResource
├── ResourceID
├── ResourceTypeID
├── Capacity
├── State
└── Memory Domain
```

Optional:

```text
NUMA Node
Physical / Virtual
Persistent / Volatile
Shared / Private
Pinned
DMA Capable
Executable
Encrypted
Latency Class
Bandwidth Class
```

## Memory Requirement

Workloads können ihren Speicherbedarf deklarieren.

```text
MemoryRequirement
├── Required Capacity
├── Maximum Capacity
└── Memory Class
```

Optional:

```text
NUMA Preference
Latency Requirement
Bandwidth Requirement
Sharing Policy
Persistence
DMA Requirement
Pinning
Huge Pages
Security Domain
```

## Memory Budget

Resource Economy kann Speicherbudgets definieren.

```text
MemoryBudget
├── Guaranteed Minimum
├── Normal Limit
├── Burst Limit
└── Enforcement Policy
```

Budgets können hierarchisch gelten:

```text
System
 ↓
Application
 ↓
Process
 ↓
Task
```

Dabei gilt:

```text
Child Budget ≤ Parent Available Budget
```

## Speicherzustände

NovaOS unterscheidet mindestens:

```text
Reserved
Allocated
Committed
Resident
Shared
Reclaimable
Pinned
Swapped
Compressed
```

Diese Zustände dürfen im Accounting nicht gleichgesetzt werden.

## Physical und Virtual Memory

Das Resource Model bleibt von der konkreten Speicherverwaltung getrennt.

```text
Virtual Memory Requirement
        ↓
Memory Manager
        ↓
Physical Frames
```

PMM, Paging, Page Tables und virtuelle Adressräume implementieren die Mechanismen.

Resource Economy definiert dagegen Budgets und Policy.

## NUMA

Speicher kann einem NUMA Node zugeordnet sein.

```text
Task
 ↓
Preferred NUMA Node
 ↓
Local Memory
```

NovaOS soll bevorzugt Speicher mit hoher Lokalität verwenden.

Hard NUMA Constraints dürfen nicht durch automatische Optimierung verletzt werden.

## Shared Memory

Shared Memory muss separat abrechenbar sein.

```text
Process A ─┐
           ├── Shared Memory
Process B ─┘
```

Die Accounting Policy kann beispielsweise definieren:

```text
Owner Pays
Requester Pays
Proportional Share
Shared Infrastructure
```

Shared Memory erzeugt keine gemeinsame Capability Domain.

## Memory Pressure

NovaOS definiert kontrollierte Pressure-Zustände.

```text
Normal
 ↓
Elevated
 ↓
Pressure
 ↓
Critical
```

Mögliche Reaktionen:

```text
Cache Reclaim
Memory Compression
Deduplication
Swap
Budget Enforcement
Throttling
Graceful Degradation
OOM Handling
```

## Reclaim

Reclaimable Memory soll bevorzugt freigegeben werden können.

```text
Pressure
  ↓
Identify Reclaimable Memory
  ↓
Reclaim
  ↓
Re-Evaluate Pressure
```

Hard Reservations und nicht reclaimbare Speicherbereiche müssen berücksichtigt werden.

## OOM

Speichererschöpfung darf nicht automatisch zu unkontrolliertem Systemversagen führen.

```text
Critical Pressure
      ↓
Reclaim
      ↓
Degradation
      ↓
Budget Enforcement
      ↓
Controlled OOM Decision
```

OOM-Entscheidungen sollen Resource Domains und Prioritäten berücksichtigen.

## ExecutionContract

Ein ExecutionContract kann Speicheranforderungen enthalten.

```text
ExecutionContract
├── Minimum Memory
├── Maximum Memory
├── NUMA Constraints
├── Latency Requirements
└── Memory Security Requirements
```

Kann ein Hard Requirement nicht erfüllt werden, muss die Ausführung kontrolliert fehlschlagen oder neu geplant werden.

## Zero-Copy

Memory Resources bilden die Grundlage für Zero-Copy.

```text
Object
 ↓
Mapped / Shared Memory
 ↓
Consumer
```

Dabei bleiben:

```text
Memory Access
Object Authority
Capability Authority
```

getrennt.

## Memory Accounting

Mindestens folgende Werte sollen erfassbar sein:

```text
Reserved
Allocated
Resident
Shared
Pinned
Compressed
Swapped
Reclaimable
```

Die Zuordnung kann erfolgen nach:

```text
Application
Process
Task
Service
Agent
ExecutionContract
Accounting Domain
```

## Sicherheit

Speicherzugriff muss durch Memory Protection und Capabilities kontrolliert werden.

Nicht freigegeben werden dürfen:

```text
Kernel Memory
Other Address Spaces
Protected Shared Memory
Uninitialized Memory
Previous Buffer Contents
```

Wiederverwendeter Speicher muss entsprechend der Security Policy bereinigt werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Total Capacity
Available Capacity
Reserved Memory
Allocated Memory
Resident Memory
Shared Memory
Pinned Memory
Compressed Memory
Swap Usage
Pressure State
NUMA Distribution
Accounting
```

## Normative Anforderungen

1. NovaOS MUSS Speicher als explizite Systemressource behandeln.
2. Speicherverbrauch MUSS durch Resource Accounting messbar sein.
3. Memory Budgets MÜSSEN hierarchisch begrenzbar sein.
4. Reserved, Allocated, Resident und Consumed Memory MÜSSEN unterscheidbar sein.
5. Shared Memory MUSS eine definierte Accounting Policy besitzen.
6. NUMA-Eigenschaften SOLLEN bei Allocation und Placement berücksichtigt werden.
7. Memory Pressure MUSS kontrollierte Reclaim- und Degradation-Mechanismen auslösen können.
8. OOM MUSS als kontrollierte Resource-Economy-Entscheidung behandelbar sein.
9. Memory Access DARF keine implizite Object- oder Capability-Authority erzeugen.
10. Memory Resources MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-MEMORY-RECLAIM-0001`
- `NPSPEC-MEMORY-OOM-0001`
- `NPSPEC-MEMORY-SHARED-0001`
- `NPSPEC-MEMORY-COMPRESSION-0001`
- `NPSPEC-MEMORY-PROTECTION-0001`
- `ADR-ARCH-0034`

## Ergebnis

```text
Physical + Virtual Memory
          ↓
Memory Resource Model
          ↓
Budget + Allocation + Protection
          ↓
Accounting + Pressure Management
          ↓
Reclaim + Optimization
```

NovaOS erhält damit ein einheitliches Memory Resource Model, das Arbeitsspeicher nicht nur technisch verwaltet, sondern systemweit budgetiert, abrechnet, schützt und unter Speicherknappheit kontrolliert priorisiert.