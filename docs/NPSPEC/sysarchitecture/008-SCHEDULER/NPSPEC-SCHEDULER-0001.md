# NPSPEC-SCHEDULER-0001 – Nova Scheduler Architecture

## Status

Entwurf

## Kategorie

Scheduler / Kernel / Execution / Resource Economy

## Zweck

NovaOS definiert eine einheitliche Scheduler-Architektur zur Auswahl ausführbarer Tasks und zur kontrollierten Verteilung von CPU-Zeit.

```text
Runnable Tasks
      ↓
Scheduler
      ↓
Scheduling Decision
      ↓
CPU
```

Der Scheduler berücksichtigt dabei Prioritäten, Deadlines, Affinitäten, Ressourcenanforderungen und Execution Contracts.

## Grundprinzip

Scheduling bestimmt:

```text
Welche Task?
Wann?
Auf welcher CPU?
Für wie lange?
```

Mechanismus und Policy bleiben getrennt.

```text
Scheduler Mechanism
        +
Scheduling Policy
        ↓
Execution Decision
```

## Scheduling-Objekt

Primäre Scheduling-Einheit ist der ausführbare Task.

Der Scheduler verwendet mindestens:

```text
TaskID
State
Priority
Affinity
Execution Contract
Resource Requirements
Scheduling Metadata
```

Optionale Informationen können sein:

```text
Deadline
Runtime Estimate
Realtime Class
NUMA Preference
Energy Preference
```

## Task-Zustände

Für Scheduling sind insbesondere relevant:

```text
Ready
Running
Waiting
Suspended
Completed
```

Nur ausführbare Tasks dürfen CPU-Zeit erhalten.

```text
Ready → Running
Running → Ready
Running → Waiting
```

## Run Queues

NovaOS darf lokale Run Queues pro CPU oder CPU-Gruppe verwenden.

```text
CPU 0 → Run Queue 0
CPU 1 → Run Queue 1
CPU 2 → Run Queue 2
```

Dies reduziert globale Synchronisation und verbessert Lokalität.

Work Stealing darf zur Lastverteilung eingesetzt werden.

## Scheduling-Entscheidung

Eine Entscheidung kann berücksichtigen:

```text
Priority
Deadline
CPU Affinity
NUMA Locality
Execution Contract
Realtime Requirements
Resource Availability
Previous Runtime
System Load
```

Harte Constraints müssen vor weichen Optimierungen geprüft werden.

```text
Hard Constraints
      ↓
Eligible Tasks
      ↓
Soft Optimization
      ↓
Selected Task
```

## Preemption

NovaOS muss präemptives Scheduling unterstützen können.

```text
Running Task
     ↓
Preemption
     ↓
Ready Queue
     ↓
Higher Priority / Eligible Task
```

Preemption darf nur an architektonisch sicheren Übergängen erfolgen.

## Multiprocessing

Auf SMP-Systemen verteilt der Scheduler Tasks auf mehrere CPUs.

```text
Tasks
  ↓
Scheduler
  ↓
CPU 0
CPU 1
CPU 2
CPU 3
```

Dabei sollen CPU-Affinität, Cache-Lokalität und NUMA-Topologie berücksichtigt werden.

## Priorität

Scheduling-Priorität und Interrupt-Priorität sind getrennte Konzepte.

```text
Task Priority ≠ Interrupt Priority
```

Prioritätsänderungen müssen kontrolliert erfolgen.

Mechanismen gegen unkontrollierte Starvation müssen möglich sein.

## Deadlines

Tasks mit Deadlines können bevorzugt geplant werden.

```text
Task
  ↓
Deadline
  ↓
Scheduler
```

Eine Deadline allein stellt keine garantierte Realtime-Ausführung dar.

Hard-Realtime-Garantien benötigen zusätzlich reservierte Ressourcen und begrenzte Ausführungslatenzen.

## Execution Contracts

Der Scheduler muss relevante Anforderungen aus `Nova.ExecutionContract` berücksichtigen.

Beispiele:

```text
Deadline
Resource Budget
Determinism
Execution Location
Latency
Required Provider
```

Hard Requirements dürfen nicht durch Scheduling-Optimierungen verletzt werden.

## Determinismus

Für deterministische Ausführung muss der Scheduler einen kontrollierten Scheduling-Modus unterstützen können.

```text
Deterministic Contract
        ↓
Controlled Scheduling
        ↓
Reproducible Ordering
```

Adaptive Entscheidungen müssen in diesem Modus eingeschränkt oder reproduzierbar sein.

## Resource Economy

Der Scheduler arbeitet mit der systemweiten Ressourcenökonomie zusammen.

```text
Scheduler
    ↕
Resource Economy
```

CPU-Zeit darf nicht unabhängig von Ressourcenbudgets und Reservierungen betrachtet werden.

## Adaptive Optimierung

NovaOS darf Scheduling anhand beobachteter Systembedingungen optimieren.

Beispiele:

```text
CPU Load
Task Behaviour
Cache Locality
NUMA Distance
Energy State
Deadline Pressure
```

Adaptive Optimierung darf harte Anforderungen oder explizite Nutzerentscheidungen nicht überschreiben.

## Normative Anforderungen

1. NovaOS MUSS präemptives Task-Scheduling unterstützen.
2. Tasks MÜSSEN anhand ihres ausführbaren Zustands geplant werden.
3. Mechanismus und Scheduling-Policy MÜSSEN logisch getrennt bleiben.
4. Hard Constraints MÜSSEN vor Soft Preferences ausgewertet werden.
5. CPU-Affinitäten MÜSSEN eingehalten werden.
6. NUMA- und Cache-Lokalität SOLLEN berücksichtigt werden.
7. Deadlines MÜSSEN bei entsprechenden Tasks berücksichtigt werden.
8. Execution Contracts MÜSSEN in Scheduling-Entscheidungen einfließen.
9. Hard-Realtime-Anforderungen DÜRFEN NICHT durch normale Optimierungen verletzt werden.
10. Deterministic Mode MUSS kontrolliertes Scheduling ermöglichen.
11. Work Stealing DARF nur für geeignete Tasks verwendet werden.
12. Scheduling-Entscheidungen SOLLEN introspektierbar und diagnostizierbar sein.

## Abhängigkeiten

- `NPSPEC-CONCURRENCY-TASK-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-CONCURRENCY-WORKSTEALING-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-HAL-TOPOLOGY-0001`
- `NPSPEC-HAL-NUMA-0001`
- `ADR-SCHED-0001`
- `ADR-SCHED-0002`

## Ergebnis

```text
Runnable Tasks
      ↓
Hard Constraint Filter
      ↓
Scheduling Policy
      ↓
CPU / Locality Selection
      ↓
Execution
```

NovaOS erhält damit eine skalierbare Scheduler-Architektur, die klassische CPU-Planung mit Structured Concurrency, Execution Contracts, Realtime-Anforderungen, Ressourcenökonomie und Hardwarelokalität verbindet.