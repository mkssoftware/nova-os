# NPSPEC-RESOURCE-ARBITRATION-0001 – Nova Resource Arbitration

## Status

Angenommen

## Kategorie

Resource / Arbitration / Resource Management

## Zweck

NovaOS definiert ein systemweites Resource Arbitration Model zur kontrollierten Entscheidung zwischen konkurrierenden Ressourcenanforderungen.

```text
Competing Requests
        ↓
Constraints + Policy
        ↓
Resource Arbitration
        ↓
Allocation Decision
        ↓
Execution
```

Arbitration entscheidet, welcher zulässige Workload eine begrenzte Ressource wann und in welchem Umfang erhält.

## Grundprinzipien

```text
Arbitration ≠ Authorization
Arbitration ≠ Admission
Arbitration ≠ Scheduling
Arbitration ≠ Priority
Arbitration ≠ Reservation
Priority ≠ Automatic Winner
Fairness ≠ Equal Allocation
Optimization ≠ Policy Override
```

## Arbitration Request

Eine Anfrage enthält mindestens:

```text
ArbitrationRequest
├── RequestID
├── Resource Requirement
├── Accounting Domain
└── ExecutionContractID
```

Optional:

```text
Priority
Deadline
ReservationID
GuaranteeID
Latency Requirement
Resource Budget
Criticality
Wait State
Preemption Policy
```

## Arbitration Decision

Das Ergebnis kann sein:

```text
Granted
PartiallyGranted
Queued
Deferred
Preempted
Rejected
```

Eine Entscheidung muss nachvollziehbar auf aktuellen Ressourcenbedingungen und gültiger Policy beruhen.

## Entscheidungsreihenfolge

NovaOS berücksichtigt grundsätzlich:

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
Reservations / Guarantees
  ↓
Realtime / Deadlines
  ↓
Resource Policy
  ↓
Soft Preferences
  ↓
Adaptive Optimization
```

Keine niedrigere Ebene darf eine höhere Constraint-Ebene überschreiben.

## Konkurrenz

Mehrere Workloads können dieselbe Ressource beanspruchen.

```text
Request A ─┐
Request B ─┼→ Arbitration → Resource
Request C ─┘
```

Arbitration kann berücksichtigen:

```text
Guaranteed Capacity
Reservations
Priority
Deadline
Fairness
Historical Usage
Resource Pressure
Wait Time
Locality
Energy
Thermal State
```

## Reservation und Guarantee

Bestehende Hard Reservations und Guarantees müssen geschützt werden.

```text
Physical Capacity
├── Guaranteed
├── Reserved
└── Available
```

Nicht reservierte Workloads dürfen zugesicherte Kapazität nicht unkontrolliert verdrängen.

## Fairness

NovaOS muss langfristige Ressourcenverdrängung vermeiden können.

Mögliche Mechanismen:

```text
Weighted Fairness
Wait-Time Aging
Quota Awareness
Usage History
Hierarchical Fairness
```

Fairness bedeutet nicht zwingend identische Ressourcenanteile.

## Priority

Prioritäten beeinflussen Arbitration, stellen aber keine Ressourcengarantie dar.

```text
High Priority ≠ Unlimited Resources
```

Priority Boosting darf Hard Limits und Security Policies nicht umgehen.

## Preemption

Ressourcen können gegebenenfalls neu zugeteilt werden.

```text
Current Consumer
       ↓
Preemption Decision
       ↓
Release / Suspend / Migrate
       ↓
New Consumer
```

Preemption muss die Semantik der Ressource berücksichtigen.

Nicht sicher unterbrechbare Operationen dürfen nicht beliebig preempted werden.

## Deadline Arbitration

Bei konkurrierenden zeitkritischen Workloads können berücksichtigt werden:

```text
Deadline
Remaining Time
Execution Budget
Criticality
Guaranteed Resources
```

Admission Control muss verhindern, dass bereits offensichtlich nicht erfüllbare Hard Guarantees erst durch Arbitration entstehen.

## Hierarchische Arbitration

Ressourcen können innerhalb von Domains verteilt werden.

```text
System
 ↓
Application
 ↓
Process
 ↓
Task
```

Arbitration innerhalb einer Child Domain darf das Budget ihrer Parent Domain nicht überschreiten.

## Multi-Resource Arbitration

Operationen benötigen häufig mehrere Ressourcen gleichzeitig.

```text
CPU
 +
Memory
 +
I/O
 +
GPU
 ↓
Execution
```

NovaOS soll koordinierte Entscheidungen ermöglichen, damit Workloads nicht eine Ressource dauerhaft blockieren, während andere zwingend benötigte Ressourcen fehlen.

## Starvation Protection

NovaOS muss Starvation erkennen und begrenzen können.

```text
Repeated Deferral
      ↓
Wait-Time Increase
      ↓
Arbitration Adjustment
```

Hard Constraints dürfen für Starvation Prevention jedoch nicht verletzt werden.

## Resource Pressure

Arbitration kann auf Pressure States reagieren.

```text
Normal
 ↓
Contended
 ↓
Constrained
 ↓
Critical
```

Mit zunehmendem Pressure können Policies restriktiver werden.

## Adaptive Arbitration

NovaOS darf historische Daten verwenden:

```text
Decision
   ↓
Execution
   ↓
Measured Result
   ↓
Prediction Error
   ↓
Policy Optimization
```

Adaptive Modelle dürfen Hard Constraints, Reservations oder Guarantees nicht überschreiben.

## Capability Security

Arbitration entscheidet ausschließlich zwischen bereits zulässigen Anforderungen.

```text
Capability Validation
        ↓
Admission
        ↓
Arbitration
```

Eine positive Arbitration Decision erzeugt keine neue Capability.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Active Requests
Resource Contention
Arbitration Decision
Decision Reason
Priority
Wait Time
Reservations
Guarantees
Preemption
Fairness State
Resource Pressure
```

## Normative Anforderungen

1. NovaOS MUSS konkurrierende Ressourcenanforderungen kontrolliert arbitrieren können.
2. Arbitration MUSS von Authorization, Admission und Scheduling getrennt bleiben.
3. Safety-, Security- und Hard Constraints MÜSSEN Vorrang besitzen.
4. Hard Reservations und Guarantees DÜRFEN durch normale Arbitration NICHT verletzt werden.
5. Priorität DARF keine unbegrenzte Ressourcennutzung erzeugen.
6. NovaOS MUSS Mechanismen gegen Starvation unterstützen.
7. Preemption DARF nur bei Ressourcen erfolgen, deren Semantik sichere Unterbrechung erlaubt.
8. Hierarchische Arbitration MUSS Parent Budgets respektieren.
9. Adaptive Arbitration DARF verbindliche Constraints NICHT überschreiben.
10. Arbitration Decisions MÜSSEN autorisiert introspektierbar und begründbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-RESOURCE-THERMAL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0046`

## Ergebnis

```text
Competing Resource Requests
           ↓
Constraints + Reservations + Guarantees
           ↓
Resource Arbitration
           ↓
Fair + Policy-Compliant Decision
           ↓
Allocation / Queue / Preemption
```

NovaOS erhält damit eine gemeinsame Arbitration-Schicht, die konkurrierende Ressourcenanforderungen nachvollziehbar auflöst und dabei Hard Constraints, Garantien, Reservierungen, Fairness und zeitkritische Anforderungen berücksichtigt.