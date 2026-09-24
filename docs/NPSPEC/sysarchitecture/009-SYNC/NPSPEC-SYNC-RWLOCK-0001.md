# NPSPEC-SYNC-RWLOCK-0001 – Nova Read-Write Lock

## Status

Entwurf

## Kategorie

Synchronization / Read-Write Lock / Concurrency / Shared State

## Zweck

NovaOS definiert Read-Write Locks zur Synchronisation gemeinsam genutzter Ressourcen, bei denen mehrere Tasks gleichzeitig lesen dürfen, Schreibzugriffe jedoch exklusiv erfolgen müssen.

```text
Reader A ─┐
Reader B ─┼→ RWLock → Shared Resource
Reader C ─┘

Writer ─────→ Exclusive Access
```

RWLocks sind insbesondere für Datenstrukturen geeignet, bei denen Lesezugriffe deutlich häufiger als Änderungen auftreten.

## Grundprinzip

Ein RWLock besitzt zwei Zugriffsarten:

```text
Read Lock
Write Lock
```

Für Read Locks gilt:

```text
Reader + Reader = Allowed
```

Für Write Locks gilt:

```text
Writer + Reader = Blocked
Writer + Writer = Blocked
```

Damit gilt:

```text
Multiple Readers
OR
One Writer
```

## Zustand

Ein RWLock verwaltet mindestens:

```text
ReaderCount
WriterState
WaitingReaders
WaitingWriters
```

Optional können Diagnoseinformationen geführt werden:

```text
WriterOwner
ContentionCount
ReadAcquireCount
WriteAcquireCount
```

## Read Lock

Ein Read Lock darf erworben werden, wenn kein inkompatibler Writer aktiv ist.

```text
ReadLock
   ↓
Writer Active?
 ┌────┴────┐
No         Yes
 ↓          ↓
Acquire    Wait
```

Mehrere Reader dürfen denselben Lock gleichzeitig halten.

## Write Lock

Ein Write Lock benötigt exklusiven Zugriff.

```text
WriteLock
    ↓
Readers == 0
AND
Writer == None
    ↓
Acquire
```

Sind Reader oder ein anderer Writer aktiv, muss der Task warten.

## Unlock

Read- und Write-Locks müssen entsprechend ihrer Zugriffsart freigegeben werden.

```text
ReadUnlock
   ↓
ReaderCount--

WriteUnlock
   ↓
WriterState = Free
```

Werden dadurch wartende Tasks ausführbar, müssen geeignete Waiter geweckt werden.

## Wait Queues

RWLocks dürfen getrennte Wait Queues für Reader und Writer verwenden.

```text
RWLock
├── Waiting Readers
└── Waiting Writers
```

Die Wakeup-Policy darf Fairness, Priorität, Deadline und Realtime-Anforderungen berücksichtigen.

## Reader / Writer Fairness

Eine reine Reader-Bevorzugung kann Writer Starvation verursachen.

```text
Reader
Reader
Reader
Reader
   ↓
Writer waits indefinitely
```

Eine reine Writer-Bevorzugung kann umgekehrt Reader unnötig verdrängen.

NovaOS soll deshalb eine kontrollierte Fairness-Policy unterstützen.

Mögliche Policies:

```text
Reader Preferred
Writer Preferred
Fair / Phase Fair
```

Die verwendete Policy muss explizit bestimmbar sein.

## Fast Path

Uncontended Zugriffe sollen einen effizienten atomaren Fast Path besitzen.

```text
Read / Write Request
        ↓
Atomic State Check
        ↓
Available?
 ┌──────┴──────┐
Yes            No
 ↓              ↓
Fast Path      Slow Path
```

## Slow Path

Bei Contention wird der Task blockiert.

```text
Acquire Failed
      ↓
Register Waiter
      ↓
Task Waiting
      ↓
Wakeup
      ↓
Acquire
```

Normale RWLocks sollen kein dauerhaftes Busy Waiting verwenden.

## Memory Ordering

Erfolgreiches Locking besitzt mindestens Acquire-Semantik.

Unlock besitzt mindestens Release-Semantik.

```text
ReadLock  → Acquire
WriteLock → Acquire

ReadUnlock  → Release
WriteUnlock → Release
```

Dadurch werden Änderungen eines Writers für nachfolgende Reader und Writer sichtbar.

## Upgrade

Die direkte Umwandlung:

```text
Read Lock → Write Lock
```

ist problematisch, da mehrere Reader gleichzeitig ein Upgrade verlangen könnten.

Standard-RWLocks sollen daher kein implizites atomisches Upgrade garantieren.

Falls Upgrade unterstützt wird, muss es eine explizite und deadlock-sichere Operation sein.

## Downgrade

Ein Writer darf optional kontrolliert zu einem Reader wechseln.

```text
Write Lock
    ↓
Downgrade
    ↓
Read Lock
```

Dabei darf zwischen Write- und Read-Besitz kein ungeschütztes Zugriffsfenster entstehen.

## Cancellation

Wartende Reader oder Writer dürfen kontrolliert aus ihrer Wait Queue entfernt werden.

```text
Waiting
  ↓
Cancellation
  ↓
Remove Waiter
```

Bereits erworbene Locks dürfen durch Cancellation nicht automatisch freigegeben werden.

## Realtime

Bei Realtime-Workloads muss insbesondere die maximale Blockierungsdauer berücksichtigt werden.

Ein lang laufender Reader darf einen zeitkritischen Writer nicht unkontrolliert blockieren.

```text
Realtime Writer
      ↓
Active Readers
      ↓
Bounded Blocking Required
```

Für harte Realtime-Anforderungen können spezialisierte Synchronisationsmechanismen erforderlich sein.

## Einsatzbereich

RWLocks eignen sich insbesondere für:

```text
Read-Mostly Metadata
System Tables
Registries
Configuration State
Shared Indexes
Lookup Structures
```

Bei sehr kurzen kritischen Bereichen oder hoher Schreibfrequenz kann ein normaler Mutex effizienter sein.

## Normative Anforderungen

1. NovaOS MUSS gleichzeitige Reader bei fehlendem Writer unterstützen.
2. Ein Writer MUSS exklusiven Zugriff erhalten.
3. Reader und Writer DÜRFEN dieselbe geschützte Ressource NICHT gleichzeitig besitzen.
4. Mehrere Writer DÜRFEN den Lock NICHT gleichzeitig besitzen.
5. Read- und Write-Acquire MÜSSEN mindestens Acquire-Semantik bereitstellen.
6. Read- und Write-Unlock MÜSSEN mindestens Release-Semantik bereitstellen.
7. Wartende Tasks SOLLEN bei Contention blockiert werden.
8. NovaOS MUSS Writer Starvation durch geeignete Policies begrenzen können.
9. Die Fairness-Policy SOLL explizit bestimmbar sein.
10. Read-to-Write-Upgrades DÜRFEN NICHT implizit als atomar angenommen werden.
11. Cancellation DARF Lock- oder Wait-Queue-Zustände NICHT beschädigen.
12. Reader-, Writer- und Contention-Zustände SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-SYNC-MUTEX-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-SYNC-0004`

## Ergebnis

```text
Shared Resource
      ↓
Read-Write Lock
   ↙         ↘
Readers     Writer
   ↓           ↓
Parallel    Exclusive
Reading     Modification
```

NovaOS erhält damit einen Synchronisationsmechanismus für read-heavy Ressourcen, der parallele Lesezugriffe erlaubt, Schreibzugriffe exklusiv schützt und Fairness, Blocking sowie Realtime-Anforderungen kontrolliert berücksichtigt.