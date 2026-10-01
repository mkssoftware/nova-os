# NPSPEC-DISTRIBUTED-SCHEDULER-0001 – Nova Distributed Scheduler

## Status

Angenommen

## Kategorie

Distributed / Scheduler / Execution Placement

## Zweck

NovaOS definiert einen Distributed Scheduler für die koordinierte Platzierung und Ausführung von Tasks über lokale und entfernte Rechenressourcen.

Der Scheduler entscheidet nicht über die Semantik einer Operation, sondern darüber, **wo und wann** bereits geplante Arbeit unter Einhaltung des Execution Contracts ausgeführt wird.

```text
Execution Plan
      ↓
Distributed Scheduler
      ↓
Eligible Nodes
      ↓
Task Placement
      ↓
Local / Remote Execution
```

## Grundprinzipien

```text
Scheduling ≠ Authorization
Scheduling ≠ Admission
Scheduling ≠ Provider Discovery
Scheduling ≠ Algorithm Selection
Scheduling ≠ Resource Ownership

Node Availability ≠ Node Eligibility
Lowest Load ≠ Best Placement
Remote Capacity ≠ Permission
Priority ≠ Authority
```

## Scheduler Model

Der Distributed Scheduler arbeitet mit:

```text
DistributedSchedule
├── ExecutionID
├── Task Graph
├── Eligible Nodes
├── Resource Requirements
├── Dependencies
└── Scheduling State
```

Optional:

```text
Deadlines
Latency Requirements
Priority
Affinity
Anti-Affinity
Data Locality
Trust
Sovereignty
Energy
Thermal State
Migration Policy
Fallback Policy
```

## Scheduling Hierarchy

NovaOS verwendet hierarchische Planung.

```text
Distributed Scheduler
      ↓
Node Selection
      ↓
Local Scheduler
      ↓
CPU / GPU / NPU / Device
```

Der Distributed Scheduler entscheidet primär über die Platzierung.

Der lokale Scheduler kontrolliert die konkrete Ausführung innerhalb des Zielsystems.

## Node Eligibility

Vor der eigentlichen Optimierung werden unzulässige Nodes ausgeschlossen.

```text
Candidate Nodes
      ↓
Capabilities
      ↓
Resources
      ↓
Security
      ↓
Trust
      ↓
Sovereignty
      ↓
Deadline Feasibility
      ↓
Eligible Nodes
```

Nur danach darf optimiert werden.

## Placement

Die Platzierung kann berücksichtigen:

```text
CPU Capacity
Memory
GPU / NPU
Current Load
Data Locality
Network Latency
Deadline
Energy
Thermal State
Provider Availability
```

Hard Constraints besitzen Vorrang.

## Data Locality

Der Scheduler arbeitet mit Distributed Storage zusammen.

```text
Task
 +
Required Objects
      ↓
Data Placement
      ↓
Scheduling Decision
```

NovaOS kann entscheiden:

```text
Move Compute to Data
```

oder:

```text
Move Data to Compute
```

Dabei müssen Kosten und Constraints berücksichtigt werden.

## Affinity

Tasks können bevorzugte Beziehungen besitzen.

```text
Task A
   ↓ affinity
Node A
```

Beispiele:

```text
Same Node
Same NUMA Domain
Same Accelerator
Near Data
Near Dependent Task
```

## Anti-Affinity

Tasks können bewusst getrennt werden.

```text
Task A → Node A
Task B → Node B
```

Dies kann für:

```text
Fault Isolation
Availability
Security Isolation
Resource Contention
```

verwendet werden.

## Resource Admission

Scheduling erfolgt erst nach oder gemeinsam mit der Prüfung verfügbarer Ressourcen.

```text
Placement Candidate
      ↓
Resource Admission
      ↓
Accepted
      ↓
Schedule
```

Ein Scheduler darf Ressourcen nicht lediglich aufgrund erwarteter Verfügbarkeit voraussetzen.

## Deadline Scheduling

Deadlines müssen Netzwerk- und Koordinationskosten berücksichtigen.

```text
Deadline Budget
├── Queue
├── Network
├── Execution
├── Synchronization
└── Result Transfer
```

Ist eine Deadline nicht realistisch erfüllbar, muss dies vor oder während der Ausführung erkannt werden.

## Priority

Priorität beeinflusst Scheduling, überschreibt aber keine:

```text
Security Constraints
Trust Requirements
Sovereignty Constraints
Hard Resource Limits
Capabilities
```

```text
Priority ≠ Permission
```

## Load Balancing

NovaOS kann Tasks verteilen, um Überlastung zu vermeiden.

```text
Node A → High Load
Node B → Medium Load
Node C → Low Load
```

Load ist jedoch nur ein Faktor der Scheduling-Entscheidung.

## Work Stealing

Geeignete Tasks können zwischen Nodes verschoben werden.

```text
Idle Node
   ↓
Eligible Work
   ↓
Steal / Transfer
```

Dies ist nur zulässig, wenn:

```text
Task is Migratable
Capabilities permit it
Trust permits it
Sovereignty permits it
Resources permit it
```

## Migration

Laufende oder pausierte Tasks können migriert werden, sofern ihr Execution Model dies unterstützt.

```text
Node A
  ↓
Checkpoint / State Transfer
  ↓
Node B
  ↓
Resume
```

Vor der Migration müssen alle Hard Constraints erneut geprüft werden.

## Structured Concurrency

Distributed Scheduling erhält die Task-Hierarchie.

```text
Execution
├── Task Group A
│   ├── Node A
│   └── Node B
└── Task Group B
    └── Node C
```

Cancellation und Lifetime bleiben an die übergeordnete Execution gebunden.

## Failure Handling

Der Scheduler muss mit folgenden Zuständen umgehen können:

```text
Node Failure
Network Failure
Provider Failure
Resource Loss
Trust Revocation
Capability Revocation
Deadline Risk
```

Mögliche Reaktionen:

```text
Reschedule
Migrate
Retry
Use Fallback
Degrade
Cancel
Fail
```

## Replanning

Bei veränderten Bedingungen kann der Schedule neu berechnet werden.

```text
Current Schedule
      ↓
Environment Change
      ↓
Constraint Revalidation
      ↓
New Schedule
```

Hard Constraints dürfen dabei nicht abgeschwächt werden.

## Determinismus

Bei:

```text
Determinism = Required
```

kann der Scheduling-Freiheitsgrad eingeschränkt werden.

Beispielsweise:

```text
Fixed Task Placement
Defined Execution Order
Controlled Synchronization
Pinned Provider Versions
```

Adaptive Lastverteilung darf Required Determinism nicht verletzen.

## Energy und Thermal

Der Scheduler kann energie- und temperaturbewusst planen.

```text
Node A → Hot
Node B → Efficient
Node C → Battery Constrained
```

Mögliche Entscheidungen:

```text
Move Work
Reduce Parallelism
Prefer Efficient Node
Delay Optional Work
```

Safety und Hard Constraints besitzen Vorrang.

## Fairness

Mehrere verteilte Executions müssen kontrolliert um Ressourcen konkurrieren.

Der Scheduler integriert deshalb:

```text
Resource Arbitration
Priority
Reservations
Guarantees
Deadlines
Fairness
```

Ein einzelner Workload darf das verteilte System nicht unbegrenzt dominieren.

## Scheduler State

Tasks können Zustände besitzen:

```text
Pending
Eligible
Admitted
Scheduled
Running
Blocked
Migrating
Completed
Cancelled
Failed
Unknown
```

Der Zustand muss über Node-Grenzen hinweg nachvollziehbar bleiben.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ExecutionID
TaskID
Current Node
Eligible Nodes
Placement Reason
Resource State
Deadline State
Affinity
Data Locality
Migration State
Load
Trust State
Sovereignty State
Scheduling History
```

## Normative Anforderungen

1. NovaOS MUSS verteilte Task-Platzierung von lokaler CPU-Ausführungsplanung trennen.
2. Scheduling DARF keine Authority erzeugen.
3. Nur Nodes, die alle relevanten Hard Constraints erfüllen, DÜRFEN als Scheduling-Ziele verwendet werden.
4. Resource Admission MUSS vor verbindlicher Ressourcenbelegung berücksichtigt werden.
5. Data Locality SOLL Bestandteil der Scheduling-Entscheidung sein.
6. Deadlines MÜSSEN Kommunikations- und Synchronisationskosten berücksichtigen können.
7. Priority DARF Security-, Trust-, Sovereignty- oder Capability-Anforderungen NICHT überschreiben.
8. Migration MUSS Hard Constraints am Ziel erneut validieren.
9. Work Stealing DARF nur für zulässige und migrierbare Tasks erfolgen.
10. Distributed Scheduling MUSS mit Structured Concurrency integrierbar sein.
11. Required Determinism DARF durch adaptive Scheduling-Entscheidungen NICHT verletzt werden.
12. Replanning DARF Hard Constraints NICHT abschwächen.
13. Resource Arbitration, Reservations und Guarantees MÜSSEN berücksichtigt werden können.
14. Scheduling State und Placement Decisions MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-STORAGE-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-EXECUTION-LATENCY-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-EXECUTION-ENERGY-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-RESOURCE-ARBITRATION-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0061`

## Ergebnis

```text
Distributed Execution Plan
        ↓
Node Eligibility
        ↓
Resource Admission
        ↓
Data + Compute Locality
        ↓
Distributed Scheduling
        ↓
Local Scheduler
        ↓
Execution
        ↓
Monitoring + Replanning
```

NovaOS erhält damit einen verteilten Scheduler, der Tasks dynamisch über lokale und entfernte Systeme platzieren kann, während Ressourcen, Datenlokalität, Deadlines, Determinismus, Trust, Sovereignty und Security als explizite Constraints erhalten bleiben.