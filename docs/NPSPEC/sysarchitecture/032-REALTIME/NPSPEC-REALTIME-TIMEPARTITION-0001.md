# NPSPEC-REALTIME-TIMEPARTITION-0001 – Nova Realtime Time Partitioning

## Status

Angenommen

## Kategorie

Realtime / Scheduling / Temporal Isolation / Time Partitioning

## Zweck

NovaOS definiert Time Partitioning zur zeitlichen Aufteilung gemeinsam genutzter Rechenressourcen in kontrollierte Ausführungsfenster.

```text
CPU Time
  ↓
Major Frame
  ↓
Time Partitions
  ↓
Controlled Execution
```

Dadurch können Realtime-Workloads zeitlich voneinander isoliert und CPU-Kapazitäten deterministisch bereitgestellt werden.

## Grundprinzipien

```text
Time Partition ≠ Process
Time Partition ≠ CPU Core
Time Partition ≠ Priority
Time Partition ≠ Capability
Reserved Time ≠ Guaranteed Completion
Temporal Isolation ≠ Spatial Isolation
```

Eine Time Partition definiert ein zeitliches Ausführungsrecht innerhalb eines festgelegten Scheduling-Zyklus.

## Partition Model

```text
TimePartition
├── PartitionID
├── ScheduleID
├── StartOffset
├── Duration
├── Period
├── AssignedExecutions
└── State
```

Optional:

```text
RealtimeProfile
CPUSet
ExecutionBudget
Deadline
JitterBound
OverrunPolicy
Criticality
ExecutionContractID
```

## Major Frame

Mehrere Time Partitions können einen wiederkehrenden Major Frame bilden.

```text
Major Frame
├── Partition A
├── Partition B
├── Partition C
└── Idle / System
```

Beispiel:

```text
0 ms        5 ms       10 ms       15 ms       20 ms
|---- A ----|---- B ----|---- A ----|---- C ----|
```

Nach Ende des Major Frames beginnt der Zeitplan erneut.

## Partition Window

Eine Partition darf nur innerhalb ihres zugewiesenen Fensters die reservierte CPU-Kapazität verwenden.

```text
Window Start
     ↓
Partition Active
     ↓
Execution
     ↓
Window End
```

Nach Ablauf des Fensters muss die reservierte Ausführung beendet, pausiert oder präemptiert werden können.

## CPU Assignment

Time Partitions können gelten für:

```text
Single CPU
CPU Core
CPU Set
Scheduling Domain
```

Auf Mehrkernsystemen können mehrere unabhängige Partition Schedules existieren.

## Static Schedule

Hard-Realtime-Systeme können einen statischen Zeitplan verwenden.

```text
Schedule Version
      ↓
Validate
      ↓
Admission Control
      ↓
Activate
```

Ein aktivierter Hard-Realtime-Zeitplan darf nicht unkontrolliert durch adaptive Scheduler verändert werden.

## Execution Assignment

Executions können einer Partition zugeordnet werden.

```text
Partition
├── Task A
├── Task B
└── Task C
```

Innerhalb einer Partition kann ein eigener Scheduling-Mechanismus verwendet werden.

Damit entsteht eine Hierarchie:

```text
Global Time Partitioning
          ↓
Partition Scheduler
          ↓
Tasks
```

## Temporal Isolation

Eine Partition darf die reservierte Zeit einer anderen Partition nicht unkontrolliert verbrauchen.

```text
Partition A Overrun
        X
Partition B Window
```

Partition Overruns müssen begrenzt werden.

## Overrun Policy

Wenn eine Execution ihr Zeitfenster überschreitet:

```text
Window End
    ↓
Overrun
```

Mögliche Reaktionen:

```text
Preempt
Suspend
Cancel
Degrade
Record Violation
Fail-safe
```

Die Reaktion richtet sich nach Realtime Profile und Execution Contract.

## Slack Time

Nicht verwendete Zeit kann optional als Slack verfügbar gemacht werden.

```text
Unused Partition Time
        ↓
Slack
```

Slack kann für Best-Effort-Workloads genutzt werden, sofern dadurch keine zukünftige Realtime-Garantie beeinträchtigt wird.

```text
Slack ≠ Reservation
```

Realtime-Executions dürfen nicht von der Verfügbarkeit von Slack abhängig sein.

## Context Switch

Partitionwechsel erzeugen Kosten.

```text
Partition A
    ↓
Switch
    ↓
Partition B
```

Zu berücksichtigen sind:

```text
Scheduler Overhead
TLB Effects
Cache Effects
Memory Mapping
Device State
Security Context
```

Diese Kosten müssen in zeitlichen Garantien berücksichtigt werden.

## Interrupts

Interrupt-Verarbeitung muss mit Time Partitioning vereinbar sein.

Mögliche Mechanismen:

```text
Interrupt Affinity
Interrupt Budget
Deferred Processing
Partition-specific Handling
```

Unbegrenzte Interrupt-Verarbeitung darf Partition Guarantees nicht zerstören.

## I/O

I/O kann über Partitionsgrenzen hinaus weiterlaufen.

```text
Partition A
   ↓
Submit I/O
   ↓
Partition Switch
   ↓
I/O Completion
```

NovaOS muss definieren, wie Completion, Budget und Deadline in diesem Fall behandelt werden.

## Admission Control

Vor Aktivierung eines Zeitplans muss geprüft werden:

```text
Partition Durations
        +
Switch Overhead
        +
System Overhead
        +
Required Capacity
        ↓
Admission Control
```

Es muss gelten:

```text
Reserved Time + Required Overhead <= Available Time
```

## Dynamic Changes

Änderungen eines aktiven Realtime-Zeitplans müssen kontrolliert erfolgen.

```text
Current Schedule
       ↓
Candidate Schedule
       ↓
Validate
       ↓
Safe Activation Point
       ↓
New Schedule
```

Hard-Realtime-Zeitpläne dürfen nicht mitten in einem kritischen Zyklus unkontrolliert verändert werden.

## Adaptive Systeme

Adaptive Systeme dürfen:

```text
Slack nutzen
Best-Effort Tasks platzieren
Energie optimieren
Nichtkritische Partitionen optimieren
```

Sie dürfen jedoch garantierte:

```text
Start Times
Durations
Budgets
Isolation Boundaries
Deadlines
```

nicht verletzen.

## Determinismus

Time Partitioning kann deterministische Realtime-Ausführung unterstützen.

```text
Fixed Schedule
      +
Fixed Resources
      +
Bounded Interference
      ↓
Predictable Execution
```

Ein festes Partition Schedule kann Bestandteil eines deterministischen Execution Contracts sein.

## Distributed Systeme

Time Partitioning ist grundsätzlich lokal.

Verteilte Systeme können koordinierte Zeitfenster verwenden, dürfen jedoch keine perfekte globale Uhr voraussetzen.

Clock Synchronization und Drift müssen explizit berücksichtigt werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
PartitionID
ScheduleID
Schedule Version
Start Offset
Duration
Period
Assigned CPU
Assigned Executions
Execution Budget
Consumed Time
Overrun Count
Current Partition
Guarantee State
```

## Normative Anforderungen

1. NovaOS MUSS CPU-Zeit in definierte Time Partitions aufteilen können.
2. Time Partitions MÜSSEN stabile PartitionIDs besitzen.
3. Wiederkehrende Partition Schedules MÜSSEN unterstützt werden können.
4. Partition Start, Dauer und Periodizität MÜSSEN explizit definierbar sein.
5. Executions MÜSSEN Time Partitions zugeordnet werden können.
6. Innerhalb einer Partition MUSS ein eigener Scheduler verwendbar sein.
7. Eine Partition DARF reservierte Zeit einer anderen Partition NICHT unkontrolliert verbrauchen.
8. Partition Overruns MÜSSEN erkannt und begrenzt werden.
9. Overrun Policies MÜSSEN durch Realtime Profile und Execution Contract bestimmbar sein.
10. Slack Time DARF für Best-Effort-Arbeit verwendet werden, sofern keine Garantie verletzt wird.
11. Garantierte Realtime-Executions DÜRFEN NICHT von Slack Time abhängen.
12. Partition-Switch-Overhead MUSS in zeitlichen Garantien berücksichtigt werden.
13. Interrupt-Interferenz MUSS begrenzt werden können.
14. Asynchrones I/O MUSS über Partitionsgrenzen hinweg korrekt behandelt werden.
15. Garantierte Partition Schedules MÜSSEN Admission Control durchlaufen.
16. Änderungen aktiver Hard-Realtime-Schedules MÜSSEN kontrolliert und atomar aktivierbar sein.
17. Adaptive Optimierung DARF garantierte Partition Boundaries NICHT verändern.
18. Partition Schedule und Laufzeitzustand MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-REALTIME-PROFILE-0001`
- `NPSPEC-REALTIME-SCHEDULING-0001`
- `NPSPEC-REALTIME-LATENCY-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-REALTIME-TEMPORALISOLATION-0001`
- `NPSPEC-REALTIME-DETERMINISTIC-0001`
- `NPSPEC-REALTIME-IO-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-SCHEDULER-DEADLINE-0001`
- `NPSPEC-INTERRUPT-PRIORITY-0001`
- `ADR-ARCH-0108`

## Ergebnis

```text
Realtime Requirements
        ↓
Partition Schedule
        ↓
Admission Control
        ↓
Major Frame
        ↓
Time Windows
        ↓
Temporally Isolated Execution
        ↓
Overrun + Deadline Verification
```

NovaOS erhält damit ein kontrolliertes Time-Partitioning-Modell, das CPU-Zeit explizit zwischen Realtime-Domains aufteilen, zeitliche Interferenz begrenzen und deterministische Ausführungsfenster für kritische Workloads bereitstellen kann.