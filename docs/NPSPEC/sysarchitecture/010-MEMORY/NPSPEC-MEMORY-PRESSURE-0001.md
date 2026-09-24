# NPSPEC-MEMORY-PRESSURE-0001 – Nova Memory Pressure Management

## Status

Entwurf

## Kategorie

Memory / Memory Pressure / Resource Management / Reclamation

## Zweck

NovaOS definiert Memory Pressure als systemweiten Mechanismus zur Erkennung und kontrollierten Behandlung knapper Speicherressourcen.

```text
Available Memory
      ↓
Pressure Detection
      ↓
Resource Response
      ↓
Reclaim / Reduce / Migrate / Fail
```

Ziel ist, Speicherknappheit frühzeitig zu erkennen und kontrolliert darauf zu reagieren, bevor kritische Komponenten ihre Funktionsfähigkeit verlieren.

## Grundprinzip

Memory Pressure beschreibt nicht nur vollständig erschöpften Speicher.

```text
Memory Pressure ≠ Out of Memory
```

Der Zustand beginnt bereits dann, wenn verfügbare Speicherreserven sinken und Gegenmaßnahmen erforderlich werden.

```text
Normal
  ↓
Elevated
  ↓
High
  ↓
Critical
  ↓
Exhausted
```

Die Übergänge müssen anhand messbarer Ressourcenbedingungen erfolgen.

## Pressure-Modell

NovaOS muss mindestens folgende Informationen berücksichtigen können:

```text
Free Physical Memory
Available Frames
Reclaimable Memory
Memory Pools
Slab Usage
NUMA Availability
Reserved Memory
Allocation Failure Rate
Fragmentation
```

Die reine Menge freier Bytes ist nicht ausreichend.

Beispiel:

```text
Free Memory = High
Largest Contiguous Block = Small
```

kann trotzdem für bestimmte Allokationen Memory Pressure bedeuten.

## Pressure-Level

### Normal

Ausreichende Ressourcen verfügbar.

```text
Normal Allocation
Normal Caching
Normal Prefetching
```

### Elevated

Speicherreserven beginnen zu sinken.

Mögliche Maßnahmen:

```text
Reduce Prefetching
Limit Cache Growth
Begin Background Reclamation
```

### High

Aktive Speicherfreigabe wird erforderlich.

```text
Reclaim Caches
Release Empty Slabs
Drain Excess Pools
Reduce Background Memory
```

### Critical

Nur noch begrenzte Reserven stehen zur Verfügung.

```text
Aggressive Reclamation
Restrict Noncritical Allocation
Degrade Optional Services
Protect Critical Reserves
```

### Exhausted

Eine angeforderte Ressource kann trotz zulässiger Maßnahmen nicht bereitgestellt werden.

```text
Allocation Request
      ↓
No Valid Memory Available
      ↓
Explicit Allocation Failure
```

## Watermarks

Speicherbereiche dürfen Watermarks besitzen.

```text
High Watermark
     ↓
Normal

Low Watermark
     ↓
Reclamation

Critical Watermark
     ↓
Emergency Measures
```

Watermarks dürfen global, pro NUMA-Node, Zone oder Ressourcenklasse definiert werden.

## Reclamation

Bei Memory Pressure dürfen reclaimable Ressourcen freigegeben werden.

Beispiele:

```text
Filesystem Cache
Unused Object Caches
Empty Slabs
Unused Memory Pools
Discardable Buffers
Prefetched Data
Reconstructable Data
```

Reclamation darf keine aktiven oder nicht rekonstruierbaren Daten verlieren.

## Reclaim Priority

Speicher soll nach semantischer Bedeutung freigegeben werden.

Konzeptionell:

```text
Discardable
    ↓
Reconstructable
    ↓
Cached
    ↓
Inactive
    ↓
Important
    ↓
Critical / Pinned
```

Kritische Ressourcen dürfen nicht allein aufgrund ihrer Größe bevorzugt entfernt werden.

## Resource Economy

Memory Pressure ist Bestandteil der systemweiten Resource Economy.

```text
Memory Pressure
      ↓
Resource Economy
      ↓
Scheduler / Storage / Cache / Services
```

Andere Subsysteme müssen auf Druck reagieren können, ohne eigene widersprüchliche Reclamation-Strategien zu entwickeln.

## NUMA Pressure

Memory Pressure muss pro NUMA-Node erkennbar sein.

```text
Node 0 → Critical
Node 1 → Normal
```

Mögliche Reaktionen:

```text
Remote Allocation
Memory Migration
Task Migration
Local Reclamation
```

Ein lokaler Engpass darf nicht automatisch als globaler Speichermangel behandelt werden.

## Pools und Reserven

Memory Pools können eigene Pressure-Zustände besitzen.

```text
Pool Capacity
     ↓
Pool Pressure
```

Kritische und Emergency Pools müssen vor allgemeiner Reclamation geschützt bleiben.

```text
General Memory Pressure
        ≠
Permission to consume Emergency Reserve
```

## Realtime

Realtime-Ressourcen dürfen nicht unkontrolliert durch globale Reclamation beeinträchtigt werden.

Vorreservierte Realtime-Pools und garantierte Memory Budgets müssen entsprechend ihrer Verträge geschützt werden.

```text
Realtime Reservation
        ↓
Protected Capacity
```

Neue Garantien dürfen abgelehnt werden, wenn die erforderlichen Ressourcen nicht zuverlässig verfügbar sind.

## Allocation Throttling

Bei hohem Memory Pressure dürfen nichtkritische Allokationen begrenzt werden.

```text
Allocation
    ↓
Pressure Policy
    ↓
Allow / Delay / Reclaim / Fail
```

Unbegrenztes Warten ist nicht zulässig, wenn dadurch Deadlocks oder Ressourcenzyklen entstehen können.

## Graceful Degradation

Nichtkritische Komponenten sollen ihre Speichernutzung kontrolliert reduzieren können.

Beispiele:

```text
Smaller Caches
Lower History Depth
Reduced Prefetch
Reduced Preview Quality
Suspend Background Work
```

Harte Sicherheits-, Konsistenz- oder Nutzeranforderungen dürfen dabei nicht verletzt werden.

## Out-of-Memory

Kann Speicher nicht mehr bereitgestellt werden:

```text
Reclamation Failed
      ↓
Fallback Failed
      ↓
Allocation Failure
```

NovaOS darf nicht grundsätzlich einen beliebigen Prozess beenden, nur um Speicher zurückzugewinnen.

Recovery muss anhand von Ownership, Resource Budgets, Kritikalität und Systemzustand erfolgen.

## Feedback

Memory Pressure soll als Feedbacksignal für adaptive Komponenten verfügbar sein.

```text
Pressure
   ↓
Observe
   ↓
Adjust Resource Usage
   ↓
Measure Result
```

Adaptive Optimierung darf harte Ressourcen- und Sicherheitsgrenzen nicht überschreiben.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Global Pressure Level
Pressure per NUMA Node
Free Memory
Reclaimable Memory
Reserved Memory
Fragmentation
Allocation Failures
Reclamation Activity
Protected Reserves
Pressure History
```

Die Ursache eines kritischen Speicherzustands soll nachvollziehbar sein.

## Normative Anforderungen

1. NovaOS MUSS Memory Pressure als expliziten Systemzustand behandeln.
2. Memory Pressure MUSS vor vollständiger Speichererschöpfung erkennbar sein.
3. Pressure-Level MÜSSEN anhand messbarer Speicherzustände bestimmt werden.
4. Globaler und NUMA-lokaler Memory Pressure MÜSSEN unterscheidbar sein.
5. Reclamation DARF aktive oder nicht rekonstruierbare Daten NICHT unkontrolliert verwerfen.
6. Reclaimable Ressourcen SOLLEN nach semantischer Bedeutung priorisiert werden.
7. Kritische und Emergency-Reserven MÜSSEN vor normaler Speichernutzung geschützt bleiben.
8. Garantierte Realtime-Ressourcen DÜRFEN durch normale Reclamation NICHT verletzt werden.
9. Nichtkritische Allokationen DÜRFEN bei hohem Pressure gedrosselt oder abgelehnt werden.
10. Speichererschöpfung MUSS als expliziter Fehler behandelbar sein.
11. Adaptive Optimierung DARF harte Resource Budgets und Sicherheitsregeln NICHT überschreiben.
12. Pressure-Zustand, Reclamation und Allokationsfehler SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-SLAB-0001`
- `NPSPEC-MEMORY-POOL-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-MEMORY-0007`

## Ergebnis

```text
Memory State
     ↓
Pressure Detection
     ↓
Normal → Elevated → High → Critical
                         ↓
                    Reclamation
                         ↓
             Reduce / Migrate / Degrade
                         ↓
                  Memory Available?
                    ↙          ↘
                  Yes           No
                   ↓             ↓
                Continue   Explicit Failure
```

NovaOS erhält damit ein systemweites Memory-Pressure-Modell, das Speicherknappheit früh erkennt, Reclamation koordiniert, kritische Reserven schützt und kontrollierte Degradation vor unvorhersehbarem Ressourcenversagen bevorzugt.