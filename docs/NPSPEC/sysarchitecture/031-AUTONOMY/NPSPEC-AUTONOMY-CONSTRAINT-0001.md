# NPSPEC-AUTONOMY-CONSTRAINT-0001 – Nova Autonomy Constraint

## Status

Angenommen

## Kategorie

Autonomy / Constraints / Safety / System Control

## Zweck

NovaOS definiert ein systemweites Constraint-Modell, das den zulässigen Handlungsspielraum autonomer Komponenten verbindlich begrenzt.

```text
Autonomous Intent
       ↓
Constraint Evaluation
       ↓
Allowed Action Space
       ↓
Autonomous Decision
       ↓
Execution
```

Autonomie darf ausschließlich innerhalb gültiger Constraints stattfinden.

## Grundprinzipien

```text
Autonomy ≠ Unlimited Freedom
Constraint ≠ Preference
Constraint ≠ Optimization Goal
Constraint ≠ Prediction
Constraint ≠ Capability

Allowed by Constraint ≠ Authorized
Authorized ≠ Allowed by Constraint
```

Eine autonome Aktion benötigt sowohl gültige Authority als auch erfüllte Constraints.

## Constraint Model

```text
AutonomyConstraint
├── ConstraintID
├── ConstraintType
├── Scope
├── Condition
├── Enforcement
└── State
```

Optional:

```text
PolicyID
IdentityID
CapabilityID
ResourceID
ExecutionID
ObjectID
ProviderID
Priority
Validity
Expiration
ProvenanceID
```

## Constraint Types

NovaOS unterscheidet mindestens:

```text
Safety
Security
Capability
Trust
Sovereignty
Resource
Execution
Realtime
Deadline
Location
Privacy
User
Operational
```

## Hard und Soft Constraints

```text
Hard Constraint
→ darf nicht verletzt werden

Soft Constraint
→ darf optimiert oder abgewogen werden
```

Beispiele:

```text
Security Boundary       → Hard
Data Sovereignty        → Hard
Memory Limit            → Hard
Preferred Provider      → Soft
Energy Preference       → Soft
Performance Preference  → Soft
```

## Constraint Hierarchy

Bei Konflikten gilt:

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
Autonomy Policy
  ↓
Soft Constraints
  ↓
Adaptive Optimization
```

Eine niedrigere Ebene darf keine höhere Ebene überschreiben.

## Allowed Action Space

Constraints definieren den zulässigen Entscheidungsraum.

```text
Possible Actions
      ↓
Hard Constraints
      ↓
Authorized Actions
      ↓
Autonomy Policy
      ↓
Allowed Action Space
```

Adaptive Optimierung darf ausschließlich innerhalb dieses verbleibenden Raums arbeiten.

## Constraint Evaluation

Vor einer autonomen Aktion muss geprüft werden:

```text
Authority
Capabilities
Security
Trust
Sovereignty
Resources
Execution Contract
User Overrides
Autonomy Policy
```

Erst danach darf die Aktion ausgeführt werden.

## Dynamic Constraints

Constraints können sich während der Laufzeit ändern.

Beispiele:

```text
Battery Level
Thermal State
Network State
Trust State
Resource Pressure
Device Availability
User Override
```

Autonome Komponenten müssen relevante Änderungen berücksichtigen.

## Constraint Invalidation

Wird ein Constraint während einer autonomen Operation verletzt:

```text
Constraint Change
       ↓
Revalidation
       ↓
Violation
       ↓
Cancel / Pause / Degrade / Rollback
```

Die konkrete Reaktion richtet sich nach Operation und Policy.

## Execution Contract

Execution Contracts können zusätzliche Constraints liefern.

```text
Execution Contract
├── Deadline
├── Resource Budget
├── Determinism
├── Trust
├── Sovereignty
└── Location
```

Diese werden Teil des effektiven Constraint-Sets der Execution.

## User Constraints

Explizite Nutzerentscheidungen können zusätzliche Grenzen setzen.

Beispiele:

```text
Never use Cloud
Only Local Execution
Do Not Preload
Do Not Migrate
Maximum Energy Usage
Preferred Provider
```

Nutzer-Constraints dürfen übergeordnete Safety- oder Security-Regeln nicht abschwächen.

## Constraint Composition

Mehrere Constraints werden kombiniert.

```text
Effective Constraints =
    System Constraints
  ∩ Security Constraints
  ∩ Trust Constraints
  ∩ Sovereignty Constraints
  ∩ Execution Constraints
  ∩ User Constraints
  ∩ Autonomy Constraints
```

Das Ergebnis definiert den zulässigen Handlungsraum.

## Conflict Handling

Sind Constraints widersprüchlich:

```text
Constraint A
     ×
Constraint B
     ↓
No Valid Action
```

darf NovaOS nicht willkürlich einen Constraint ignorieren.

Mögliche Reaktionen:

```text
Reject
Degrade
Fallback
Request User Decision
Escalate
```

## Unknown State

Kann ein Hard Constraint nicht zuverlässig geprüft werden:

```text
Constraint State = Unknown
```

gilt:

```text
Unknown ≠ Satisfied
```

Für sicherheitskritische autonome Aktionen muss ein unbekannter Hard Constraint die Aktion blockieren.

## Constraint Provenance

Constraints sollen nachvollziehbar machen, woher sie stammen.

```text
System Policy
User Policy
Execution Contract
Security Policy
Trust Policy
Sovereignty Policy
Resource Manager
```

Damit kann erklärt werden, warum eine autonome Aktion erlaubt oder verhindert wurde.

## Prediction

Predictions dürfen Constraints nicht verändern.

```text
Prediction
    ↓
Optimization Input

Constraint
    ↓
Decision Boundary
```

Auch eine sehr hohe Prediction Confidence darf keinen Hard Constraint überschreiben.

## Policy Learning

Policy Learning darf:

```text
Soft Weights
Thresholds
Preferences
Optimization Strategies
```

anpassen.

Es darf Hard Constraints weder entfernen noch abschwächen.

## Distributed Autonomy

Bei verteilten Aktionen müssen lokale und entfernte Constraints berücksichtigt werden.

```text
Local Constraints
       ∩
Remote Constraints
       ∩
Execution Constraints
       ↓
Allowed Distributed Action
```

Ein Remote Node darf lokale Constraints nicht umgehen.

## Safe Fallback

Existiert keine zulässige autonome Aktion:

```text
No Valid Action
      ↓
Safe Fallback
      ↓
Escalation if required
```

NovaOS darf nicht automatisch die Constraints lockern.

## Security

Constraint-Konfiguration benötigt entsprechende Authority.

Beispiele:

```text
ConstraintRead
ConstraintCreate
ConstraintModify
ConstraintRemove
ConstraintOverride
```

Das Recht, einen Constraint zu lesen, impliziert nicht das Recht, ihn zu verändern.

## Observability

Autorisierte Komponenten sollen beobachten können:

```text
ConstraintID
Constraint Type
Scope
State
Source
Priority
Validity
Affected Decision
Violation
Reason Code
Fallback
Provenance
```

Decision Observability soll darstellen können, welche Constraints eine Entscheidung begrenzt oder verhindert haben.

## Normative Anforderungen

1. NovaOS MUSS autonome Aktionen durch explizite Constraints begrenzen.
2. Hard Constraints DÜRFEN NICHT durch adaptive Optimierung überschrieben werden.
3. Constraint-Erfüllung DARF NICHT mit Authority gleichgesetzt werden.
4. Jede autonome Aktion MUSS sowohl Authority als auch relevante Constraints erfüllen.
5. Hard und Soft Constraints MÜSSEN unterscheidbar sein.
6. Constraints MÜSSEN einen definierten Scope besitzen.
7. Mehrere Constraints MÜSSEN zu einem effektiven Constraint-Set kombinierbar sein.
8. Widersprüchliche Hard Constraints DÜRFEN NICHT willkürlich aufgelöst werden.
9. `Unknown` DARF bei sicherheitskritischen Hard Constraints NICHT als erfüllt behandelt werden.
10. Execution Contracts MÜSSEN zusätzliche Autonomy Constraints liefern können.
11. Explizite User Constraints MÜSSEN innerhalb ihrer Authority berücksichtigt werden.
12. Dynamische Constraints MÜSSEN während relevanter Operationen revalidierbar sein.
13. Constraint-Verletzungen MÜSSEN Cancel, Pause, Degrade oder Rollback auslösen können.
14. Predictions DÜRFEN Constraints NICHT verändern.
15. Policy Learning DARF Hard Constraints NICHT entfernen oder abschwächen.
16. Distributed Autonomy MUSS lokale und entfernte Constraints respektieren.
17. Wenn keine gültige autonome Aktion existiert, MUSS ein sicherer Fallback oder eine Eskalation erfolgen.
18. Constraint-Zustand, Herkunft und Auswirkungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-AUTONOMY-POLICY-0001`
- `NPSPEC-ADAPTIVE-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-POLICYLEARNING-0001`
- `NPSPEC-ADAPTIVE-USEROVERRIDE-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-RESOURCE-GUARANTEE-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-OBSERVABILITY-INTROSPECTION-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `ADR-ARCH-0094`

## Ergebnis

```text
Possible Actions
       ↓
Authority
       ↓
Hard Constraints
       ↓
User Constraints
       ↓
Autonomy Policy
       ↓
Allowed Action Space
       ↓
Adaptive Optimization
       ↓
Autonomous Action
```

NovaOS erhält damit eine verbindliche Constraint-Schicht für autonome Systeme. Autonome und adaptive Komponenten können innerhalb klar definierter Grenzen selbstständig handeln, ohne Safety, Security, Authority, Trust, Sovereignty, Ressourcenlimits oder explizite Nutzerentscheidungen überschreiben zu können.