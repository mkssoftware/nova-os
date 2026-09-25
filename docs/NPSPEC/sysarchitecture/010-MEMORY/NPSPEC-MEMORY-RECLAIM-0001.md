# NPSPEC-MEMORY-RECLAIM-0001 – Nova Memory Reclamation

## Status

Angenommen

## Kategorie

Memory / Reclamation / Resource Management / Memory Pressure

## Zweck

NovaOS definiert Memory Reclamation als kontrollierten Mechanismus zur Rückgewinnung nicht mehr benötigter oder rekonstruierbarer Speicherressourcen.

```text
Memory Pressure
      ↓
Find Reclaimable Memory
      ↓
Evaluate Reclaim Cost
      ↓
Release / Evict / Shrink
      ↓
Memory Available
```

Reclamation soll Speicher zurückgewinnen, ohne aktive, kritische oder nicht rekonstruierbare Daten zu beschädigen.

## Grundprinzip

Nicht jeder belegte Speicher besitzt dieselbe Bedeutung.

```text
Allocated Memory
├── Active
├── Cached
├── Reconstructable
├── Discardable
├── Pinned
└── Critical
```

Dabei gilt:

```text
Allocated ≠ Unreclaimable
Unused ≠ Immediately Reclaimable
Reclaimable ≠ Free
```

Speicher darf erst als frei gelten, nachdem die jeweilige Reclamation vollständig abgeschlossen wurde.

## Reclaimable Memory

Subsysteme müssen Speicher als reclaimable klassifizieren können.

Typische Kandidaten:

```text
Filesystem Cache
Page Cache
Unused Slabs
Empty Memory Pools
Prefetched Data
Derived Data
Inactive Buffers
Discardable UI Resources
Cached Computation Results
```

Aktiver oder semantisch notwendiger Zustand darf nicht allein aufgrund von Memory Pressure verworfen werden.

## Reclaim-Klassen

NovaOS soll mindestens folgende semantische Klassen unterscheiden können:

```text
Discardable
Reconstructable
Cached
Inactive
Protected
Pinned
Critical
```

Bevorzugte Reihenfolge:

```text
Discardable
    ↓
Reconstructable
    ↓
Cached
    ↓
Inactive
```

`Protected`, `Pinned` und `Critical` benötigen besondere Freigabebedingungen.

## Reclaim Cost

Reclamation soll nicht ausschließlich anhand der freisetzbaren Byte-Anzahl entschieden werden.

Berücksichtigt werden dürfen:

```text
Memory Gain
Reconstruction Cost
I/O Cost
CPU Cost
Latency
Energy Cost
Future Use Probability
NUMA Locality
Resource Priority
```

Konzeptionell:

```text
Reclaim Benefit
      vs.
Reconstruction Cost
```

Harte Systemanforderungen haben Vorrang vor Optimierungsentscheidungen.

## Reclaim Request

Der Memory Manager darf Reclaim Requests erzeugen.

```text
ReclaimRequest
├── TargetAmount
├── PressureLevel
├── PreferredNode
├── Deadline
└── Constraints
```

Subsysteme können daraufhin geeignete Ressourcen freigeben.

## Kooperative Reclamation

Subsysteme sollen selbst beschreiben können, welche Ressourcen sicher freigegeben werden dürfen.

```text
Memory Manager
      ↓
Reclaim Request
      ↓
Subsystem
      ↓
Select Safe Resources
      ↓
Release Memory
```

Dadurch muss der zentrale Memory Manager keine subsysteminternen Semantiken kennen.

## Background Reclamation

Bei moderatem Memory Pressure soll Reclamation im Hintergrund erfolgen können.

```text
Elevated Pressure
       ↓
Background Reclaimer
       ↓
Gradual Memory Recovery
```

Ziel ist, kritischen Speichermangel möglichst zu vermeiden.

Background Reclamation darf interaktive oder zeitkritische Workloads nicht unnötig beeinträchtigen.

## Direct Reclamation

Kann eine Allokation nicht unmittelbar erfüllt werden, darf der ausführende Pfad kontrollierte direkte Reclamation auslösen.

```text
Allocation Failed
      ↓
Direct Reclaim
      ↓
Retry Allocation
```

Direct Reclamation darf nicht verwendet werden, wenn der aktuelle Kontext nicht blockieren oder entsprechende Ressourcenabhängigkeiten einen Deadlock verursachen könnten.

## NUMA Reclamation

Reclamation muss NUMA-lokal möglich sein.

```text
Node 0 Pressure
      ↓
Reclaim Node 0
```

Erst danach darf entsprechend der Policy geprüft werden:

```text
Remote Allocation
Memory Migration
Task Migration
```

Lokaler Pressure darf nicht automatisch unnötige globale Reclamation auslösen.

## Slab Reclamation

Der Slab Allocator muss vollständig freie Slabs zurückgeben können.

```text
Empty Slab
    ↓
Release Frames
    ↓
Frame Allocator
```

Teilweise belegte Slabs dürfen nur durch geeignete Objekt- oder Cache-Policies reduziert werden.

## Pool Reclamation

Memory Pools dürfen überschüssige ungenutzte Kapazität zurückgeben, sofern ihre Policy dies erlaubt.

```text
Pool
 ↓
Unused Capacity
 ↓
Shrink
 ↓
Return Memory
```

Harte Pool-Reservierungen dürfen dabei nicht verletzt werden.

## Pinned Memory

Bestimmte Speicherbereiche dürfen nicht ohne Weiteres reclaimt werden.

Beispiele:

```text
Active DMA Buffers
Critical Kernel State
Page Tables
Realtime Reservations
Hardware-owned Memory
```

```text
Pinned ≠ Reclaimable
```

Erst nach Aufhebung der jeweiligen Bindung darf der Speicher wieder Reclamation-Kandidat werden.

## Realtime

Realtime-reservierter Speicher muss entsprechend seiner Garantie geschützt werden.

```text
Realtime Reservation
        ↓
Protected from Normal Reclaim
```

Reclamation darf keine unbeschränkten Latenzen in Hard-Realtime-Pfaden verursachen.

Neue Realtime-Reservierungen dürfen abgelehnt werden, wenn ihre Speicheranforderungen nicht zuverlässig garantiert werden können.

## Thrashing

Aggressive Reclamation kann dazu führen, dass kurz darauf dieselben Daten erneut erzeugt oder geladen werden müssen.

```text
Reclaim
   ↓
Immediate Reuse
   ↓
Reload
   ↓
Reclaim
   ↓
Thrashing
```

NovaOS soll solche Muster erkennen und Reclaim-Entscheidungen entsprechend anpassen können.

## Adaptive Reclamation

Reclaim-Policies dürfen aus beobachteten Nutzungsmustern lernen.

```text
Reclaim Decision
      ↓
Later Reuse?
      ↓
Prediction Error
      ↓
Policy Adjustment
```

Adaptive Entscheidungen bleiben Soft Optimization und dürfen harte Constraints nicht überschreiben.

## Fehlerfall

Kann nicht ausreichend Speicher zurückgewonnen werden:

```text
Reclaim
   ↓
Target not reached
   ↓
Pressure remains
   ↓
Escalate / Degrade / Fail Allocation
```

Reclamation darf nicht unbegrenzt wiederholt werden.

Retry- und Reclaim-Budgets müssen Ressourcenstürme verhindern können.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
Reclaimed Bytes
Reclaimable Bytes
Reclaim Source
Reclaim Cost
Reclaim Duration
Failed Reclaims
NUMA Node
Pressure Level
Thrashing Indicators
Protected Memory
```

Reclaim-Entscheidungen sollen nachvollziehbar sein.

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Memory Reclamation unterstützen.
2. Speicher MUSS vor Reclamation hinsichtlich seiner Freigabefähigkeit klassifiziert werden.
3. Aktive oder nicht rekonstruierbare Daten DÜRFEN NICHT unkontrolliert verworfen werden.
4. Discardable und rekonstruierbare Ressourcen SOLLEN bevorzugt reclaimt werden.
5. Subsysteme SOLLEN eigene sichere Reclaim-Kandidaten bereitstellen können.
6. Background Reclamation SOLL vor kritischer Speichererschöpfung möglich sein.
7. Direct Reclamation DARF NICHT in Kontexten erfolgen, in denen dadurch unzulässiges Blocking oder Deadlock entstehen kann.
8. NUMA-lokale Reclamation SOLL vor unnötiger globaler Reclamation bevorzugt werden.
9. Pinned und garantierte Realtime-Ressourcen DÜRFEN durch normale Reclamation NICHT verletzt werden.
10. Reclaim-Schleifen MÜSSEN durch geeignete Budgets oder Abbruchbedingungen begrenzt werden.
11. Thrashing SOLL erkannt und bei zukünftigen Reclaim-Entscheidungen berücksichtigt werden.
12. Reclaim-Aktivitäten und deren Auswirkungen SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-MEMORY-PMM-0001`
- `NPSPEC-MEMORY-FRAMEALLOCATOR-0001`
- `NPSPEC-MEMORY-SLAB-0001`
- `NPSPEC-MEMORY-POOL-0001`
- `NPSPEC-MEMORY-NUMA-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-MEMORY-0008`

## Ergebnis

```text
Memory Pressure
      ↓
Identify Reclaimable Resources
      ↓
Classify + Evaluate Cost
      ↓
Reclaim Safely
      ↓
Return Memory
      ↓
Pressure Reduced?
   ┌──────┴──────┐
  Yes            No
   ↓              ↓
Continue      Escalate
```

NovaOS erhält damit einen semantisch gesteuerten Reclamation-Mechanismus, der Speicher kontrolliert zurückgewinnt, NUMA- und Realtime-Anforderungen berücksichtigt und unnötiges Thrashing sowie unkontrollierte Ressourcenverluste vermeidet.