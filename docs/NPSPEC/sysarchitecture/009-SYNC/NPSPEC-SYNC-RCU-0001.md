# NPSPEC-SYNC-RCU-0001 – Nova Read-Copy-Update

## Status

Entwurf

## Kategorie

Synchronization / RCU / Read-Mostly Data / Concurrency

## Zweck

NovaOS definiert Read-Copy-Update (RCU) als Synchronisationsmechanismus für gemeinsam genutzte Datenstrukturen mit sehr häufigen Lesezugriffen und vergleichsweise seltenen Änderungen.

```text
Readers ─────────→ Current Version
                       ↑
Writer → Copy → Modify → Publish
                       ↓
                Retire Old Version
```

Ziel ist, Reader möglichst ohne klassische Locks arbeiten zu lassen, während Writer Änderungen kontrolliert veröffentlichen.

## Grundprinzip

RCU trennt Aktualisierung und Speicherfreigabe.

```text
Read
 ↓
Use Current Object

Writer
 ↓
Create / Modify New State
 ↓
Publish
 ↓
Wait Grace Period
 ↓
Reclaim Old State
```

Dabei gilt:

```text
Unpublished ≠ Unreachable
Unreachable ≠ Safe to Free
```

Ein entferntes Objekt darf erst freigegeben werden, wenn kein Reader der vorherigen Generation mehr darauf zugreifen kann.

## Read-Side Critical Section

Reader markieren einen RCU-Lesebereich.

```text
RCUReadLock
    ↓
Load RCU Pointer
    ↓
Read Object
    ↓
RCUReadUnlock
```

Read-Side Critical Sections sollen möglichst kurz sein.

Die konkrete Implementierung darf lockfrei oder nahezu ohne Synchronisationskosten erfolgen.

## Writer

Writer erstellen oder verändern einen neuen Zustand und veröffentlichen ihn anschließend atomar.

```text
Current Object
      ↓
Copy / Prepare
      ↓
New Object
      ↓
Atomic Publish
```

Bestehende Reader dürfen weiterhin die vorherige Version verwenden.

Neue Reader sehen nach der Veröffentlichung den neuen Zustand entsprechend der definierten Memory-Ordering-Semantik.

## Publication

Die Veröffentlichung eines neuen RCU-Zustands muss atomar erfolgen.

```text
Old Pointer
     ↓
Atomic Replace
     ↓
New Pointer
```

Publication muss mindestens die erforderliche Release-Semantik besitzen.

Reader müssen den veröffentlichten Zustand mit entsprechender Acquire-Semantik beobachten können.

## Grace Period

Nach einer Änderung beginnt eine Grace Period.

```text
Publish New Version
        ↓
Existing Readers Continue
        ↓
All Preexisting Readers Exit
        ↓
Grace Period Complete
```

Erst danach darf der alte Zustand grundsätzlich zurückgewonnen werden.

## Quiescent State

Ein Quiescent State zeigt an, dass ein Ausführungskontext keine Referenz aus einem vorherigen RCU-Lesebereich mehr verwendet.

Mögliche Quiescent States können abhängig von der Implementierung sein:

```text
Task Context Switch
Explicit RCU Exit
Idle Transition
Scheduler Quiescent Point
```

Die konkrete Erkennung bleibt Implementierungsdetail.

## Memory Reclamation

Alte Objekte werden zunächst zurückgestellt.

```text
Remove
  ↓
Retire
  ↓
Grace Period
  ↓
Reclaim
```

NovaOS darf Deferred-Reclamation-Queues verwenden.

```text
Retired Objects
├── Object A
├── Object B
└── Object C
       ↓
Grace Period Complete
       ↓
Free
```

Speicher darf nicht vor Ende der relevanten Grace Period freigegeben werden.

## Synchronize

NovaOS soll eine Operation bereitstellen, mit der ein Writer auf das Ende einer Grace Period warten kann.

Konzeptionell:

```text
Publish
   ↓
SynchronizeRCU
   ↓
Grace Period Complete
   ↓
Safe Reclamation
```

Diese Operation darf blockieren und ist daher nicht für jeden Kontext geeignet.

## Asynchrone Reclamation

Alternativ darf die Freigabe asynchron erfolgen.

```text
Retire Object
     ↓
Register Callback
     ↓
Grace Period
     ↓
Callback
     ↓
Free Object
```

Dadurch muss der Writer nicht auf die Grace Period warten.

## Preemption

RCU muss mit dem präemptiven Nova Scheduler zusammenarbeiten.

Ein präemptierter Reader kann eine Grace Period verlängern.

```text
Reader Active
    ↓
Preempted
    ↓
Grace Period Waits
```

Die RCU-Implementierung muss aktive Reader korrekt über Scheduling-Wechsel hinweg verfolgen.

## CPU Hotplug

CPU-Hotplug darf Grace-Period-Erkennung nicht beschädigen.

```text
CPU Offline
    ↓
RCU State Transfer
    ↓
Remove from Active Set
```

Eine entfernte CPU darf keine Grace Period dauerhaft blockieren.

## RCU und Locks

RCU ersetzt nicht sämtliche Locks.

```text
RCU
→ Efficient Reads
→ Deferred Reclamation

Mutex / Atomic
→ Writer Coordination
→ Complex State Changes
```

Writer dürfen zusätzliche Synchronisation benötigen, wenn mehrere Writer denselben Zustand verändern.

## Typische Einsatzbereiche

RCU eignet sich insbesondere für:

```text
System Registries
Routing Tables
Capability Lookups
Process Tables
Configuration Views
Read-Mostly Metadata
Scheduler Metadata
Object Registries
```

Für häufig veränderte Datenstrukturen kann RCU ungeeignet sein.

## Realtime

Grace Periods und Reclamation müssen bei Realtime-Systemen kontrolliert werden.

Ein dauerhaft laufender oder blockierter Reader darf nicht zu unbegrenztem Ressourcenwachstum führen.

Hard-Realtime-Pfade dürfen nur RCU-Varianten verwenden, deren relevante Latenzen ausreichend begrenzt und analysierbar sind.

## Normative Anforderungen

1. NovaOS SOLL RCU für geeignete read-mostly Datenstrukturen unterstützen.
2. Reader SOLLEN ohne exklusiven Writer-Lock lesen können.
3. Die Veröffentlichung neuer Zustände MUSS atomar erfolgen.
4. Publication MUSS definierte Memory-Ordering-Garantien besitzen.
5. Alte Objekte DÜRFEN NICHT vor Ende ihrer relevanten Grace Period freigegeben werden.
6. NovaOS MUSS aktive RCU-Reader über Scheduler- und CPU-Wechsel korrekt berücksichtigen.
7. Grace Periods MÜSSEN eindeutig bestimmbar sein.
8. Synchrone und/oder asynchrone Reclamation MUSS unterstützt werden können.
9. CPU-Hotplug DARF Grace-Period-Erkennung NICHT beschädigen.
10. Writer-Synchronisation MUSS bei konkurrierenden Änderungen separat gewährleistet werden.
11. RCU DARF NICHT als allgemeiner Ersatz für Mutexes oder atomare Operationen behandelt werden.
12. Grace Periods, Reader-Zustände und ausstehende Reclamation SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-SYNC-LOCKFREE-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-HOTPLUG-0001`
- `ADR-SYNC-0007`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Readers
   ↓
Read Current Version
   ↓
Continue Without Writer Lock

Writer
   ↓
Prepare New Version
   ↓
Atomic Publish
   ↓
Grace Period
   ↓
Safe Reclamation
```

NovaOS erhält damit einen effizienten Synchronisationsmechanismus für read-mostly Datenstrukturen, der schnelle parallele Lesezugriffe mit atomarer Veröffentlichung und sicher verzögerter Speicherfreigabe verbindet.