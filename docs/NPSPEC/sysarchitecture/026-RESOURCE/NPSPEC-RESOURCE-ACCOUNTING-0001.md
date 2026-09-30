# NPSPEC-RESOURCE-ACCOUNTING-0001 – Nova Resource Accounting

## Status

Angenommen

## Kategorie

Resource / Accounting / Resource Management

## Zweck

NovaOS definiert ein systemweites Resource Accounting zur präzisen Erfassung und Zuordnung des tatsächlichen Ressourcenverbrauchs.

```text
Resource Usage
      ↓
Measurement
      ↓
Attribution
      ↓
Accounting
      ↓
Resource Economy
```

Resource Accounting liefert die Messgrundlage für Budgets, Quotas, Scheduling, Optimierung, Diagnose und Ressourcenplanung.

## Grundprinzipien

```text
Accounting ≠ Authorization
Accounting ≠ Budget
Accounting ≠ Scheduling
Allocated ≠ Consumed
Reserved ≠ Consumed
Measured Usage ≠ Ownership
Shared Resource ≠ Unaccounted Resource
```

## Accounting-Modell

Ein Accounting Record beschreibt mindestens:

```text
ResourceAccountingRecord
├── ResourceTypeID
├── ConsumerID
├── Usage
├── Time Window
└── Accounting Domain
```

Optional:

```text
ResourceID
IdentityID
ApplicationID
ProcessID
TaskID
AgentID
ExecutionID
ExecutionContractID
TransactionID
Parent Accounting Domain
Reservation
Budget
Timestamp
Measurement Quality
```

## Accounting Domains

Ressourcenverbrauch kann hierarchisch zugeordnet werden.

```text
System
  ↓
User / Service
  ↓
Application
  ↓
Process
  ↓
Task
```

Dadurch kann Verbrauch sowohl einzelnen Workloads als auch übergeordneten Gruppen zugerechnet werden.

## Ressourcenklassen

Accounting muss mindestens möglich sein für:

```text
CPU Time
Memory
Storage
I/O
Network
GPU / Accelerator
Energy
Device Usage
IPC Resources
```

Ressourcenspezifische Metriken dürfen ergänzt werden.

## CPU Accounting

CPU-Verbrauch kann beispielsweise erfasst werden als:

```text
Execution Time
CPU Cycles
Scheduler Time
Core Usage
```

Gemeinsam genutzte CPU-Zeit muss dem verursachenden Workload möglichst eindeutig zugeordnet werden.

## Memory Accounting

Memory Accounting unterscheidet mindestens:

```text
Allocated Memory
Resident Memory
Shared Memory
Pinned Memory
Compressed Memory
```

Shared Memory darf nicht unkontrolliert mehrfach als vollständiger exklusiver Verbrauch berechnet werden.

Die Accounting Policy muss die Zuordnung definieren.

## I/O Accounting

I/O kann gemessen werden nach:

```text
Bytes Read
Bytes Written
Operations
Queue Time
Device Time
```

Caching darf bei Bedarf zwischen logischem und physischem I/O unterscheiden.

## Network Accounting

Netzwerkverbrauch kann erfassen:

```text
Bytes Sent
Bytes Received
Packets
Connections
Bandwidth Time
```

Accounting muss auch über virtuelle Netzwerkpfade hinweg dem verursachenden Workload zugeordnet werden können.

## Accelerator Accounting

GPU- und Accelerator-Verbrauch kann berücksichtigen:

```text
Execution Time
Memory Usage
Queue Usage
Compute Units
Energy
```

Geteilte Beschleuniger benötigen eine kontrollierte Zuordnung zwischen mehreren Workloads.

## Reservation und Consumption

NovaOS unterscheidet:

```text
Reserved
Allocated
Consumed
Released
```

Beispiel:

```text
Budget:    100 MiB
Reserved:   80 MiB
Allocated:  60 MiB
Consumed:   42 MiB
```

Diese Werte dürfen nicht semantisch gleichgesetzt werden.

## Hierarchische Zuordnung

Child Accounting wird in übergeordnete Domains aggregiert.

```text
Task A ─┐
Task B ─┼→ Process → Application → User
Task C ─┘
```

Dabei muss Double Accounting vermieden werden.

## ExecutionContract

Verbrauch kann direkt einem ExecutionContract zugeordnet werden.

```text
ExecutionContract
      ↓
Execution
      ↓
Measured Usage
      ↓
Budget Comparison
```

Damit kann NovaOS feststellen:

```text
Expected Resources
        vs.
Actual Resources
```

## Budget Enforcement

Resource Accounting liefert Messdaten für Budget Enforcement.

```text
Usage
  ↓
Accounting
  ↓
Budget Check
  ↓
Continue / Throttle / Degrade / Stop
```

Accounting selbst trifft jedoch nicht zwingend die Policy-Entscheidung.

## Shared Resources

Bei gemeinsam genutzten Ressourcen muss eine definierte Accounting Policy gelten.

Mögliche Modelle:

```text
Equal Share
Usage Proportional
Owner Pays
Requester Pays
System Overhead
```

Die gewählte Policy muss introspektierbar sein.

## System Overhead

Nicht eindeutig zurechenbarer Systemverbrauch kann einem eigenen Domain zugeordnet werden.

```text
Nova.System
Nova.Kernel
Nova.SharedInfrastructure
```

Systemkosten dürfen nicht willkürlich einzelnen Anwendungen zugeschrieben werden.

## Messqualität

Nicht jede Ressource kann exakt gemessen werden.

Accounting Records können deshalb eine Messqualität angeben:

```text
Exact
Estimated
Sampled
Derived
Unknown
```

Geschätzte Werte dürfen nicht als exakt dargestellt werden.

## Adaptive Systeme

Accounting kann Feedback für adaptive Optimierung liefern.

```text
Predicted Usage
      ↓
Execution
      ↓
Measured Usage
      ↓
Prediction Error
      ↓
Model Adjustment
```

Die Messdaten dürfen jedoch keine Safety-, Security- oder Privacy-Regeln umgehen.

## Sicherheit und Datenschutz

Accounting-Daten können sensible Informationen enthalten.

Beispiele:

```text
User Activity
Application Usage
Network Activity
Resource Patterns
Execution History
```

Zugriff muss daher durch Capabilities und Privacy Policies kontrolliert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Consumer
Resource Type
Current Usage
Historical Usage
Reservation
Allocation
Budget
Accounting Domain
Measurement Quality
Time Window
```

## Normative Anforderungen

1. NovaOS MUSS Ressourcenverbrauch systemweit messbar und zuordenbar machen.
2. Accounting MUSS von Authorization, Budgeting und Scheduling getrennt bleiben.
3. `Reserved`, `Allocated` und `Consumed` MÜSSEN getrennt erfassbar sein.
4. Accounting Domains MÜSSEN hierarchisch aggregierbar sein.
5. Double Accounting MUSS bei hierarchischen und gemeinsam genutzten Ressourcen vermieden werden.
6. Shared Resources MÜSSEN eine definierte Accounting Policy besitzen.
7. Resource Usage MUSS ExecutionContracts zugeordnet werden können.
8. Nicht exakt messbare Werte MÜSSEN ihre Messqualität kenntlich machen können.
9. Accounting-Daten MÜSSEN durch Security- und Privacy-Regeln geschützt werden.
10. Resource Accounting MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-SCHED-QOS-0001`
- `NPSPEC-MEMORY-PRESSURE-0001`
- `NPSPEC-IO-QOS-0001`
- `NPSPEC-NETWORK-QOS-0001`
- `ADR-ARCH-0032`

## Ergebnis

```text
Resource Consumption
        ↓
Precise Measurement
        ↓
Hierarchical Attribution
        ↓
Resource Accounting
        ↓
Budget + Policy + Optimization
```

NovaOS erhält damit eine gemeinsame Mess- und Zuordnungsschicht für Ressourcenverbrauch, auf der Resource Economy, Budget Enforcement, Scheduling, adaptive Optimierung und Systemdiagnose zuverlässig aufbauen können.