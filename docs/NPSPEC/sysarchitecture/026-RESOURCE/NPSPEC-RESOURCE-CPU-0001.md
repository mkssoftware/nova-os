# NPSPEC-RESOURCE-CPU-0001 – Nova CPU Resource Model

## Status

Angenommen

## Kategorie

Resource / CPU / Compute

## Zweck

NovaOS definiert CPU-Rechenzeit als explizite, messbare und kontrollierbare Systemressource.

```text
Execution Demand
      ↓
CPU Requirement
      ↓
Budget + Policy
      ↓
Scheduler
      ↓
CPU Resource
```

CPU-Leistung wird nicht als unbegrenzt verfügbare Eigenschaft eines Prozesses betrachtet, sondern über Resource Economy, Accounting und Scheduling verwaltet.

## Grundprinzipien

```text
CPU Access ≠ Unlimited CPU Time
CPU Availability ≠ CPU Authority
Priority ≠ CPU Ownership
CPU Budget ≠ Guaranteed Completion
Core Identity ≠ Execution Identity
Affinity ≠ Exclusive Ownership
Reservation ≠ Consumption
CPU Time ≠ Wall Time
```

## CPU Resource

Eine logische CPU-Ressource wird beschrieben durch:

```text
CPUResource
├── ResourceID
├── ResourceTypeID
├── Topology
├── Capabilities
├── State
└── Capacity
```

Optional:

```text
Architecture
Core Type
NUMA Node
Frequency Range
Cache Topology
Energy Properties
Thermal State
Isolation State
Realtime Capability
```

## Topologie

NovaOS muss die physische und logische CPU-Topologie berücksichtigen können.

```text
System
├── NUMA Node
│   ├── Package
│   │   ├── Core
│   │   │   ├── Logical CPU
│   │   │   └── Logical CPU
```

Scheduling und Ressourcenplanung dürfen diese Struktur zur Optimierung verwenden.

## CPU Requirement

Workloads beschreiben benötigte CPU-Eigenschaften semantisch.

```text
CPURequirement
├── Compute Capacity
├── CPU Budget
└── Scheduling Class
```

Optional:

```text
Deadline
Affinity
Isolation
Determinism
Latency
Architecture Features
NUMA Preference
Energy Preference
```

Der Workload soll nach Möglichkeit keine konkrete CPU-ID voraussetzen müssen.

## CPU Budget

CPU-Verbrauch kann über Zeitbudgets begrenzt werden.

```text
CPU Budget
├── Execution Time
├── Time Window
├── Burst
└── Reservation
```

Beispiel:

```text
20 ms CPU Time
per
100 ms Window
```

Budgetüberschreitungen können zu:

```text
Throttle
Deprioritize
Degrade
Suspend
Controlled Failure
```

führen.

## Scheduling

Die CPU Resource wird über den Scheduler vergeben.

```text
Runnable Tasks
      ↓
Policy + Budget
      ↓
Scheduler
      ↓
CPU Allocation
```

Dabei können berücksichtigt werden:

```text
Fairness
Priority
Deadline
Realtime
Topology
NUMA
Cache Locality
Energy
Thermal State
Heterogeneous Cores
```

## Affinity

Workloads können CPU-Affinitäten besitzen.

```text
Task
 ↓
Preferred CPU Set
```

Affinity kann sein:

```text
Preferred
Required
Exclusive
```

Eine Preferred Affinity darf bei Bedarf durch den Scheduler verlassen werden.

Eine Required Affinity ist ein Hard Constraint.

## Heterogene CPUs

NovaOS muss unterschiedliche CPU-Klassen berücksichtigen können.

```text
Performance Core
Efficiency Core
Specialized Core
```

Die Auswahl kann anhand des ExecutionContract erfolgen.

```text
Latency Sensitive → Performance
Background Work   → Efficiency
```

Dies ist eine Policy und keine feste globale Regel.

## CPU Reservation

Kritische Workloads können CPU-Kapazität reservieren.

```text
ExecutionContract
      ↓
CPU Reservation
      ↓
Scheduler
```

Reservationen müssen in der Resource Economy berücksichtigt und dürfen nicht unbegrenzt überbucht werden.

## Realtime

Realtime-Workloads können strengere CPU-Anforderungen besitzen.

```text
Deadline
Worst-Case Budget
Latency Bound
Temporal Isolation
```

Hard-Realtime-Anforderungen haben Vorrang vor adaptiver Performance- oder Energieoptimierung, sofern die Systempolicy dies zulässt.

## CPU Accounting

CPU-Verbrauch muss mindestens nach:

```text
Process
Task
Application
Service
Agent
ExecutionContract
Accounting Domain
```

zuordenbar sein.

Mögliche Messgrößen:

```text
Execution Time
CPU Cycles
Scheduler Runtime
Core Usage
```

## Migration

Tasks können zwischen CPUs migriert werden.

```text
CPU A
 ↓
Task Migration
 ↓
CPU B
```

Migration darf jedoch definierte:

```text
Affinity
NUMA Constraints
Realtime Constraints
Determinism Requirements
Isolation Requirements
```

nicht verletzen.

## Capability Security

Direkter Zugriff auf privilegierte CPU-Funktionen benötigt entsprechende Capabilities.

Beispiele:

```text
CPU Configuration
Frequency Control
Topology Administration
Core Isolation
Performance Counters
Privileged Instructions
```

Normale CPU-Ausführung erzeugt keine administrative CPU-Autorität.

## Pressure

CPU-Überlastung muss explizit erkennbar sein.

```text
Normal
 ↓
Busy
 ↓
Contended
 ↓
Overloaded
```

Resource Economy und Scheduler können darauf mit Priorisierung, Throttling oder Graceful Degradation reagieren.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
CPU Topology
Available Capacity
Current Utilization
Reservations
Budgets
Task Placement
Affinity
Pressure State
Thermal State
Accounting
```

## Normative Anforderungen

1. NovaOS MUSS CPU-Rechenzeit als explizite Systemressource behandeln.
2. CPU-Verbrauch MUSS durch Resource Accounting messbar sein.
3. CPU Budgets MÜSSEN durch Resource Economy definierbar sein.
4. Scheduling MUSS CPU-Topologie berücksichtigen können.
5. NUMA-, Cache- und heterogene CPU-Eigenschaften SOLLEN für Placement-Entscheidungen nutzbar sein.
6. Required Affinity MUSS als Hard Constraint behandelbar sein.
7. CPU Reservationen MÜSSEN kontrolliert und abrechenbar sein.
8. Realtime-Anforderungen DÜRFEN nicht durch adaptive Optimierung verletzt werden.
9. Privilegierte CPU-Steuerung MUSS Capability-kontrolliert sein.
10. CPU-Ressourcen und deren Auslastung MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SCHED-TOPOLOGY-0001`
- `NPSPEC-SCHED-NUMA-0001`
- `NPSPEC-SCHED-CACHE-0001`
- `NPSPEC-SCHED-ENERGY-0001`
- `NPSPEC-SCHED-THERMAL-0001`
- `NPSPEC-SCHED-HETEROGENEOUS-0001`
- `NPSPEC-SCHED-REALTIME-0001`
- `ADR-ARCH-0033`

## Ergebnis

```text
CPU Capacity
     +
Execution Demand
     ↓
Budget + Policy + Topology
     ↓
Scheduler
     ↓
Controlled CPU Allocation
     ↓
Accounting + Feedback
```

NovaOS erhält damit ein einheitliches CPU-Ressourcenmodell, das Rechenleistung explizit budgetiert, topologiebewusst verteilt, präzise abrechnet und mit Realtime-, Energie-, NUMA- und Performance-Anforderungen verbindet.