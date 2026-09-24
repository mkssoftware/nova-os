# NPSPEC-SYNC-LOCKFREE-0001 – Nova Lock-Free Synchronization

## Status

Entwurf

## Kategorie

Synchronization / Lock-Free / Concurrency / Non-Blocking Algorithms

## Zweck

NovaOS definiert Anforderungen für lockfreie Synchronisationsmechanismen und Datenstrukturen, bei denen konkurrierende Tasks Fortschritt erzielen können, ohne gegenseitig von einem exklusiven Lock abhängig zu sein.

```text
Task A ─┐
Task B ─┼→ Atomic Shared State
Task C ─┘
          ↓
     Lock-Free Progress
```

Lock-Free Synchronization dient insbesondere für stark konkurrierende Kernelpfade, Scheduler-Strukturen, Queues und andere latenzkritische Komponenten.

## Grundprinzip

Lock-Free bedeutet:

```text
Systemweiter Fortschritt
ohne
exklusiven blockierenden Lock
```

Dabei gilt:

```text
Lock-Free ≠ Wait-Free
Lock-Free ≠ Starvation-Free
Lock-Free ≠ Faster in Every Case
```

Bei Lock-Free garantiert der Algorithmus systemweiten Fortschritt, jedoch nicht zwingend den Fortschritt jedes einzelnen Tasks.

## Progress Guarantees

NovaOS unterscheidet mindestens:

```text
Blocking
Lock-Free
Wait-Free
```

### Blocking

Ein Task kann den Fortschritt anderer Tasks durch gehaltene Locks verhindern.

### Lock-Free

Mindestens ein konkurrierender Teilnehmer erzielt innerhalb begrenzter Systemschritte Fortschritt.

### Wait-Free

Jeder Teilnehmer erzielt innerhalb einer begrenzten Anzahl eigener Schritte Fortschritt.

Eine Implementierung darf keine stärkere Progress-Garantie deklarieren als tatsächlich erfüllt wird.

## Atomare Grundlage

Lock-Free Algorithmen basieren auf atomaren Operationen.

Typische Primitive:

```text
Load
Store
Exchange
CompareExchange
FetchAdd
```

Beispiel:

```text
Read State
    ↓
Calculate New State
    ↓
CompareExchange
   ↙          ↘
Success      Failure
  ↓            ↓
Done         Retry
```

## Retry-Loops

Lock-Free Algorithmen dürfen optimistische Retry-Loops verwenden.

```text
Observe
   ↓
Prepare Update
   ↓
CAS
   ↓
Failed?
   ↓
Retry
```

Retry-Loops müssen so gestaltet sein, dass hohe Contention nicht zu unkontrolliertem CPU-Verbrauch führt.

Backoff oder Scheduler-Kooperation darf eingesetzt werden.

## ABA-Problem

Compare-and-Exchange kann durch das ABA-Problem beeinflusst werden.

```text
A → B → A
```

Obwohl der Wert wieder `A` ist, kann sich der zugrunde liegende Zustand verändert haben.

NovaOS muss geeignete Gegenmaßnahmen unterstützen können.

Beispiele:

```text
Version Tags
Generation Counters
Tagged Pointers
Safe Reclamation
```

## Memory Reclamation

Speicher darf nicht freigegeben werden, solange konkurrierende Tasks noch darauf zugreifen könnten.

```text
Remove Object
     ↓
Still Referenced?
     ↓
Deferred Reclamation
```

Mögliche Mechanismen:

```text
Epoch-Based Reclamation
Hazard Pointers
Reference Tracking
RCU-like Grace Periods
```

Die konkrete Methode hängt von der Datenstruktur und ihrem Einsatzgebiet ab.

## Memory Ordering

Lock-Free Algorithmen müssen explizite Memory-Ordering-Regeln verwenden.

```text
Relaxed
Acquire
Release
AcquireRelease
SequentiallyConsistent
```

Die verwendete Ordnung muss ausreichen, um die spezifizierte Semantik auf allen unterstützten Architekturen sicherzustellen.

Eine auf x86 zufällig funktionierende Implementierung darf nicht als architekturunabhängig korrekt betrachtet werden.

## Lock-Free Datenstrukturen

NovaOS darf lockfreie Varianten bereitstellen für:

```text
Queues
Stacks
Counters
Free Lists
Ring Buffers
State Machines
Work Queues
Reference Structures
```

Nicht jede Datenstruktur muss lockfrei implementiert werden.

## Auswahl des Mechanismus

Lock-Free Synchronisation soll nur verwendet werden, wenn sie einen konkreten Vorteil bietet.

```text
Contention
Latency Requirement
Interrupt Context
Scalability
Progress Requirement
Complexity
```

Ein einfacher Mutex darf bevorzugt werden, wenn er bessere Verständlichkeit und ausreichende Performance bietet.

```text
Lock-Free ≠ Default Synchronization
```

## Interrupt-Kontext

Lock-Free Strukturen können für Kontexte geeignet sein, in denen Blocking nicht erlaubt ist.

```text
Interrupt
   ↓
Atomic Operation
   ↓
Lock-Free Queue
   ↓
Deferred Processing
```

Die verwendete Implementierung muss jedoch ausdrücklich für den jeweiligen Kontext geeignet sein.

## Preemption

Ein pausierter Task darf den globalen Fortschritt einer lockfreien Struktur nicht dauerhaft verhindern.

```text
Task A paused
     ↓
Task B continues
     ↓
Structure progresses
```

Dies unterscheidet Lock-Free Mechanismen von Locks, deren Besitzer präemptiert werden kann.

## Contention

Unter hoher Konkurrenz können wiederholte atomare Updates teuer werden.

```text
Many CPUs
    ↓
Same Cache Line
    ↓
CAS Failures
    ↓
Cache Coherency Traffic
```

NovaOS soll unnötig stark umkämpfte globale Atomics vermeiden.

Per-CPU- oder partitionierte Strukturen dürfen bevorzugt werden.

## Realtime

Lock-Free bedeutet nicht automatisch Realtime-tauglich.

Ein einzelner Task kann theoretisch wiederholt durch konkurrierende Operationen verdrängt werden.

```text
Lock-Free
   ≠
Bounded Per-Task Latency
```

Hard-Realtime-Anforderungen benötigen eine ausreichend starke Progress-Garantie und analysierbare Worst-Case-Latenzen.

## Verifikation

Lock-Free Algorithmen sind besonders anfällig für schwer reproduzierbare Race Conditions.

Kritische Implementierungen sollen daher durch geeignete Verfahren geprüft werden.

Beispiele:

```text
Stress Testing
Model Checking
Race Detection
Memory Ordering Verification
Formal Specification
```

## Normative Anforderungen

1. NovaOS MUSS Lock-Free und Wait-Free als unterschiedliche Progress-Garantien behandeln.
2. Eine Implementierung DARF keine stärkere Progress-Garantie deklarieren als tatsächlich erfüllt wird.
3. Lock-Free Algorithmen MÜSSEN auf den Nova Atomic Operations aufbauen.
4. Memory Ordering MUSS explizit und architekturunabhängig korrekt definiert sein.
5. Speicher DARF NICHT freigegeben werden, solange konkurrierende Zugriffe noch möglich sind.
6. ABA-Risiken MÜSSEN bei betroffenen Datenstrukturen berücksichtigt werden.
7. Retry-Loops DÜRFEN NICHT zu unkontrolliertem CPU-Verbrauch führen.
8. Lock-Free Synchronisation DARF NICHT automatisch gegenüber einfacheren Synchronisationsmechanismen bevorzugt werden.
9. Nicht blockierbare Kontexte DÜRFEN nur dafür geeignete Lock-Free-Implementierungen verwenden.
10. Hohe Contention SOLL durch Partitionierung, Per-CPU-Strukturen oder andere geeignete Verfahren reduziert werden.
11. Hard-Realtime-Code DARF Lock-Free NICHT automatisch mit begrenzter Worst-Case-Latenz gleichsetzen.
12. Kritische Lock-Free-Strukturen SOLLEN intensiv verifiziert und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `ADR-SYNC-0007`
- `ADR-VERIFY-0002_Model_Checking_für_kritische_State_Machines`
- `ADR-VERIFY-0003_Memory_Safety`

## Ergebnis

```text
Shared Concurrent State
        ↓
Atomic Operations
        ↓
Lock-Free Algorithm
        ↓
Safe Memory Reclamation
        ↓
Systemwide Progress
```

NovaOS erhält damit eine Grundlage für nicht blockierende Synchronisationsmechanismen, die hohe Parallelität und systemweiten Fortschritt ermöglichen, ohne Lock-Free unnötig als universellen Ersatz für klassische Synchronisation zu behandeln.