# NPSPEC-RESOURCE-ENERGY-0001 – Nova Energy Resource Model

## Status

Angenommen

## Kategorie

Resource / Energy / Power Management

## Zweck

NovaOS definiert Energie als explizite, budgetierbare und messbare Systemressource.

```text
Workload
   ↓
Energy Requirement
   ↓
Budget + Policy
   ↓
Resource Selection
   ↓
Execution
   ↓
Energy Accounting
```

Damit kann NovaOS Performance, Laufzeit, Energieverbrauch und thermische Belastung gemeinsam berücksichtigen.

## Grundprinzipien

```text
Available Energy ≠ Unlimited Energy
Energy Budget ≠ Power Limit
Energy Usage ≠ CPU Usage
Energy Optimization ≠ Performance Minimization
Low Power ≠ Highest Efficiency
Priority ≠ Unlimited Energy
Estimated Energy ≠ Exact Measurement
Energy Policy ≠ Authority
```

## Energy Resource

Energie wird als systemweite Ressource beschrieben.

```text
EnergyResource
├── ResourceID
├── ResourceTypeID
├── State
├── Available Energy
└── Power State
```

Optional:

```text
Energy Source
Battery Capacity
Charge Level
Power Limit
Thermal Constraints
Measurement Capability
Efficiency Characteristics
```

## Energy Sources

NovaOS kann unterschiedliche Energiequellen berücksichtigen:

```text
AC Power
Battery
UPS
External Power
Energy Harvesting
Unknown Source
```

Die verfügbare Energiequelle kann Resource Policies beeinflussen.

## Energy Budget

Workloads können Energiebudgets erhalten.

```text
EnergyBudget
├── Energy Limit
├── Time Window
└── Enforcement Policy
```

Optional:

```text
Power Limit
Reservation
Burst Allowance
Minimum Runtime Target
Priority
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

## Energy und Power

NovaOS unterscheidet ausdrücklich:

```text
Energy = verbrauchte Energiemenge
Power  = momentane Leistungsaufnahme
```

Ein Workload kann wenig Zeit benötigen, aber kurzfristig hohe Leistung beanspruchen.

Deshalb können Energy Budget und Power Limit unabhängig definiert werden.

## Energy Accounting

Energieverbrauch soll soweit technisch möglich zugeordnet werden können nach:

```text
Application
Process
Task
Service
Agent
ExecutionContract
Accounting Domain
```

Messwerte können klassifiziert werden als:

```text
Exact
Estimated
Sampled
Derived
Unknown
```

## Hardware-Zuordnung

Energieverbrauch kann Ressourcen zugeordnet werden wie:

```text
CPU
GPU
NPU
Memory
Storage
Network
Display
Devices
```

Gemeinsam verursachter Verbrauch benötigt eine definierte Accounting Policy.

## ExecutionContract

Ein ExecutionContract kann Energieanforderungen enthalten.

```text
ExecutionContract
├── Energy Budget
├── Power Limit
├── Performance Requirement
├── Deadline
└── Energy Preference
```

Beispiele:

```text
Prefer Lowest Energy
Prefer Highest Performance
Balanced
Maximum Battery Runtime
Energy Hard Limit
```

Hard Requirements haben Vorrang vor Optimierungspräferenzen.

## Resource Selection

NovaOS kann unterschiedliche Ausführungspfade anhand ihrer Energieeigenschaften vergleichen.

```text
Operation
   ↓
Possible Providers
├── CPU
├── GPU
└── NPU
   ↓
Energy + Performance Evaluation
   ↓
Provider Selection
```

Die energieeffizienteste Ressource ist nicht zwingend die Ressource mit der geringsten Leistungsaufnahme.

## Adaptive Optimierung

NovaOS kann erwarteten und tatsächlichen Energieverbrauch vergleichen.

```text
Predicted Energy
       ↓
Execution
       ↓
Measured Energy
       ↓
Prediction Error
       ↓
Model Adjustment
```

Dadurch können zukünftige Ressourcenentscheidungen verbessert werden.

## Battery-Aware Execution

Bei batteriebetriebenen Geräten kann NovaOS den Systemzustand berücksichtigen.

```text
Normal
 ↓
Energy Constrained
 ↓
Low Energy
 ↓
Critical
```

Mögliche Reaktionen:

```text
Reduce Background Work
Prefer Efficient Hardware
Reduce Burst Usage
Delay Non-Critical Tasks
Graceful Degradation
Suspend Work
```

## Realtime und Safety

Energieoptimierung darf kritische Anforderungen nicht verletzen.

```text
Safety
  ↓
Realtime / Hard Constraints
  ↓
Energy Optimization
```

Ein Hard-Realtime-Task darf nicht verlangsamt werden, wenn dadurch seine Deadline verletzt würde.

## Thermal Interaction

Energie und thermische Belastung sind verbunden, bleiben aber getrennte Ressourcenmodelle.

```text
Power Consumption
       ↓
Heat Generation
       ↓
Thermal State
       ↓
Resource Policy
```

Thermal Limits können Energy- und Performance-Entscheidungen einschränken.

## Capability Security

Privilegierte Energie- und Hardwaresteuerung benötigt explizite Capabilities.

Beispiele:

```text
Power Policy Control
CPU Power State Control
GPU Power Control
Device Power Control
System Suspend
Battery Administration
```

Normale Ausführung erzeugt keine Power-Management-Autorität.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Energy Source
Available Energy
Battery State
Current Power
Energy Consumption
Energy Budgets
Power Limits
Energy Pressure
Accounting
Measurement Quality
```

## Normative Anforderungen

1. NovaOS MUSS Energie als explizite Systemressource behandeln können.
2. Energy und Power MÜSSEN getrennte Größen bleiben.
3. Energieverbrauch SOLL soweit technisch möglich Workloads zugeordnet werden.
4. Geschätzte und gemessene Werte MÜSSEN unterscheidbar sein.
5. Energy Budgets MÜSSEN hierarchisch definierbar sein.
6. ExecutionContracts MÜSSEN Energieanforderungen ausdrücken können.
7. Resource Selection SOLL Energieeffizienz berücksichtigen können.
8. Energieoptimierung DARF Safety-, Security-, Realtime- oder Hard Requirements NICHT verletzen.
9. Privilegierte Power-Steuerung MUSS Capability-kontrolliert sein.
10. Energy Resources MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-RESOURCE-GPU-0001`
- `NPSPEC-RESOURCE-NPU-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SCHED-ENERGY-0001`
- `NPSPEC-SCHED-THERMAL-0001`
- `ADR-ARCH-0039`

## Ergebnis

```text
Energy Supply
     +
Workload Demand
     ↓
Energy Budget + ExecutionContract
     ↓
Resource Selection
     ↓
Controlled Execution
     ↓
Energy Accounting + Feedback
```

NovaOS erhält damit ein systemweites Energiemodell, das Energieverbrauch gemeinsam mit Performance, Realtime, Hardwareauswahl und Resource Economy verwaltet und damit energieeffiziente Entscheidungen ermöglicht, ohne harte Systemanforderungen zu verletzen.