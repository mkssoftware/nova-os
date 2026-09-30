# NPSPEC-RESOURCE-IO-0001 – Nova I/O Resource Model

## Status

Angenommen

## Kategorie

Resource / I/O / Resource Management

## Zweck

NovaOS definiert Ein-/Ausgabe-Kapazität als explizite, budgetierbare, messbare und kontrollierbare Systemressource.

```text
I/O Demand
    ↓
Budget + Policy
    ↓
I/O Scheduler
    ↓
Device / Provider
    ↓
Completion + Accounting
```

Das Modell verbindet asynchrones I/O, Resource Economy, QoS, Deadlines und Capability Security.

## Grundprinzipien

```text
Device Access ≠ Unlimited I/O
I/O Capability ≠ I/O Budget
Bandwidth ≠ IOPS
Submitted I/O ≠ Completed I/O
Priority ≠ Unlimited Throughput
Reservation ≠ Consumption
Queue Depth ≠ Performance Guarantee
I/O Completion ≠ Semantic Success
```

## I/O Resource

Eine I/O-Ressource wird beschrieben durch:

```text
IOResource
├── ResourceID
├── ResourceTypeID
├── State
├── Capacity
└── I/O Domain
```

Optional:

```text
Bandwidth
IOPS
Latency
Queue Capacity
Block / Stream / Message
Read / Write Capability
DMA Support
Zero-Copy Support
DeviceID
ProviderID
```

## I/O Requirement

Workloads können ihren I/O-Bedarf deklarieren.

```text
IORequirement
├── Operation Type
├── Required Throughput
└── I/O Budget
```

Optional:

```text
Maximum Latency
Deadline
IOPS
Queue Depth
Priority
Ordering
Durability
Zero-Copy Preference
Locality
```

## I/O Budget

Resource Economy kann I/O-Budgets definieren.

```text
IOBudget
├── Byte Limit
├── Operation Limit
├── Time Window
└── Enforcement Policy
```

Optional:

```text
Bandwidth Reservation
IOPS Reservation
Burst Limit
Latency Target
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

## Asynchrones I/O

I/O soll grundsätzlich asynchron ausführbar sein.

```text
Submit
  ↓
Pending
  ↓
Executing
  ↓
Completion
```

Ein wartender I/O-Vorgang soll keinen CPU-Thread unnötig blockieren müssen.

## I/O Scheduling

Der I/O Scheduler verteilt begrenzte I/O-Kapazität.

Berücksichtigt werden können:

```text
Priority
Fairness
Deadline
Latency
Throughput
Queue State
Resource Budget
Device Characteristics
```

Hard Requirements dürfen durch Optimierungen nicht verletzt werden.

## QoS

I/O kann unterschiedliche Serviceklassen verwenden.

```text
Realtime
Interactive
Normal
Background
Bulk
```

QoS kann Ziele für:

```text
Latency
Bandwidth
IOPS
Priority
```

definieren.

QoS ist keine Garantie, sofern keine entsprechende Ressource reserviert wurde.

## Deadline

I/O-Requests können Deadlines besitzen.

```text
Submit
  ↓
Deadline
  ↓
Complete / Deadline Miss
```

Ein Deadline Miss muss explizit erkennbar sein.

```text
Deadline ≠ Timeout
```

## Backpressure

Producer dürfen I/O-Systeme nicht unbegrenzt mit Requests füllen.

```text
Producer
   ↓
Bounded Queue
   ↓
I/O Provider
```

Mögliche Reaktionen:

```text
Delay
Throttle
Reject
Coalesce
Batch
```

## Zero-Copy

I/O soll Zero-Copy unterstützen können.

```text
Object Buffer
     ↓
DMA / Shared Buffer
     ↓
Device
```

Falls Zero-Copy nicht möglich oder sicher ist:

```text
Zero-Copy
   ↓
Copy Fallback
```

Die semantische Operation darf dadurch nicht verändert werden.

## DMA

Direkter Speicherzugriff durch Geräte benötigt separate Kontrolle.

```text
I/O Request
   ↓
DMA Capability
   ↓
IOMMU Mapping
   ↓
Device
```

Eine I/O Capability darf nicht automatisch beliebigen DMA-Zugriff erlauben.

## I/O Accounting

Mindestens folgende Werte sollen messbar sein:

```text
Bytes Read
Bytes Written
Operations
Queue Time
Service Time
Completion Time
Bandwidth
IOPS
```

Die Zuordnung kann erfolgen nach:

```text
Application
Process
Task
Service
Agent
ExecutionContract
Accounting Domain
```

## Capability Security

I/O benötigt explizite Autorität.

```text
I/O Capability
      +
I/O Budget
      ↓
Permitted I/O
```

Rechte können getrennt werden in:

```text
Read
Write
Control
Configure
Flush
Discard
Direct I/O
DMA
```

## Fehler und Degradation

Bei Überlastung oder Ausfall kann NovaOS reagieren mit:

```text
Throttle
Requeue
Alternative Provider
Reduced QoS
Graceful Degradation
Controlled Failure
```

Eine Wiederholung darf nur erfolgen, wenn die Operation dies sicher erlaubt.

## ExecutionContract

Ein ExecutionContract kann definieren:

```text
I/O Budget
Latency
Deadline
Throughput
Durability
Priority
Zero-Copy Requirement
Locality
```

Resource Resolution und I/O Scheduling müssen diese Anforderungen berücksichtigen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
I/O Capacity
Current Usage
Queue Depth
Bandwidth
IOPS
Latency
Reservations
Budgets
Pressure State
Provider
Accounting
```

## Normative Anforderungen

1. NovaOS MUSS I/O-Kapazität als explizite Systemressource behandeln.
2. I/O-Verbrauch MUSS durch Resource Accounting messbar sein.
3. I/O Budgets MÜSSEN hierarchisch begrenzbar sein.
4. I/O Capability und I/O Budget MÜSSEN getrennte Konzepte bleiben.
5. I/O MUSS asynchron ausführbar sein können.
6. I/O Scheduling MUSS QoS, Prioritäten und Deadlines berücksichtigen können.
7. Backpressure MUSS unkontrolliertes Queue-Wachstum verhindern können.
8. Zero-Copy SOLL unterstützt werden, MUSS aber einen sicheren Copy-Fallback erlauben.
9. DMA MUSS separat autorisiert und durch geeignete Hardwaremechanismen isolierbar sein.
10. I/O-Ressourcen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-IO-ASYNC-0001`
- `NPSPEC-IO-COMPLETION-0001`
- `NPSPEC-IO-REQUEST-0001`
- `NPSPEC-IO-SCHEDULER-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-IO-DEADLINE-0001`
- `NPSPEC-IO-ZEROCOPY-0001`
- `NPSPEC-DATAMOVE-DMA-0001`
- `ADR-ARCH-0035`

## Ergebnis

```text
I/O Capacity
     +
I/O Demand
     ↓
Budget + Capability + QoS
     ↓
I/O Scheduler
     ↓
Async Execution
     ↓
Completion + Accounting
```

NovaOS erhält damit ein einheitliches I/O-Ressourcenmodell, das Ein-/Ausgabe systemweit budgetiert, priorisiert, abrechnet und mit asynchroner Ausführung, QoS, Deadlines, Zero-Copy und sicherem DMA verbindet.