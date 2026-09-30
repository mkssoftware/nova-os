# NPSPEC-RESOURCE-NPU-0001 – Nova NPU Resource Model

## Status

Angenommen

## Kategorie

Resource / NPU / AI Accelerator / Resource Management

## Zweck

NovaOS definiert Neural Processing Units (NPUs) als explizite, budgetierbare und kontrollierbare Compute-Ressourcen für KI-, Machine-Learning- und neuronale Workloads.

```text
AI Workload
    ↓
NPU Requirement
    ↓
Capability + Budget + Policy
    ↓
NPU Resource Resolution
    ↓
Execution
    ↓
Accounting
```

Anwendungen sollen keine konkrete NPU-Architektur voraussetzen müssen.

## Grundprinzipien

```text
NPU Access ≠ Unlimited NPU Usage
NPU Capability ≠ NPU Budget
Model Access ≠ NPU Authority
NPU Memory ≠ System Memory
NPU Availability ≠ NPU Compatibility
Hardware Acceleration ≠ Required Execution Path
Provider Selection ≠ Authority Grant
```

## NPU Resource

Eine NPU wird beschrieben durch:

```text
NPUResource
├── ResourceID
├── ResourceTypeID
├── DeviceID
├── State
├── Compute Capacity
└── Supported Operations
```

Optional:

```text
Architecture
Supported Data Types
Supported Operators
Tensor Capabilities
Memory Capacity
Memory Bandwidth
Queue Capacity
DMA Support
Zero-Copy Support
Power State
Thermal State
Driver Provider
```

## NPU Requirement

KI-Workloads deklarieren benötigte Eigenschaften semantisch.

```text
NPURequirement
├── Operation Requirements
├── Precision Requirements
├── Compute Requirement
└── Resource Budget
```

Optional:

```text
Memory Requirement
Latency
Deadline
Determinism
Tensor Layout
Supported Operators
Energy Preference
Locality
Privacy Requirements
```

## Unterstützte Präzision

NPUs können unterschiedliche Datentypen unterstützen.

Beispiele:

```text
FP32
FP16
BF16
INT16
INT8
INT4
```

Die Ausführung darf die geforderte numerische Präzision nicht ohne explizite Erlaubnis reduzieren.

```text
Optimization ≠ Permission to Reduce Precision
```

## NPU Budget

Resource Economy kann NPU-Budgets definieren.

```text
NPUBudget
├── Compute Time
├── Memory Limit
├── Queue Limit
└── Time Window
```

Optional:

```text
Energy Budget
Bandwidth Budget
Reservation
Burst Limit
```

## Provider-Auswahl

NovaOS kann semantische AI-Operationen auf unterschiedliche Compute-Ressourcen abbilden.

```text
AI Operation
     ↓
ExecutionContract
     ↓
Provider Resolution
     ↓
CPU / GPU / NPU / Accelerator
```

Berücksichtigt werden können:

```text
Performance
Latency
Energy
Precision
Memory
Supported Operators
Determinism
Privacy
Resource Pressure
```

## Model Execution

Modelle werden nicht direkt an eine bestimmte NPU gebunden.

```text
Model
  +
Semantic Operation
  +
ExecutionContract
        ↓
Execution Plan
        ↓
NPU Provider
```

Provider-spezifische Optimierungen dürfen intern durchgeführt werden, solange die geforderte Semantik erhalten bleibt.

## Compilation und Lowering

Falls erforderlich, kann ein Modell für eine konkrete NPU transformiert werden.

```text
Model / Compute Graph
        ↓
Optimization
        ↓
NPU Lowering
        ↓
Executable Representation
```

Dabei müssen relevante Anforderungen erhalten bleiben:

```text
Precision
Semantics
Security
Determinism
Privacy
```

## Speicher und Datenbewegung

NPU-Ausführung kann verwenden:

```text
Device Memory
Shared Memory
Pinned Memory
DMA
Zero-Copy
```

Direkter Zugriff benötigt die entsprechenden Memory-, DMA- und NPU-Capabilities.

Falls Zero-Copy nicht sicher möglich ist, muss ein Copy-Fallback möglich bleiben.

## Isolation

Mehrere Workloads können dieselbe NPU verwenden.

```text
NPU
├── Workload A
├── Workload B
└── Workload C
```

Speicher, Queues, Modelle und Ausführungszustände unterschiedlicher Security Domains müssen soweit technisch möglich isoliert werden.

## Scheduling

NPU-Workloads können geplant werden nach:

```text
Priority
Fairness
Deadline
Latency
Compute Budget
Memory Budget
Energy Budget
Queue Pressure
```

Interaktive KI-Funktionen können dadurch anders behandelt werden als Hintergrund-Inferenz.

## Privacy und Sovereignty

KI-Daten können besonders sensible Inhalte enthalten.

ExecutionContracts können daher Anforderungen definieren wie:

```text
Local Execution Only
No Remote Provider
Trusted Provider Required
Specific Sovereignty Domain
Protected Model
Protected Input
```

Ein schnellerer externer Provider darf solche Hard Constraints nicht umgehen.

## NPU Accounting

Mindestens folgende Größen sollen erfassbar sein:

```text
Execution Time
Queue Time
Compute Usage
Memory Usage
Memory Transfers
Inference Count
Energy Usage
```

Zuordnung kann erfolgen nach:

```text
Application
Process
Task
Agent
Model
ExecutionContract
Accounting Domain
```

## Pressure und Fallback

Bei NPU-Knappheit kann NovaOS reagieren mit:

```text
Queue
Throttle
Alternative NPU
GPU Fallback
CPU Fallback
Reduced Parallelism
Graceful Degradation
Controlled Failure
```

Fallback ist nur zulässig, wenn der ExecutionContract dies erlaubt.

## Capability Security

Privilegierte Operationen benötigen explizite Capabilities.

Beispiele:

```text
NPU Execute
NPU Memory
NPU Queue
NPU Control
NPU Administration
DMA
Performance Counters
```

Normale AI-Ausführung erzeugt keine administrative NPU-Autorität.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Available NPUs
Supported Operations
Supported Precision
Compute Capacity
Memory Capacity
Current Usage
Queues
Budgets
Reservations
Pressure
Power State
Thermal State
Accounting
```

## Normative Anforderungen

1. NovaOS MUSS NPUs als explizite Compute-Ressourcen behandeln können.
2. NPU-Verbrauch MUSS durch Resource Accounting messbar sein.
3. NPU Compute und Memory MÜSSEN budgetierbar sein.
4. NPU Capability und NPU Budget MÜSSEN getrennte Konzepte bleiben.
5. Workloads SOLLEN semantische Anforderungen statt konkrete NPU-Modelle deklarieren.
6. Provider-Auswahl MUSS Precision-, Security-, Privacy- und ExecutionContract-Anforderungen berücksichtigen.
7. Numerische Präzision DARF nicht ohne explizite Erlaubnis reduziert werden.
8. CPU-, GPU- oder andere Fallbacks DÜRFEN Hard Requirements NICHT verletzen.
9. DMA und Zero-Copy MÜSSEN capability-kontrolliert erfolgen.
10. NPU-Ressourcen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-RESOURCE-GPU-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SCHED-ACCELERATOR-0001`
- `NPSPEC-SCHED-HETEROGENEOUS-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `NPSPEC-DATAMOVE-ZEROCOPY-0001`
- `NPSPEC-HAL-IOMMU-0001`
- `ADR-ARCH-0038`

## Ergebnis

```text
AI Workload
     ↓
Semantic Requirements
     ↓
ExecutionContract
     ↓
CPU / GPU / NPU Resolution
     ↓
Controlled NPU Execution
     ↓
Accounting + Feedback
```

NovaOS erhält damit ein hardwareunabhängiges NPU-Ressourcenmodell, das KI-Beschleuniger in die gemeinsame Resource Economy integriert und eine kontrollierte Auswahl zwischen CPU, GPU, NPU und zukünftigen Accelerators ermöglicht.