# NPSPEC-SYNC-FUTEX-0001 – Nova Futex

## Status

Entwurf

## Kategorie

Synchronization / Futex / Userspace Synchronization / Kernel Waiting

## Zweck

NovaOS definiert Futexes als effiziente Grundlage für Synchronisationsprimitive, bei denen der uncontended Fast Path vollständig ohne Kernelwechsel ausgeführt werden kann.

```text
Userspace Atomic State
        ↓
   Contention?
   ┌────┴────┐
  No        Yes
   ↓          ↓
Continue    Kernel Wait
```

Der Kernel wird nur benötigt, wenn ein Task tatsächlich blockiert oder geweckt werden muss.

## Grundprinzip

Futex bedeutet konzeptionell:

```text
Atomic Userspace State
        +
Kernel Wait / Wake
```

Dabei gilt:

```text
Futex ≠ Mutex
Futex ≠ Semaphore
Futex ≠ Condition Variable
```

Ein Futex ist ein niedrigstufiger Wait/Wake-Mechanismus, auf dem höherwertige Synchronisationsprimitive aufgebaut werden können.

## Fast Path

Der uncontended Fall soll ausschließlich über atomare Operationen erfolgen.

```text
Acquire
   ↓
Atomic State Change
   ↓
Success?
   ↓
Continue
```

Es erfolgt kein Syscall und kein Scheduler-Wechsel.

Beispiel:

```text
Unlocked → Locked
```

kann vollständig im Userspace erfolgen.

## Slow Path

Kann die Operation nicht unmittelbar abgeschlossen werden:

```text
Atomic Operation Failed
        ↓
Futex Wait
        ↓
Kernel
        ↓
Block Task
```

Der Kernel verwaltet nur das eigentliche Warten und Aufwecken.

## Futex Key

Der Kernel benötigt eine stabile Identifikation des Futex-Zustands.

Konzeptionell:

```text
FutexKey
├── Address Space / Shared Object
└── Offset / Address
```

Bei gemeinsamem Speicher muss dieselbe logische Futex-Position von mehreren Prozessen als identisches Synchronisationsobjekt erkannt werden können.

Eine virtuelle Adresse allein darf bei Shared Memory nicht grundsätzlich als globale Identität angenommen werden.

## Wait

Eine Futex-Wait-Operation folgt konzeptionell:

```text
FutexWait(Address, ExpectedValue)
```

Der Kernel muss den aktuellen Wert prüfen.

```text
CurrentValue == ExpectedValue?
       ├── No  → Return
       └── Yes → Block
```

Diese Prüfung verhindert Lost Wakeups zwischen Userspace-Prüfung und Kernel-Wait.

## Wake

Eine Futex-Wake-Operation macht wartende Tasks wieder ausführbar.

```text
FutexWake(Address, Count)
```

Beispiel:

```text
Wake(1)
→ one eligible waiter

Wake(N)
→ up to N eligible waiters
```

Wake verändert den eigentlichen Userspace-Zustand nicht automatisch.

Dieser bleibt Verantwortung des verwendeten Synchronisationsalgorithmus.

## Wait Queue

Der Kernel darf Waiter nach Futex Key organisieren.

```text
Futex Key
   ↓
Wait Queue
├── Task A
├── Task B
└── Task C
```

Die Implementierung soll skalierbare Hash-, Bucket- oder vergleichbare Strukturen verwenden können.

Globale zentrale Locks sollen vermieden werden.

## Lost Wakeups

Der kritische Übergang:

```text
Check State
    ↓
Register Waiter
    ↓
Block
```

muss gegen konkurrierendes Wake abgesichert sein.

Der Kernel muss deshalb den erwarteten Zustand während der Wait-Registrierung validieren.

```text
State Changed Before Wait
        ↓
Do Not Sleep
```

## Spurious Wakeups

Futex-Waits dürfen Spurious Wakeups zulassen.

Daher gilt:

```text
Wakeup ≠ Condition Satisfied
```

Der aufrufende Synchronisationsalgorithmus muss den Zustand nach dem Wakeup erneut prüfen.

## Timeout

Futex Wait darf optional einen Timeout oder eine Deadline besitzen.

```text
Wait
 ↓
Wake / Timeout / Cancellation
```

Die Ergebnisse müssen eindeutig unterscheidbar sein.

Zeitangaben sollen auf einer geeigneten monotonen Zeitbasis beruhen.

## Cancellation

Ein wartender Task darf kontrolliert aus einer Futex-Wait-Queue entfernt werden.

```text
Waiting
  ↓
Cancellation
  ↓
Remove Waiter
  ↓
Runnable / Cancelled
```

Dabei dürfen keine verlorenen Wakeups oder beschädigten Queue-Zustände entstehen.

## Shared Memory

Futexes dürfen prozessübergreifend verwendet werden, wenn der zugrunde liegende Zustand in gemeinsamem Speicher liegt.

```text
Process A ─┐
           ├→ Shared Memory → Futex
Process B ─┘
```

Der Kernel muss dabei die gemeinsame Speicheridentität berücksichtigen.

## Priority Inheritance

NovaOS darf spezielle Futex-Operationen für Priority-Inheritance-fähige Mutexes bereitstellen.

```text
High Priority Waiter
        ↓
Futex PI
        ↓
Temporary Priority Adjustment
        ↓
Lock Owner
```

Priority Inheritance ist keine Eigenschaft jedes Futexes, sondern eines darauf aufbauenden Synchronisationsprotokolls.

## Memory Ordering

Futex Wait/Wake ersetzt nicht die Memory-Ordering-Anforderungen des eigentlichen Synchronisationsprimitives.

```text
Atomic State
    +
Acquire / Release Semantics
    +
Futex Wait / Wake
```

Die korrekte Speicherordnung muss durch die atomaren Operationen des übergeordneten Synchronisationsalgorithmus definiert werden.

## Typische Verwendung

Futexes können Grundlage sein für:

```text
Mutex
Condition Variable
Semaphore
Read-Write Lock
Event
Once Initialization
Runtime Synchronization
```

Höhere APIs sollen Futex-Details vor normalen Anwendungen verbergen.

## Normative Anforderungen

1. NovaOS MUSS Futex-artiges kernelgestütztes Wait/Wake für atomaren Userspace-Zustand bereitstellen können.
2. Uncontended Synchronisation SOLL ohne Kernelwechsel möglich sein.
3. Der Kernel SOLL nur bei tatsächlichem Waiting oder Wakeup beteiligt werden.
4. `Wait` MUSS einen erwarteten Zustand validieren, bevor der Task blockiert wird.
5. Ein bereits veränderter Zustand DARF NICHT zu einem verlorenen Wakeup führen.
6. `Wake` DARF den Userspace-Synchronisationszustand NICHT implizit verändern.
7. Shared-Memory-Futexes MÜSSEN über eine gemeinsame logische Identität auflösbar sein.
8. Virtuelle Adressen DÜRFEN bei gemeinsamem Speicher NICHT grundsätzlich als globale Futex-Identität verwendet werden.
9. Spurious Wakeups DÜRFEN auftreten und MÜSSEN vom aufbauenden Algorithmus berücksichtigt werden.
10. Timeout und Cancellation SOLLEN unterstützt werden.
11. Futex-Wait-Queues SOLLEN für hohe Parallelität skalierbar implementiert werden.
12. Futex-Zustand, Waiter und Contention SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SYNC-ATOMIC-0001`
- `NPSPEC-SYNC-MUTEX-0001`
- `NPSPEC-SYNC-CONDITION-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-SYNC-0009`

## Ergebnis

```text
Atomic Userspace State
        ↓
Uncontended?
   ┌────┴────┐
  Yes        No
   ↓          ↓
Fast Path   Futex Wait
             ↓
           Kernel
             ↓
          Scheduler
             ↓
            Wake
```

NovaOS erhält damit eine effiziente Basis für höherwertige Synchronisationsprimitive, bei der der Normalfall im Userspace verbleibt und Kernel sowie Scheduler nur bei tatsächlicher Konkurrenz eingreifen.