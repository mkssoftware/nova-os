# NPSPEC-SCHEDULER-REALTIME-0001 – Nova Realtime Scheduling

## Status

Entwurf

## Kategorie

Scheduler / Realtime / Determinism / Resource Reservation

## Zweck

NovaOS definiert Realtime Scheduling für Tasks mit verbindlichen oder zeitkritischen Ausführungsanforderungen.

```text
Realtime Task
     ↓
Validate Requirements
     ↓
Reserve Resources
     ↓
Realtime Scheduler
     ↓
Bounded Execution
```

Ziel ist nicht maximale Geschwindigkeit, sondern vorhersehbares zeitliches Verhalten.

## Grundprinzip

Realtime bedeutet:

```text
Correct Result
+
Correct Time
=
Valid Result
```

NovaOS unterscheidet:

```text
Hard Realtime
Firm Realtime
Soft Realtime
```

Realtime-Priorität allein stellt keine zeitliche Garantie dar.

## Realtime-Klassen

### Hard Realtime

Eine Deadline MUSS eingehalten werden.

Ein nicht garantierbarer Hard-Realtime-Task darf nicht als garantiert angenommen werden.

### Firm Realtime

Ein Ergebnis nach der Deadline besitzt keinen oder nur noch geringen Nutzen.

### Soft Realtime

Deadline-Verletzungen sind zulässig, verschlechtern jedoch die Dienstqualität.

## Admission Control

Vor Annahme garantierter Realtime-Arbeit erfolgt eine Machbarkeitsprüfung.

```text
Execution Requirements
        +
Available Resources
        +
Existing Reservations
        ↓
Admission Control
        ↓
Accept / Reject
```

Dabei können berücksichtigt werden:

```text
CPU Budget
Execution Time
Period
Deadline
Interrupt Latency
I/O Latency
Memory
Device Access
Existing Realtime Load
```

## Ressourcenreservierung

Garantierte Realtime-Ausführung kann explizite Reservierungen benötigen.

```text
CPU
Memory
I/O
Interrupt Capacity
Bandwidth
Devices
```

Reservierte Ressourcen dürfen nicht durch normale Optimierungen unkontrolliert entzogen werden.

## Scheduling

Der Scheduler kann Realtime-Tasks anhand geeigneter Policies verwalten.

Beispiele:

```text
Fixed Priority
Earliest Deadline First
Time Partitioning
```

Die konkrete Policy darf austauschbar sein.

```text
Realtime Contract
      ↓
Select Valid Policy
      ↓
Schedule
```

## Preemption

Realtime-Tasks dürfen normale Tasks verdrängen, wenn dies zur Einhaltung garantierter zeitlicher Anforderungen notwendig ist.

```text
Normal Task
    ↓
Realtime Task Ready
    ↓
Preempt
    ↓
Realtime Execution
```

Preemption muss begrenzt und zeitlich analysierbar bleiben.

## Temporal Isolation

Realtime-Tasks müssen vor unkontrollierten zeitlichen Einflüssen anderer Workloads geschützt werden.

```text
Realtime Domain
      ↓
Reserved Resources
      ↓
Bounded Interference
```

Normale Tasks dürfen garantierte Realtime-Budgets nicht verbrauchen.

## Interrupts

Interrupt-Verarbeitung muss in Realtime-Analysen einbezogen werden.

Berücksichtigt werden müssen insbesondere:

```text
Interrupt Priority
Interrupt Affinity
Handler Duration
Deferred Work
Interrupt Coalescing
```

Coalescing oder Deferred Processing darf garantierte Latenzen nicht verletzen.

## Multiprocessing

Realtime-Tasks können feste oder eingeschränkte CPU-Affinitäten besitzen.

```text
Realtime Task
      ↓
Allowed CPU Set
      ↓
Reserved Execution Capacity
```

Unkontrollierte Migrationen sollen vermieden werden.

## Determinismus

Hard-Realtime-Ausführung soll möglichst deterministische Ausführungspfade verwenden.

```text
Bounded Runtime
+
Controlled Scheduling
+
Controlled I/O
+
Controlled Interrupts
```

Adaptive Optimierungen dürfen garantierte Worst-Case-Grenzen nicht ersetzen.

## Deadline Miss

Ein Deadline Miss muss explizit erkannt werden.

```text
Deadline Miss
     ↓
Realtime Policy
     ↓
Fail / Cancel / Degrade / Escalate
```

Bei Hard Realtime gilt ein Deadline Miss als Verletzung der garantierten Ausführungsanforderung.

## Normative Anforderungen

1. NovaOS MUSS Hard-, Firm- und Soft-Realtime unterscheiden.
2. Garantierte Hard-Realtime-Tasks MÜSSEN Admission Control durchlaufen.
3. Nicht garantierbare Hard-Realtime-Anforderungen DÜRFEN NICHT als garantiert akzeptiert werden.
4. Realtime-Ressourcen MÜSSEN reservierbar sein.
5. Garantierte Ressourcen DÜRFEN NICHT durch normale Scheduling-Optimierungen entzogen werden.
6. Realtime Scheduling MUSS präemptive Ausführung unterstützen können.
7. Temporal Isolation MUSS für garantierte Realtime-Ausführung unterstützt werden.
8. Interrupt- und I/O-Latenzen MÜSSEN bei Realtime-Garantien berücksichtigt werden.
9. Harte CPU-Affinitäten MÜSSEN eingehalten werden.
10. Adaptive Optimierungen DÜRFEN garantierte zeitliche Grenzen NICHT überschreiben.
11. Deadline Misses MÜSSEN erkannt und entsprechend ihrer Realtime-Klasse behandelt werden.
12. Realtime-Reservierungen und Scheduling-Entscheidungen SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SCHEDULER-0001`
- `NPSPEC-SCHEDULER-DEADLINE-0001`
- `NPSPEC-CONCURRENCY-DEADLINE-0001`
- `NPSPEC-INTERRUPT-PRIORITY-0001`
- `NPSPEC-INTERRUPT-AFFINITY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `ADR-SCHED-0005`
- `ADR-REALTIME-0001_Hard_Soft_und_Firm_Realtime_Profiles`
- `ADR-REALTIME-0002_Temporal_Isolation`
- `ADR-REALTIME-0003_Bounded_Latency`
- `ADR-REALTIME-0004_Deadline_Guarantees`
- `ADR-REALTIME-0005_Deterministic_Execution`

## Ergebnis

```text
Realtime Requirement
        ↓
Admission Control
        ↓
Resource Reservation
        ↓
Realtime Scheduling
        ↓
Bounded Execution
        ↓
Deadline Verification
```

NovaOS erhält damit ein Realtime-Scheduling-Modell, das zeitliche Garantien nicht nur über Prioritäten, sondern über Admission Control, Ressourcenreservierung, Temporal Isolation und kontrollierte Ausführung absichert.