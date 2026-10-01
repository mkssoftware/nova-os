# NPSPEC-EXECUTION-CONTRACT-0001 – Nova Execution Contract

## Status

Angenommen

## Kategorie

Execution / Contract / System Architecture

## Zweck

NovaOS definiert mit dem Execution Contract eine systemweite deklarative Beschreibung dafür, **was** ausgeführt werden soll und **unter welchen Bedingungen** diese Ausführung zulässig ist.

```text
Intent / Request
      ↓
ExecutionContract
      ↓
Validation
      ↓
Planning + Resource Resolution
      ↓
Execution
      ↓
Verification
```

Der Vertrag trennt die gewünschte Operation von ihrer konkreten Implementierung, ihrem Provider, ihrer Hardware und ihrem Ausführungsort.

## Grundprinzipien

```text
Execution Contract ≠ Process
Execution Contract ≠ Capability
Execution Contract ≠ Provider
Execution Contract ≠ Resource Reservation
Execution Contract ≠ Implementation
Execution Contract ≠ Guarantee

Operation = Was?
Contract = Unter welchen Bedingungen?
Planner = Wie?
Provider = Wodurch?
Resources = Womit?
```

## Contract Model

Ein Execution Contract enthält mindestens:

```text
ExecutionContract
├── ContractID
├── OperationID
├── Input Semantic Types
├── Output Semantic Types
└── Hard Requirements
```

Optional:

```text
Soft Preferences
Resource Budget
Deadline
Latency Requirement
Determinism
Security Context
Trust Requirements
Sovereignty Requirements
Privacy Requirements
Required Capabilities
Resource Requirements
Preferred Provider
Forced Provider
Preferred Location
Forced Location
Algorithm Requirements
Precision Requirements
Failure Policy
Fallback Policy
Transaction Context
```

## Hard und Soft Constraints

NovaOS unterscheidet strikt:

```text
Hard Requirement
→ MUSS erfüllt werden

Soft Preference
→ SOLL nach Möglichkeit erfüllt werden
```

Beispiele:

```text
Hard:
- Daten dürfen Gerät nicht verlassen
- maximale Latenz 10 ms
- deterministische Ausführung erforderlich
- mindestens 512 MiB Speicher
- INT8 nicht zulässig

Soft:
- GPU bevorzugt
- energieeffizienter Provider bevorzugt
- lokale Ausführung bevorzugt
```

Eine Optimierung darf niemals ein Hard Requirement verletzen.

## Constraint-Priorität

Bei konkurrierenden Anforderungen gilt grundsätzlich:

```text
Safety
  ↓
Security
  ↓
Sovereignty / Trust
  ↓
Hard System Constraints
  ↓
Explicit User Decisions
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

## Semantische Operation

Der Contract referenziert eine semantische Operation.

```text
OperationID
├── Input Types
├── Output Types
└── Operation Semantics
```

Dadurch ist die Operation nicht an eine bestimmte Anwendung oder Implementierung gebunden.

Beispiel:

```text
Image.Resize
```

kann ausgeführt werden durch:

```text
CPU Provider
GPU Provider
NPU Provider
System Service
Application Provider
Remote Provider
```

sofern der jeweilige Provider alle Contract-Anforderungen erfüllt.

## Execution Planning

Aus dem Contract erzeugt NovaOS einen Execution Plan.

```text
ExecutionContract
      ↓
Semantic Discovery
      ↓
Capability Resolution
      ↓
Resource Resolution
      ↓
Provider Selection
      ↓
ExecutionPlan
```

Ein Execution Plan kann enthalten:

```text
Provider
Resources
Capabilities
Conversions
Dataflow
Reservations
Guarantees
Scheduling Requirements
Fallback Paths
```

## Resource Integration

Der Contract kann Ressourcen deklarativ anfordern.

```text
CPU
Memory
I/O
Network
GPU
NPU
Energy
Thermal Headroom
```

Beispiel:

```text
Memory:
  Minimum: 256 MiB
  Maximum: 1 GiB

Latency:
  Maximum: 20 ms

GPU:
  Preferred

Energy:
  Minimize
```

Die Resource Economy entscheidet anschließend über Admission, Reservation, Arbitration und Allocation.

## Deadline und Latency

Der Contract kann zeitliche Anforderungen enthalten.

```text
Deadline
Latency
Execution Budget
Jitter
Realtime Class
```

Dabei gilt:

```text
Deadline ≠ Latency
Deadline ≠ Timeout
Priority ≠ Guarantee
```

## Capability Integration

Der Execution Contract kann erforderliche Capability-Typen deklarieren.

```text
ExecutionContract
      ↓
Required Capability Types
      ↓
Capability Resolution
      ↓
Minimal Authority
```

Der Contract selbst gewährt keine Autorität.

```text
ExecutionContract ≠ Capability
```

NovaOS soll nur die für die konkrete Ausführung notwendigen Capabilities bereitstellen.

## Security

Security-Anforderungen können Bestandteil des Contracts sein.

Beispiele:

```text
Required Isolation
Required Sandbox
Allowed Data Access
Allowed Devices
Allowed Network Scope
Required Code Integrity
```

Eine schnellere Ausführung darf niemals durch Umgehung dieser Anforderungen erreicht werden.

## Trust und Sovereignty

Der Contract kann verlangen:

```text
Trusted Provider Only
Local Execution Only
Specific Trust Domain
Specific Sovereignty Domain
No Remote Processing
No External Network
```

Damit kann dieselbe semantische Operation abhängig vom Kontext unterschiedliche Provider verwenden.

## Determinismus

Der Contract kann deklarieren:

```text
Determinism:
  Required
  Preferred
  NotRequired
```

Bei `Required` dürfen ausschließlich Ausführungspfade verwendet werden, die die geforderte Determinismusklasse erfüllen.

## Precision

Numerische Operationen können Präzisionsanforderungen definieren.

```text
Precision:
  Required: FP32
```

oder:

```text
Precision:
  Minimum: FP16
  Preferred: FP32
```

NovaOS darf die Präzision nicht stillschweigend reduzieren.

## Location

Ausführungsort und Provider bleiben grundsätzlich abstrahiert.

```text
Identity ≠ Location
Operation ≠ Provider
```

Der Contract kann jedoch Location Constraints enthalten:

```text
LocalOnly
DeviceOnly
SpecificNode
TrustedDomainOnly
RegionRestricted
RemoteAllowed
```

## Fallback

Fallbacks müssen explizit zulässig sein.

```text
Preferred GPU
      ↓ unavailable
CPU Allowed?
      ↓
Yes → Replan
No  → Fail
```

NovaOS darf keine semantisch relevante Anforderung stillschweigend abschwächen.

## Laufzeitänderungen

Ändern sich Bedingungen während der Ausführung:

```text
Resource Loss
Provider Failure
Thermal Pressure
Network Failure
Deadline Risk
Capability Revocation
```

kann NovaOS:

```text
Replan
Migrate
Change Provider
Degrade
Cancel
Fail
```

jedoch nur innerhalb der Grenzen des ursprünglichen Contracts.

## Transactions

Ein Execution Contract kann Teil einer Transaktion sein.

```text
Begin
 ↓
Validate Contract
 ↓
Prepare Execution
 ↓
Execute
 ↓
Commit
 ↓
Verify
```

Ein Fehlschlag darf keine undefinierten Teilzustände hinterlassen.

## Structured Concurrency

Aus einem Contract erzeugte Tasks gehören zu einer kontrollierten Task-Hierarchie.

```text
ExecutionContract
      ↓
Root Task
├── Child Task
├── Child Task
└── Child Task
```

Cancellation, Deadline und Ressourcenlebenszyklen können dadurch strukturiert propagiert werden.

## Verification

Nach der Ausführung kann NovaOS prüfen:

```text
Correct Output Type?
Contract Constraints Satisfied?
Deadline Met?
Resource Budget Respected?
Required Precision Used?
Correct Provider Class?
Security Constraints Maintained?
```

Erst danach gilt die Contract-Ausführung als vollständig bewertet.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ContractID
OperationID
State
Hard Requirements
Soft Preferences
Selected Provider
Selected Resources
Capabilities
Execution Plan
Resource Consumption
Deadline State
Constraint Violations
Fallbacks
Result
```

## Lifecycle

```text
Created
   ↓
Validated
   ↓
Planned
   ↓
Admitted
   ↓
Executing
   ↓
Completed
```

Alternative Zustände:

```text
Rejected
Deferred
Replanning
Cancelled
Failed
ConstraintViolated
```

## Normative Anforderungen

1. NovaOS MUSS Execution Contracts als systemweites Ausführungsmodell unterstützen.
2. Operation und konkrete Implementierung MÜSSEN getrennt bleiben.
3. Hard Requirements und Soft Preferences MÜSSEN explizit unterschieden werden.
4. Hard Requirements DÜRFEN durch Optimierung NICHT abgeschwächt werden.
5. Der Contract DARF selbst keine Capability erzeugen.
6. Provider und Ressourcen SOLLEN anhand des Contracts dynamisch aufgelöst werden können.
7. Resource Budgets, Deadlines, Latency und Reservations MÜSSEN integrierbar sein.
8. Security-, Trust-, Privacy- und Sovereignty-Anforderungen MÜSSEN als Hard Constraints formulierbar sein.
9. Fallback und Degradation MÜSSEN innerhalb explizit erlaubter Grenzen erfolgen.
10. Laufzeit-Replanning DARF den ursprünglichen Contract NICHT verletzen.
11. Contract-Ausführungen MÜSSEN mit Structured Concurrency und Transactions integrierbar sein.
12. Contract-Zustand und Constraint-Verletzungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-STRUCTUREDCONCURRENCY-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ARBITRATION-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0048`

## Ergebnis

```text
Intent
  ↓
Semantic Operation
  ↓
ExecutionContract
  ↓
Validate Constraints
  ↓
Discover Capabilities + Resources
  ↓
Admission + Planning
  ↓
Provider Selection
  ↓
Controlled Execution
  ↓
Verification
```

Der Nova Execution Contract wird damit zur zentralen deklarativen Verbindung zwischen semantischer Operation, Capabilities, Ressourcen, Security, Trust, Sovereignty, Determinismus und tatsächlicher Ausführung, ohne die Operation an eine konkrete Implementierung oder Hardware zu binden.