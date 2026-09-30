# NPSPEC-RESOURCE-ADMISSION-0001 – Nova Resource Admission Control

## Status

Angenommen

## Kategorie

Resource / Admission Control / Resource Management

## Zweck

NovaOS definiert ein systemweites Admission Control, das vor einer ressourcenrelevanten Ausführung prüft, ob deren Anforderungen unter den aktuellen und zugesicherten Systembedingungen erfüllbar sind.

```text
Execution Request
      ↓
Resource Requirements
      ↓
Admission Control
      ↓
Accept / Reject / Replan
```

Damit verhindert NovaOS, dass neue Workloads bestehende Hard Guarantees, Reservations oder kritische Systemgrenzen unkontrolliert gefährden.

## Grundprinzipien

```text
Admission ≠ Authorization
Admission ≠ Scheduling
Admission ≠ Reservation
Admission ≠ Guarantee
Available Resource ≠ Admissible Workload
Accepted ≠ Successfully Completed
Rejected ≠ Permanently Impossible
Prediction ≠ Guarantee
```

## Admission Request

Eine Admission-Prüfung erhält mindestens:

```text
AdmissionRequest
├── ExecutionContractID
├── Resource Requirements
├── Hard Constraints
└── Accounting Domain
```

Optional:

```text
Deadline
Latency Requirement
Reservations
Guarantees
Priority
Duration
Burst Requirements
Fallback Policy
Provider Constraints
```

## Admission Decision

Das Ergebnis ist explizit:

```text
Accepted
AcceptedWithConstraints
Deferred
ReplanRequired
Rejected
Unknown
```

`Unknown` darf bei Hard Requirements nicht automatisch als `Accepted` behandelt werden.

## Prüfgrundlage

Admission Control berücksichtigt mindestens:

```text
Available Capacity
Current Allocation
Existing Reservations
Existing Guarantees
Resource Budgets
Current Pressure
Hardware Limits
Thermal Limits
Energy State
Deadline Feasibility
Security Constraints
```

## Ablauf

```text
Request
  ↓
Validate Requirements
  ↓
Resolve Candidate Resources
  ↓
Check Capacity
  ↓
Check Reservations
  ↓
Check Guarantees
  ↓
Check Hard Constraints
  ↓
Admission Decision
```

## Hard Requirements

Hard Requirements müssen vollständig erfüllbar sein.

```text
Hard Requirement
      ↓
Not Satisfied
      ↓
No Admission
```

NovaOS darf Hard Constraints nicht automatisch abschwächen, um einen Workload zuzulassen.

## Soft Requirements

Soft Requirements können optimiert oder kontrolliert reduziert werden.

```text
Preferred GPU
      ↓ unavailable
CPU Fallback
      ↓
AcceptedWithConstraints
```

Dies ist nur zulässig, wenn der ExecutionContract den alternativen Ausführungspfad erlaubt.

## Reservation Integration

Benötigt ein Workload reservierte Ressourcen:

```text
Admission
   ↓
Reservation Planning
   ↓
Reservation Commit
   ↓
Accepted
```

Die Admission-Entscheidung darf nicht auf Ressourcen basieren, die gleichzeitig anderen Hard Reservations zugesichert sind.

## Guarantee Integration

Für angeforderte Hard Guarantees muss Admission Control prüfen, ob diese tatsächlich abgesichert werden können.

```text
Requirement
   ↓
Feasibility
   ↓
Reservation
   ↓
Guarantee
```

Eine positive Admission-Entscheidung allein stellt noch keine Garantie dar.

## Deadline Admission

Bei zeitkritischen Workloads kann geprüft werden:

```text
Remaining Time
      ↓
Required Execution
      +
Available Capacity
      ↓
Feasible?
```

Hard-Deadline-Workloads sollen nicht angenommen werden, wenn ihre rechtzeitige Ausführung bereits zum Admission-Zeitpunkt nicht realistisch abgesichert werden kann.

## Hierarchische Ressourcen

Admission muss Parent Budgets berücksichtigen.

```text
System
 ↓
Application
 ↓
Process
 ↓
Task
```

Freie lokale Kapazität eines Tasks bedeutet nicht automatisch, dass im übergeordneten Resource Domain noch Budget verfügbar ist.

## Concurrent Admission

Mehrere parallele Admission Requests dürfen dieselbe freie Kapazität nicht mehrfach verbindlich zusagen.

```text
Request A ─┐
           ├→ Atomic Admission Decision
Request B ─┘
```

Admission und zugehörige Reservationen müssen entsprechend atomar oder transaktional koordinierbar sein.

## Dynamic Re-Admission

Ändern sich wesentliche Bedingungen, kann eine erneute Prüfung erforderlich sein.

Beispiele:

```text
Provider Failure
Resource Loss
Thermal Constraint
Migration
Guarantee Violation
ExecutionContract Change
```

Eine erneute Admission-Prüfung darf bestehende Safety- oder Security-Regeln nicht umgehen.

## Resource Pressure

Admission Control kann bei Ressourcenknappheit neue Workloads begrenzen.

```text
Normal       → Normal Admission
Elevated     → Restricted Admission
Critical     → Critical Workloads Only
```

Die konkrete Policy bleibt von der Admission-Mechanik getrennt.

## Security

Admission Control entscheidet über Ressourcenfähigkeit, nicht über Zugriffsautorität.

```text
Capability Check
      +
Admission Check
      ↓
Executable Request
```

Ein zugelassener Workload benötigt weiterhin alle erforderlichen Capabilities.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Admission Request
Decision
Reason
Evaluated Resources
Available Capacity
Required Capacity
Reservations
Guarantees
Rejected Constraints
Admission Timestamp
```

## Normative Anforderungen

1. NovaOS MUSS Resource Admission vor verbindlichen Ressourcenzusagen unterstützen.
2. Admission Control MUSS von Authorization, Scheduling, Reservation und Guarantee getrennt bleiben.
3. Hard Requirements MÜSSEN vollständig geprüft werden.
4. `Unknown` DARF bei Hard Requirements NICHT als erfolgreiche Admission gelten.
5. Bestehende Hard Reservations und Guarantees DÜRFEN durch neue Admissions NICHT verletzt werden.
6. Hierarchische Resource Budgets MÜSSEN berücksichtigt werden.
7. Parallele Admission Requests DÜRFEN Ressourcen NICHT mehrfach verbindlich zusagen.
8. Deadline-kritische Workloads SOLLEN eine Feasibility-Prüfung unterstützen.
9. Wesentliche Ressourcenänderungen MÜSSEN eine Re-Admission ermöglichen.
10. Admission Decisions MÜSSEN autorisiert introspektierbar und begründbar sein.

## Abhängigkeiten

- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-RESOURCE-ACCOUNTING-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-LATENCY-0001`
- `NPSPEC-RESOURCE-DEADLINE-0001`
- `NPSPEC-RESOURCE-ENERGY-0001`
- `NPSPEC-RESOURCE-THERMAL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `ADR-ARCH-0045`

## Ergebnis

```text
Execution Request
        ↓
Requirements + Current State
        ↓
Feasibility + Constraint Check
        ↓
Admission Decision
        ↓
Reservation / Guarantee
        ↓
Controlled Execution
```

NovaOS erhält damit ein systemweites Admission Control, das neue Workloads nur dann verbindlich zulässt, wenn ihre Ressourcenanforderungen mit vorhandenen Budgets, Reservations, Guarantees und Hard Constraints vereinbar sind.