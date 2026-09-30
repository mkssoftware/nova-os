# NPSPEC-RESOURCE-GPU-0001 – Nova GPU Resource Model

## Status

Angenommen

## Kategorie

Resource / GPU / Accelerator / Resource Management

## Zweck

NovaOS definiert GPU-Rechenleistung als explizite, budgetierbare, messbare und kontrollierbare Systemressource.

```text
Compute Demand
      ↓
GPU Requirement
      ↓
Capability + Budget + Policy
      ↓
GPU Resource Resolution
      ↓
Execution
      ↓
Accounting
```

GPU-Ressourcen werden nicht direkt an Anwendungen gekoppelt, sondern über das gemeinsame Resource-, Capability- und ExecutionContract-Modell verwaltet.

## Grundprinzipien

```text
GPU Access ≠ Unlimited GPU Usage
GPU Capability ≠ GPU Budget
GPU Memory ≠ System Memory
Queue Access ≠ Device Ownership
GPU Availability ≠ GPU Authority
Reservation ≠ Consumption
GPU Execution ≠ Guaranteed Completion
Hardware Acceleration ≠ Required Execution Path
```

## GPU Resource

Eine GPU-Ressource wird beschrieben durch:

```text
GPUResource
├── ResourceID
├── ResourceTypeID
├── DeviceID
├── State
├── Compute Capacity
└── Memory Capacity
```

Optional:

```text
Architecture
Compute Units
Supported Operations
Memory Domains
Queue Capacity
DMA Support
Zero-Copy Support
Power State
Thermal State
Driver Provider
Isolation Capabilities
```

## GPU Requirement

Workloads deklarieren benötigte Eigenschaften statt konkrete Geräte vorauszusetzen.

```text
GPURequirement
├── Required Operations
├── Compute Requirement
└── Resource Budget
```

Optional:

```text
Memory Requirement
Latency
Deadline
Determinism
Precision
Queue Requirement
Zero-Copy Preference
Energy Preference
Device Locality
```

## GPU Budget

Resource Economy kann GPU-Budgets definieren.

```text
GPUBudget
├── Compute Time
├── Memory Limit
├── Queue Limit
└── Time Window
```

Optional:

```text
Reservation
Burst Limit
Energy Budget
Bandwidth Budget
```

Budgets können hierarchisch gelten:

```text
System
 ↓
Application
 ↓
Process
 ↓
Task
```

## GPU Memory

GPU-Speicher wird separat verwaltet und abgerechnet.

```text
Device Local Memory
Shared Memory
Mapped System Memory
Pinned Memory
Transfer Buffers
```

Dabei gilt:

```text
GPU Memory Allocation ≠ GPU Compute Authority
```

## Execution

GPU-Ausführung erfolgt über kontrollierte Work Queues.

```text
Task
 ↓
ExecutionContract
 ↓
GPU Provider
 ↓
Command Queue
 ↓
GPU Execution
 ↓
Completion
```

Eine Anwendung soll keine unkontrollierte direkte Kontrolle über globale GPU-Zustände erhalten.

## Accelerator Selection

NovaOS kann zwischen CPU, GPU und anderen Accelerators wählen.

```text
Semantic Operation
       ↓
ExecutionContract
       ↓
Provider Selection
       ↓
CPU / GPU / Accelerator
```

Die Auswahl kann berücksichtigen:

```text
Performance
Latency
Energy
Memory Locality
Precision
Determinism
Resource Pressure
```

Ein `Forced Provider` oder Hard Requirement darf nicht automatisch überschrieben werden.

## Isolation

Mehrere Workloads können dieselbe GPU verwenden.

```text
GPU
├── Workload A
├── Workload B
└── Workload C
```

NovaOS muss verfügbare Hardware- und Treibermechanismen zur Isolation verwenden.

Ein Workload darf nicht auf Speicher, Queues oder Zustände anderer Security Domains zugreifen.

## DMA und Zero-Copy

GPU-Datenbewegung kann verwenden:

```text
DMA
Shared Buffer
Mapped Memory
Zero-Copy
```

Direkter DMA-Zugriff benötigt kontrollierte:

```text
DMA Capability
Memory Capability
GPU Capability
IOMMU Mapping
```

Falls Zero-Copy nicht sicher oder möglich ist, muss ein Copy-Fallback möglich bleiben.

## Scheduling

GPU-Arbeit muss kontrolliert geplant werden können.

Berücksichtigt werden können:

```text
Priority
Fairness
Deadline
Queue Pressure
Compute Budget
Memory Budget
Realtime Requirements
```

Lang laufende GPU-Workloads dürfen das System nicht unkontrolliert blockieren.

## GPU Accounting

Mindestens folgende Größen sollen erfassbar sein:

```text
Execution Time
Queue Time
Compute Usage
GPU Memory
Memory Bandwidth
Transfers
Energy Usage
```

Die Zuordnung kann erfolgen nach:

```text
Application
Process
Task
Agent
ExecutionContract
Accounting Domain
```

## Pressure

GPU-Ressourcen können eigene Pressure-Zustände besitzen.

```text
Normal
 ↓
Busy
 ↓
Contended
 ↓
Critical
```

Mögliche Reaktionen:

```text
Queue Throttling
Memory Reclaim
Reduced Parallelism
Alternative GPU
CPU Fallback
Graceful Degradation
Controlled Failure
```

Fallback ist nur zulässig, wenn der ExecutionContract dies erlaubt.

## Fehlerbehandlung

GPU-Fehler dürfen nicht automatisch das gesamte System destabilisieren.

NovaOS soll unterscheiden können zwischen:

```text
Task Failure
Queue Failure
Driver Failure
GPU Reset
Device Failure
```

Betroffene Workloads sollen soweit möglich isoliert behandelt werden.

## Capability Security

Privilegierte GPU-Operationen benötigen explizite Capabilities.

Beispiele:

```text
GPU Execute
GPU Memory
GPU Queue
GPU Control
GPU Administration
DMA
Performance Counters
```

Normale GPU-Ausführung erzeugt keine administrative GPU-Autorität.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
GPU Resources
Compute Capacity
Memory Capacity
Current Usage
Queues
Reservations
Budgets
Pressure
Temperature
Power State
Accounting
Active Workloads
```

## Normative Anforderungen

1. NovaOS MUSS GPUs als explizite Compute-Ressourcen behandeln.
2. GPU-Verbrauch MUSS durch Resource Accounting messbar sein.
3. GPU Compute und GPU Memory MÜSSEN separat budgetierbar sein.
4. GPU Capability und GPU Budget MÜSSEN getrennte Konzepte bleiben.
5. GPU-Ausführung MUSS zwischen Security Domains isolierbar sein.
6. GPU Scheduling MUSS Budgets, Prioritäten und Deadlines berücksichtigen können.
7. DMA und Zero-Copy MÜSSEN kontrolliert und capability-basiert erfolgen.
8. NovaOS SOLL CPU-, GPU- und Accelerator-Provider anhand des ExecutionContract auswählen können.
9. Fallback auf andere Compute-Ressourcen DARF Hard Requirements NICHT verletzen.
10. GPU-Ressourcen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SCHED-ACCELERATOR-0001`
- `NPSPEC-SCHED-HETEROGENEOUS-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `ADR-ARCH-0037`

## Ergebnis

```text
GPU Capacity
     +
Compute Demand
     ↓
Capability + Budget + ExecutionContract
     ↓
GPU Resolution + Scheduling
     ↓
Isolated Execution
     ↓
Accounting + Feedback
```

NovaOS erhält damit ein einheitliches GPU-Ressourcenmodell, das GPU-Rechenleistung und GPU-Speicher kontrolliert verteilt und gleichzeitig Hardwarebeschleunigung, Isolation, Zero-Copy, DMA, Scheduling und dynamische Provider-Auswahl in die Resource Economy integriert.