# NPSPEC-RESOURCE-GUARANTEE-0001 – Nova Resource Guarantee

## Status

Angenommen

## Kategorie

Resource / Guarantee / Resource Management

## Zweck

NovaOS definiert ein systemweites Modell für verbindliche Ressourcengarantien.

Eine Resource Guarantee beschreibt, welche Ressourcen oder Serviceeigenschaften NovaOS einer Ausführung unter definierten Bedingungen tatsächlich zusichert.

```text
Requirement
    ↓
Reservation
    ↓
Feasibility Verification
    ↓
Guarantee
    ↓
Execution
    ↓
Continuous Verification
```

## Grundprinzipien

```text
Guarantee ≠ Reservation
Guarantee ≠ Budget
Guarantee ≠ Priority
Guarantee ≠ Capability
Guarantee ≠ Prediction
Guarantee ≠ Best Effort
Reservation ≠ Guarantee
Available Capacity ≠ Guaranteed Capacity
```

## Guarantee Model

Eine Garantie wird beschrieben durch:

```text
ResourceGuarantee
├── GuaranteeID
├── ResourceTypeID
├── Guaranteed Property
├── Guaranteed Value
├── Scope
├── Validity
└── State
```

Optional:

```text
ResourceID
ReservationID
ExecutionContractID
AccountingDomain
Start
Expiration
Conditions
Tolerance
FailurePolicy
VerificationMethod
```

## Garantierbare Eigenschaften

NovaOS kann Garantien beispielsweise definieren für:

```text
CPU Capacity
Memory Capacity
I/O Bandwidth
Network Bandwidth
GPU Capacity
NPU Capacity
Latency
Deadline
Availability
```

Nicht jede Hardware oder Ressource muss jede Garantieform unterstützen.

## Guarantee Levels

NovaOS unterscheidet mindestens:

```text
Hard Guarantee
Conditional Guarantee
Best-Effort Target
```

### Hard Guarantee

Die zugesicherte Eigenschaft muss innerhalb des definierten Gültigkeitsbereichs eingehalten werden.

### Conditional Guarantee

Die Garantie gilt nur, solange explizit definierte Bedingungen erfüllt sind.

### Best-Effort Target

Das System versucht das Ziel zu erreichen, gibt jedoch keine verbindliche Zusicherung.

```text
Best Effort ≠ Guarantee
```

## Reservation und Guarantee

Eine Reservierung kann Voraussetzung für eine Garantie sein.

```text
Reservation
    ↓
Capacity Secured
    ↓
Verification
    ↓
Guarantee
```

Eine Reservation allein beweist jedoch nicht, dass die gewünschte End-to-End-Eigenschaft garantiert werden kann.

## Feasibility Verification

Vor Erteilung einer Garantie muss NovaOS prüfen:

```text
Available Capacity
Existing Guarantees
Reservations
Hardware Limits
Resource Dependencies
Execution Requirements
System Constraints
```

Kann eine Garantie nicht zuverlässig eingehalten werden, darf sie nicht als Hard Guarantee bestätigt werden.

## End-to-End Guarantee

Garantien können mehrere Ressourcen umfassen.

```text
CPU
 +
Memory
 +
I/O
 +
Network
 ↓
End-to-End Guarantee
```

Eine einzelne garantierte Teilressource bedeutet nicht automatisch eine garantierte Gesamtoperation.

## Guarantee State

Eine Garantie besitzt einen expliziten Zustand.

```text
Requested
Verified
Active
AtRisk
Violated
Expired
Revoked
Released
```

`AtRisk` ermöglicht eine Reaktion, bevor eine Garantie tatsächlich verletzt wird.

## Continuous Verification

Aktive Garantien müssen überwacht werden können.

```text
Guarantee
    ↓
Resource Accounting
    ↓
Runtime Measurement
    ↓
Still Feasible?
```

Änderungen an Hardware, thermischem Zustand oder verfügbaren Ressourcen können die Garantie beeinflussen.

## Guarantee Violation

Eine Verletzung muss explizit erkannt werden.

```text
Guarantee
    ↓
Violation
    ↓
Failure Policy
```

Mögliche Reaktionen:

```text
Reallocate Resources
Change Provider
Activate Reservation
Graceful Degradation
Cancel Execution
Rollback
Safety Action
Report Violation
```

Eine Verletzung darf nicht stillschweigend als erfolgreiche garantierte Ausführung behandelt werden.

## Safety und Hardware Limits

Garantien dürfen keine physischen oder sicherheitsrelevanten Grenzen überschreiben.

```text
Hardware Safety
      ↓
Security
      ↓
Hard System Constraints
      ↓
Resource Guarantees
      ↓
Optimization
```

Beispiel:

Eine CPU-Leistungsgarantie darf aufgehoben werden, wenn eine kritische thermische Grenze erreicht wird.

Der Garantiebruch muss dabei explizit sichtbar sein.

## ExecutionContract

ExecutionContracts können Garantien anfordern.

```text
ExecutionContract
├── Required Guarantees
├── Acceptable Degradation
├── Failure Policy
└── Resource Requirements
```

Der ExecutionContract kann festlegen:

```text
Guarantee Required
Conditional Guarantee Accepted
Best Effort Accepted
```

## Accounting

Resource Accounting liefert Messdaten zur Überprüfung aktiver Garantien.

```text
Guaranteed
    vs.
Measured
```

Damit kann NovaOS feststellen:

```text
Satisfied
AtRisk
Violated
```

## Capability Security

Das Erteilen oder Verändern einer Garantie benötigt entsprechende Autorität.

```text
Resource Capability
        +
Guarantee Authority
        ↓
Resource Guarantee
```

Eine Garantie erzeugt selbst keine zusätzliche Zugriffsautorität.

```text
Guarantee ≠ Authority
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
GuaranteeID
Resource Type
Guaranteed Property
Guaranteed Value
Guarantee Level
State
Validity
Reservation
ExecutionContract
Current Measurement
Risk State
Violation History
```

## Normative Anforderungen

1. NovaOS MUSS Resource Guarantees explizit von Budgets, Reservations und Prioritäten unterscheiden.
2. Hard Guarantees MÜSSEN vor ihrer Aktivierung auf Erfüllbarkeit geprüft werden.
3. Best-Effort-Ziele DÜRFEN NICHT als Garantien dargestellt werden.
4. Garantien MÜSSEN einen expliziten Gültigkeitsbereich besitzen.
5. End-to-End-Garantien MÜSSEN alle relevanten Ressourcenabhängigkeiten berücksichtigen.
6. Aktive Garantien MÜSSEN zur Laufzeit überprüfbar sein.
7. Gefährdete und verletzte Garantien MÜSSEN explizit erkennbar sein.
8. Hardware-, Safety- und Security-Grenzen DÜRFEN durch Garantien NICHT überschrieben werden.
9. ExecutionContracts MÜSSEN erforderliche Guarantee Levels deklarieren können.
10. Resource Guarantees MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-CPU-0001`
- `NPSPEC-RESOURCE-MEMORY-0001`
- `NPSPEC-RESOURCE-IO-0001`
- `NPSPEC-RESOURCE-NETWORK-0001`
- `NPSPEC-RESOURCE-GPU-0001`
- `NPSPEC-RESOURCE-NPU-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-RESOURCE-THERMAL-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0044`

## Ergebnis

```text
Resource Requirement
        ↓
Reservation + Verification
        ↓
Explicit Guarantee
        ↓
Continuous Measurement
        ↓
Satisfied / AtRisk / Violated
```

NovaOS erhält damit ein explizites Resource-Guarantee-Modell, das klar zwischen gewünschter Leistung, reservierter Kapazität und tatsächlich zugesicherten Eigenschaften unterscheidet und Garantien während der gesamten Ausführung überprüfbar macht.