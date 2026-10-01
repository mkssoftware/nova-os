# NPSPEC-REALTIME-SCHEDULING-0001 – Nova Realtime Scheduling

## Status

Angenommen

## Kategorie

Realtime / Scheduling / Temporal Guarantees

## Zweck

NovaOS definiert ein Realtime-Scheduling-Modell für Executions mit zeitlichen Anforderungen. Der Scheduler muss Realtime Profiles, Deadlines, Execution Budgets, Ressourcenreservierungen und temporale Isolation berücksichtigen.

```text
Realtime Execution
        ↓
Admission Control
        ↓
Realtime Scheduler
        ↓
Temporal Isolation
        ↓
Execution
        ↓
Deadline Verification
```

## Grundprinzipien

```text
Priority ≠ Realtime Guarantee
Deadline ≠ Priority
Fast Execution ≠ Realtime
Scheduled ≠ Guaranteed
CPU Guarantee ≠ End-to-End Guarantee
Prediction ≠ Reservation
```

Realtime Scheduling ergänzt den normalen NovaOS-Scheduler.

## Scheduling Model

```text
RealtimeSchedulingContext
├── ExecutionID
├── RealtimeProfile
├── Deadline
├── ExecutionBudget
├── Period
└── SchedulingState
```

Optional:

```text
LatencyBound
JitterBound
Priority
CPUAffinity
ResourceReservation
DeadlineID
ExecutionContractID
DeterminismMode
```

## Scheduling Classes

NovaOS muss mindestens unterscheiden:

```text
Hard Realtime
Firm Realtime
Soft Realtime
Normal
```

Die konkrete Scheduling-Strategie darf austauschbar sein, solange die zugesicherte Semantik erhalten bleibt.

## Admission Control

Firm- und Hard-Realtime-Executions müssen vor Aufnahme geprüft werden.

```text
Realtime Request
      ↓
Timing Requirements
      ↓
Available Capacity
      ↓
Interference Analysis
      ↓
Admission
```

Ergebnis:

```text
Accepted
Rejected
Degraded
```

`Accepted` darf nur verwendet werden, wenn die angeforderte Garantie tatsächlich bereitgestellt werden kann.

## Execution Budget

Realtime Executions können ein definiertes CPU-Budget besitzen.

```text
Period
├───────────────┤

Execution Budget
├──────┤
```

Der Scheduler muss Budgetverbrauch erfassen und begrenzen können.

## Deadline Scheduling

Scheduling-Entscheidungen können Deadlines berücksichtigen.

```text
Ready Tasks
    ↓
Deadline Evaluation
    ↓
Scheduling Decision
```

Geeignete Verfahren können beispielsweise deadline-, fixed-priority- oder policybasiert arbeiten.

Die konkrete Strategie ist nicht Bestandteil der Execution-Semantik.

## Temporal Isolation

Realtime Workloads müssen gegen unkontrollierte CPU-Interferenz geschützt werden können.

Mechanismen können sein:

```text
CPU Reservation
Core Affinity
Priority Isolation
Budget Enforcement
Time Partitioning
Controlled Preemption
```

## Preemption

Höher priorisierte Realtime-Arbeit muss niedrigere Arbeit kontrolliert verdrängen können.

```text
Normal Task
    ↓
Realtime Task Ready
    ↓
Preempt
    ↓
Realtime Execution
```

Kritische nicht präemptierbare Abschnitte müssen zeitlich begrenzt sein.

## Priority Inversion

Realtime Scheduling muss Priority Inversion berücksichtigen.

```text
High Priority
     ↓ waits for
Low Priority
```

Geeignete Mechanismen können sein:

```text
Priority Inheritance
Priority Ceiling
Bounded Critical Sections
```

Unbegrenzte Priority Inversion ist für garantierte Realtime-Pfade unzulässig.

## Multicore Scheduling

Auf Mehrkernsystemen können Realtime Executions:

```text
Pinned
Partitioned
Migratable
```

sein.

Migration darf nur erfolgen, wenn deren Kosten mit den zeitlichen Anforderungen vereinbar sind.

## Interrupts

Interrupt-Verarbeitung ist Teil der Realtime-Betrachtung.

Zu berücksichtigen sind:

```text
Interrupt Latency
Interrupt Priority
Deferred Work
Interrupt Affinity
Interrupt Load
```

Unkontrollierte Interrupt-Last darf Hard-Realtime-Executions nicht unbegrenzt verdrängen.

## Resource Integration

Eine CPU-Scheduling-Garantie allein genügt nicht.

```text
CPU
Memory
IO
Network
Interrupts
Devices
```

müssen bei End-to-End-Realtime-Anforderungen gemeinsam betrachtet werden.

## Adaptive Scheduler

Adaptive Scheduling darf Realtime Scheduling unterstützen:

```text
Prediction
Topology
Cache Locality
NUMA
Energy
Thermal
```

aber:

```text
Realtime Guarantee
        >
Adaptive Optimization
```

Hard-Realtime-Scheduling darf nicht von unsicheren Predictions abhängen.

## Energy und Thermal

Energieoptimierung darf Scheduling nur verändern, wenn zeitliche Garantien erhalten bleiben.

```text
Safety
  ↓
Hard Realtime
  ↓
Firm Realtime
  ↓
Soft Realtime
  ↓
Adaptive / Energy Optimization
```

Physische oder thermische Sicherheitsgrenzen besitzen weiterhin Vorrang.

## Deadline Miss

NovaOS muss Deadline Misses erkennen.

```text
Soft
→ Continue / Degrade

Firm
→ Cancel / Discard / Degrade

Hard
→ Contract Violation
```

Bei Hard Realtime muss der Verlust der Garantie explizit sichtbar werden.

## Determinismus

Deterministische Realtime-Ausführung kann verlangen:

```text
Fixed Scheduling Policy
Fixed CPU Assignment
Bounded Preemption
Bounded Blocking
Fixed Resource Reservations
Recorded Decisions
```

Adaptive Änderungen können für solche Executions eingeschränkt oder deaktiviert werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
Realtime Profile
Scheduling Class
Execution Budget
Budget Consumption
Period
Deadline
CPU Assignment
Preemptions
Blocking Time
Deadline Misses
Admission State
Guarantee State
```

## Normative Anforderungen

1. NovaOS MUSS Realtime Scheduling als Erweiterung des normalen Schedulers unterstützen.
2. Realtime Scheduling MUSS Realtime Profiles berücksichtigen.
3. Firm- und Hard-Realtime-Executions MÜSSEN Admission Control durchlaufen.
4. Eine Realtime-Garantie DARF nur nach erfolgreicher Admission zugesichert werden.
5. Execution Budgets MÜSSEN definierbar und kontrollierbar sein.
6. Hard-Realtime-Executions MÜSSEN temporal isolierbar sein.
7. Kritische nicht präemptierbare Bereiche MÜSSEN zeitlich begrenzbar sein.
8. Unbegrenzte Priority Inversion DARF in garantierten Realtime-Pfaden NICHT auftreten.
9. Priority-Inheritance- oder vergleichbare Mechanismen MÜSSEN unterstützt werden können.
10. Multicore-Realtime-Scheduling MUSS CPU-Affinity und kontrollierte Migration unterstützen können.
11. Interrupt-Latenz MUSS bei Realtime-Garantien berücksichtigt werden.
12. End-to-End-Garantien MÜSSEN relevante CPU-, Memory-, IO-, Network- und Device-Ressourcen berücksichtigen können.
13. Adaptive Optimierung DARF zugesicherte Realtime-Garantien NICHT verletzen.
14. Hard-Realtime-Scheduling DARF NICHT von unsicheren Predictions abhängig sein.
15. Deadline Misses MÜSSEN erkannt und entsprechend dem Realtime Profile behandelt werden.
16. Verlust einer Hard-Realtime-Garantie MUSS explizit sichtbar sein.
17. Deterministische Realtime-Ausführung MUSS adaptive Scheduling-Entscheidungen begrenzen können.
18. Realtime-Scheduling-Zustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-REALTIME-PROFILE-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-SCHEDULER-DEADLINE-0001`
- `NPSPEC-SCHEDULER-REALTIME-0001`
- `NPSPEC-SYNC-PRIORITYINHERITANCE-0001`
- `NPSPEC-INTERRUPT-PRIORITY-0001`
- `NPSPEC-INTERRUPT-AFFINITY-0001`
- `NPSPEC-ADAPTIVE-SCHEDULER-0001`
- `ADR-ARCH-0101`

## Ergebnis

```text
Realtime Contract
       ↓
Admission Control
       ↓
Budget + Reservation
       ↓
Realtime Scheduling
       ↓
Temporal Isolation
       ↓
Execution
       ↓
Deadline Verification
       ↓
Guarantee State
```

NovaOS erhält damit ein Realtime-Scheduling-Modell, das zeitliche Anforderungen nicht nur als Prioritäten behandelt, sondern durch Admission Control, Execution Budgets, Ressourcenreservierung, temporale Isolation und Deadline-Verifikation tatsächlich absicherbar macht.