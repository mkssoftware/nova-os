# NPSPEC-SYNC-MUTEX-0001 – Nova Mutex

## Status

Angenommen

## Kategorie

Synchronization / Mutex / Concurrency / Kernel

## Zweck

NovaOS definiert Mutexes zur kontrollierten exklusiven Nutzung gemeinsam veränderlicher Ressourcen.

```text
Task A ─┐
        ├→ Mutex → Shared Resource
Task B ─┘
```

Zu jedem Zeitpunkt darf höchstens ein berechtigter Ausführungskontext den durch einen Mutex geschützten kritischen Bereich besitzen.

## Grundprinzip

```text
Lock
 ↓
Critical Section
 ↓
Unlock
```

Dabei gilt:

```text
Mutex ≠ Spinlock
Mutex ≠ Atomic Operation
```

Ein wartender Task soll bei längerer Wartezeit blockiert werden können, statt CPU-Zeit durch aktives Warten zu verbrauchen.

## Mutex-Zustand

Ein Mutex besitzt mindestens:

```text
State
Owner
Waiters
```

Mögliche Zustände:

```text
Unlocked
Locked
Contended
```

Optional können Diagnoseinformationen geführt werden:

```text
OwnerTaskID
AcquisitionTime
WaiterCount
ContentionCount
```

## Lock

Ist der Mutex frei:

```text
Unlocked
   ↓
Acquire
   ↓
Locked
```

Der aufrufende Task wird Besitzer des Mutex.

Ist der Mutex bereits belegt:

```text
Locked
  ↓
Contention
  ↓
Wait Queue
  ↓
Task Waiting
```

Der wartende Task soll vom Scheduler blockiert werden können.

## Unlock

Nur der Besitzer darf einen normalen Mutex freigeben.

```text
Owner
  ↓
Unlock
  ↓
Wake Eligible Waiter
  ↓
Ownership Transfer
```

Die Übergabe muss ohne Zustand erfolgen, in dem mehrere Tasks gleichzeitig Besitz annehmen können.

## Wait Queue

Konkurrierende Tasks werden über eine kontrollierte Wait Queue verwaltet.

```text
Mutex
  ↓
Wait Queue
├── Task B
├── Task C
└── Task D
```

Die Auswahl des nächsten Tasks darf Scheduling-Priorität, Realtime-Anforderungen oder Fairness berücksichtigen.

Eine strikt FIFO-basierte Reihenfolge ist nicht zwingend vorgeschrieben.

## Fast Path

Ein uncontended Mutex soll möglichst ohne Scheduler-Wechsel erworben werden können.

```text
Atomic Acquire
     ↓
Success?
 ┌───┴───┐
Yes      No
 ↓        ↓
Fast    Slow
Path    Path
```

Der Fast Path darf atomare Operationen verwenden.

## Slow Path

Bei Contention wird der wartende Task über den Synchronisations- und Scheduler-Mechanismus blockiert.

```text
Acquire Failed
      ↓
Register Waiter
      ↓
Block Task
      ↓
Wakeup
      ↓
Retry / Ownership Transfer
```

Busy Waiting soll bei normalen Mutexes vermieden werden.

## Memory Ordering

Erfolgreiches Locking besitzt mindestens Acquire-Semantik.

Unlock besitzt mindestens Release-Semantik.

```text
Lock    → Acquire
Unlock  → Release
```

Dadurch werden Änderungen innerhalb des kritischen Bereichs korrekt zwischen Tasks sichtbar.

## Priority Inversion

Ein Mutex kann Priority Inversion verursachen.

```text
High Priority Task
      ↓ waits
Low Priority Owner
```

NovaOS muss Mechanismen bereitstellen können, um dies insbesondere bei zeitkritischen Tasks zu begrenzen.

Mögliche Mechanismen:

```text
Priority Inheritance
Priority Ceiling
Bounded Critical Sections
```

Die konkrete Policy wird durch Scheduler- und Realtime-Regeln bestimmt.

## Deadlock

Mutexes verhindern keine Deadlocks.

```text
Task A owns M1 → waits M2
Task B owns M2 → waits M1
```

NovaOS soll Lock-Hierarchien und Diagnosemechanismen unterstützen können.

Debug- und Verifikationssysteme dürfen potenzielle Lock-Zyklen erkennen.

## Recursive Mutex

Normale Nova-Mutexes sind standardmäßig nicht rekursiv.

```text
Owner
  ↓
Acquire Same Mutex Again
  ↓
Invalid / Deadlock Detection
```

Falls rekursive Mutexes benötigt werden, müssen sie explizit als eigener Typ oder Modus deklariert werden.

## Cancellation

Ein auf einen Mutex wartender Task darf kontrolliert abbrechbar sein, sofern die verwendete Synchronisationssemantik dies erlaubt.

```text
Waiting
  ↓
Cancellation
  ↓
Remove from Wait Queue
```

Ein bereits erworbener Mutex darf durch Cancellation nicht automatisch freigegeben werden.

Cleanup bleibt Aufgabe des Besitzers bzw. seines kontrollierten Unwind-Pfads.

## Realtime

Realtime-Mutexes müssen zeitlich analysierbares Verhalten ermöglichen.

Kritische Bereiche sollen kurz und begrenzt bleiben.

```text
Realtime Task
      ↓
Mutex Contention
      ↓
Bounded Blocking
```

Unbegrenzte Priority Inversion darf bei garantierter Hard-Realtime-Ausführung nicht auftreten.

## Normative Anforderungen

1. NovaOS MUSS Mutexes für exklusiven Ressourcenzugriff bereitstellen.
2. Ein Mutex DARF zu einem Zeitpunkt höchstens einen Besitzer besitzen.
3. Nur der Besitzer DARF einen normalen Mutex freigeben.
4. Wartende Tasks SOLLEN bei Contention blockiert werden können.
5. Uncontended Acquisition SOLL einen effizienten Fast Path besitzen.
6. Lock MUSS mindestens Acquire-Semantik bereitstellen.
7. Unlock MUSS mindestens Release-Semantik bereitstellen.
8. Normale Mutexes SOLLEN standardmäßig nicht rekursiv sein.
9. NovaOS MUSS Priority Inversion für zeitkritische Workloads begrenzen können.
10. Hard-Realtime-Mutexes DÜRFEN keine unkontrolliert unbegrenzte Priority Inversion verursachen.
11. Cancellation eines wartenden Tasks DARF Mutex- und Wait-Queue-Zustand NICHT beschädigen.
12. Mutex-Besitz, Waiter und Contention SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-SYNC-0002`

## Ergebnis

```text
Task
 ↓
Acquire Mutex
 ↓
Exclusive Ownership
 ↓
Critical Section
 ↓
Release
 ↓
Wake Next Waiter
```

NovaOS erhält damit einen blockierenden Synchronisationsmechanismus für exklusiven Ressourcenzugriff, der atomare Fast Paths mit Scheduler-Integration, kontrolliertem Waiting und Realtime-fähiger Behandlung von Priority Inversion verbindet.