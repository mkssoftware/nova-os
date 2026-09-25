# NPSPEC-SYNC-PRIORITYINHERITANCE-0001 – Nova Priority Inheritance

## Status

Angenommen

## Kategorie

Synchronization / Priority Inheritance / Realtime / Scheduling

## Zweck

NovaOS definiert Priority Inheritance zur Begrenzung von Priority Inversion bei blockierenden Synchronisationsmechanismen.

```text
High Priority Task
       ↓ waits
     Mutex
       ↓ owned by
Low Priority Task
```

Ohne Gegenmaßnahme kann ein Task mittlerer Priorität den Lock-Besitzer verdrängen und dadurch indirekt den höher priorisierten Task blockieren.

Priority Inheritance erlaubt dem Lock-Besitzer deshalb, vorübergehend eine höhere effektive Priorität zu erhalten.

## Grundprinzip

Beispiel:

```text
Task H = High Priority
Task M = Medium Priority
Task L = Low Priority

L owns Mutex
H waits for Mutex
M becomes runnable
```

Ohne Priority Inheritance:

```text
M preempts L
     ↓
L cannot release Mutex
     ↓
H remains blocked
```

Mit Priority Inheritance:

```text
H waits on Mutex owned by L
        ↓
L inherits Priority(H)
        ↓
L executes
        ↓
L releases Mutex
        ↓
H continues
```

Dabei gilt:

```text
Inherited Priority ≠ Permanent Priority
```

Die ursprüngliche Priorität des Tasks bleibt erhalten.

## Basis- und effektive Priorität

Ein Task besitzt mindestens:

```text
BasePriority
EffectivePriority
```

Normalfall:

```text
EffectivePriority = BasePriority
```

Bei Priority Inheritance:

```text
EffectivePriority =
max(BasePriority, InheritedPriorities)
```

Nach Wegfall aller relevanten Abhängigkeiten muss die effektive Priorität neu berechnet werden.

## Vererbung

Wartet ein höher priorisierter Task auf einen Lock, muss dessen Besitzer die erforderliche Priorität erben können.

```text
High Task
   ↓ waits
Mutex
   ↓ owned
Low Task
   ↓
Priority Boost
```

Die Vererbung gilt nur solange die entsprechende Blockierungsabhängigkeit besteht.

## Transitive Vererbung

Priority Inheritance muss über mehrere Lock-Abhängigkeiten propagiert werden können.

```text
Task H
  ↓ waits
Mutex A
  ↓ owned by
Task L
  ↓ waits
Mutex B
  ↓ owned by
Task X
```

Dann kann gelten:

```text
Priority(H)
    ↓
Task L
    ↓
Task X
```

Die Vererbung muss entlang der relevanten Wait-For-Kette propagiert werden.

## Mehrere Waiter

Besitzt ein Task einen Lock mit mehreren Waitern:

```text
Mutex
├── Task A Priority 20
├── Task B Priority 50
└── Task C Priority 80
```

muss die effektive Vererbung mindestens die höchste relevante Prioritätsanforderung berücksichtigen.

```text
InheritedPriority = 80
```

## Mehrere Locks

Ein Task darf mehrere Locks gleichzeitig besitzen.

```text
Task
├── Mutex A → Waiter Priority 70
└── Mutex B → Waiter Priority 40
```

Die effektive Priorität muss alle aktiven Vererbungsquellen berücksichtigen.

Wird Mutex A freigegeben, darf die Priorität nicht automatisch auf die Basispriorität zurückfallen, solange Mutex B weiterhin eine Vererbung verursacht.

## Priority Restoration

Beim Entfernen einer Abhängigkeit muss die effektive Priorität neu bestimmt werden.

```text
Release Lock
    ↓
Remove Inheritance Source
    ↓
Recalculate Effective Priority
```

Die Wiederherstellung darf keine noch aktive Vererbung verlieren.

## Scheduler-Integration

Der Scheduler muss Änderungen der effektiven Priorität unmittelbar berücksichtigen können.

```text
Priority Inheritance
        ↓
Effective Priority Change
        ↓
Scheduler Update
        ↓
Run Queue Reordering
```

Ein Priority Boost ohne entsprechende Scheduler-Reaktion wäre wirkungslos.

## Wait Queue

Wait Queues von PI-fähigen Synchronisationsobjekten sollen Prioritätsinformationen berücksichtigen.

```text
Waiters
├── High
├── Medium
└── Low
```

Die konkrete Wakeup-Policy muss mit Scheduler- und Realtime-Regeln konsistent sein.

## Cancellation

Wird ein wartender Task abgebrochen:

```text
High Task waits
      ↓
Cancellation
      ↓
Remove Waiter
      ↓
Recalculate Owner Priority
```

Eine dadurch nicht mehr benötigte Priority Inheritance muss entfernt werden.

## Timeout

Dasselbe gilt für Timeouts.

```text
Wait Timeout
     ↓
Remove Dependency
     ↓
Recalculate Inheritance
```

Veraltete Priority Boosts dürfen nicht bestehen bleiben.

## Deadlocks

Priority Inheritance verhindert keine Deadlocks.

```text
Priority Inheritance ≠ Deadlock Prevention
```

Eine zyklische Lock-Abhängigkeit bleibt auch mit Priority Inheritance ein Deadlock.

Die Mechanismen aus `NPSPEC-SYNC-DEADLOCK-0001` bleiben erforderlich.

## Semaphore und Condition Variable

Priority Inheritance ist besonders für Synchronisationsobjekte mit eindeutigem Besitzer geeignet.

```text
Mutex → Owner known → PI suitable
```

Bei Semaphoren oder anderen Mechanismen ohne eindeutigen Besitzer ist klassische Priority Inheritance nicht unmittelbar anwendbar.

## Realtime

Priority Inheritance ist insbesondere für Realtime-Systeme relevant.

```text
Realtime Task
     ↓
Blocked by Lower Priority Task
     ↓
Priority Inheritance
     ↓
Bounded Priority Inversion
```

Priority Inheritance allein garantiert jedoch keine Deadline.

Zusätzlich müssen kritische Bereiche, Scheduling-Latenzen und Ressourcenabhängigkeiten begrenzt sein.

## Diagnose

NovaOS soll folgende Informationen introspektierbar machen:

```text
Base Priority
Effective Priority
Inherited Priority
Inheritance Source
Owned Locks
Blocking Waiters
Inheritance Chain
```

Damit muss nachvollziehbar sein, warum ein Task aktuell mit erhöhter Priorität ausgeführt wird.

## Normative Anforderungen

1. NovaOS MUSS Priority Inheritance für geeignete Synchronisationsprimitive unterstützen können.
2. Basispriorität und effektive Priorität eines Tasks MÜSSEN getrennt verwaltet werden.
3. Ein Lock-Besitzer MUSS die Priorität eines höher priorisierten blockierten Waiters erben können.
4. Priority Inheritance MUSS über relevante verschachtelte Lock-Abhängigkeiten propagiert werden können.
5. Mehrere gleichzeitig aktive Vererbungsquellen MÜSSEN korrekt berücksichtigt werden.
6. Beim Wegfall einer Vererbungsquelle MUSS die effektive Priorität neu berechnet werden.
7. Aktive verbleibende Vererbungen DÜRFEN bei Priority Restoration NICHT verloren gehen.
8. Scheduler-Run-Queues MÜSSEN Änderungen der effektiven Priorität berücksichtigen.
9. Cancellation und Timeout MÜSSEN veraltete Vererbungsabhängigkeiten entfernen.
10. Priority Inheritance DARF NICHT als Deadlock-Vermeidung behandelt werden.
11. Priority Inheritance SOLL nur für Synchronisationsmechanismen mit eindeutig bestimmbarer Blockierungsabhängigkeit verwendet werden.
12. Priority-Inheritance-Ketten SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-SYNC-MUTEX-0001`
- `NPSPEC-SYNC-FUTEX-0001`
- `NPSPEC-SYNC-DEADLOCK-0001`
- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-CANCELLATION-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `ADR-SYNC-0011`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`

## Ergebnis

```text
High Priority Task
       ↓ waits
     Mutex
       ↓
Low Priority Owner
       ↓
Inherit Priority
       ↓
Scheduler Boost
       ↓
Finish Critical Section
       ↓
Release Mutex
       ↓
Restore Effective Priority
       ↓
High Priority Task Continues
```

NovaOS erhält damit einen schedulerintegrierten Mechanismus zur Begrenzung von Priority Inversion, der verschachtelte Abhängigkeiten, mehrere Waiter, Cancellation und Realtime-Anforderungen kontrolliert berücksichtigt.