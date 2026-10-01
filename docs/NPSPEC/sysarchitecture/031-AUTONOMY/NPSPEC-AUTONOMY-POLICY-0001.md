# NPSPEC-AUTONOMY-POLICY-0001 – Nova Autonomy Policy

## Status

Angenommen

## Kategorie

Autonomy / Policy / System Control

## Zweck

NovaOS definiert eine systemweite Autonomy Policy, die festlegt, welche Entscheidungen das System selbstständig treffen darf, wann eine Nutzerentscheidung erforderlich ist und welche Grenzen autonome Komponenten niemals überschreiten dürfen.

```text
Intent / Event
      ↓
Autonomy Policy
      ↓
Allowed Autonomy Level
      ↓
Decision / User Confirmation
      ↓
Execution
```

## Grundprinzipien

```text
Autonomy ≠ Authority
Autonomy ≠ Unlimited Control
Prediction ≠ Permission
Automation ≠ Consent
Optimization ≠ User Intent
Learned Policy ≠ Hard Policy

Autonomous Action
    ⊆
Authorized Action
```

Eine autonome Komponente darf nur innerhalb bereits vorhandener Authority handeln.

## Autonomy Policy Model

```text
AutonomyPolicy
├── PolicyID
├── Scope
├── AutonomyLevel
├── AllowedActions
├── Constraints
└── State
```

Optional:

```text
IdentityID
CapabilityType
ExecutionType
ResourceType
RiskClass
ConfirmationPolicy
Expiration
PolicyVersion
ProvenanceID
```

## Autonomy Levels

NovaOS definiert mindestens:

```text
Level 0 – Manual
Level 1 – Suggest
Level 2 – Confirm
Level 3 – Limited Autonomous
Level 4 – Autonomous
```

### Level 0 – Manual

```text
System observes
User decides
System executes
```

### Level 1 – Suggest

```text
System proposes
User chooses
```

### Level 2 – Confirm

```text
System prepares decision
User confirms
System executes
```

### Level 3 – Limited Autonomous

Das System darf definierte reversible oder risikoarme Aktionen selbstständig durchführen.

### Level 4 – Autonomous

Das System darf innerhalb eines ausdrücklich definierten Bereichs selbstständig beobachten, entscheiden, handeln und verifizieren.

```text
Observe
   ↓
Decide
   ↓
Execute
   ↓
Verify
   ↓
Feedback
```

Auch Level 4 bleibt durch Hard Constraints und Capabilities begrenzt.

## Scope

Autonomie kann begrenzt werden auf:

```text
Operation
Execution
Application
Capability
Object
Resource
Device
Service
Session
User
System
```

NovaOS soll den kleinsten notwendigen Autonomy Scope verwenden.

## Risk Classification

Aktionen können nach Risiko klassifiziert werden:

```text
Low
Moderate
High
Critical
```

Beispiele:

```text
Cache Eviction          → Low
Provider Migration      → Moderate
Persistent Data Change  → High
Security Policy Change  → Critical
```

Die konkrete Klassifikation wird durch Policy definiert.

## Reversibility

Reversible Aktionen können höhere Autonomie erlauben als irreversible Aktionen.

```text
Reversible
   ↓
Higher Autonomous Freedom

Irreversible
   ↓
Stronger Confirmation
```

Transaktionen, Snapshots und Undo können den sicheren autonomen Handlungsspielraum erweitern.

## Decision Hierarchy

Autonome Entscheidungen folgen:

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
Soft Preferences
  ↓
Adaptive Optimization
```

## User Override

Explizite User Overrides besitzen Vorrang vor autonomen Soft-Entscheidungen.

```text
Autonomous Decision
        ↓
User Override
        ↓
Constraint Validation
        ↓
Effective Decision
```

Adaptive oder autonome Komponenten dürfen einen gültigen Override nicht selbstständig rückgängig machen.

## Capability Integration

Autonomy Policy erzeugt keine Capabilities.

```text
Autonomy Permission
       +
Required Capability
       ↓
Action Allowed
```

Fehlt die notwendige Capability, darf eine Aktion unabhängig vom Autonomy Level nicht ausgeführt werden.

## Adaptive Integration

Adaptive Systeme dürfen innerhalb der Autonomy Policy selbstständig:

```text
Predict
Optimize
Schedule
Cache
Prefetch
Preload
Place
Migrate
Adjust Resources
```

Die Autonomy Policy definiert den zulässigen Handlungsspielraum.

## Policy Learning

Policy Learning darf adaptive Parameter verändern, aber nicht eigenständig den erlaubten Autonomy Level erhöhen.

```text
Learned Policy
      ≠
Expanded Autonomy
```

Eine Erweiterung autonomer Rechte benötigt eine entsprechend autorisierte Policy-Änderung.

## Autonomous Recovery

Self-Healing kann eigene Autonomy Policies besitzen.

Beispiel:

```text
Failure Detected
      ↓
Low-risk Recovery
      ↓
Automatic Repair

High-risk Recovery
      ↓
User Confirmation
```

## Escalation

Kann eine autonome Komponente keine sichere Entscheidung treffen:

```text
Unknown State
Low Confidence
Policy Conflict
High Risk
Missing Authority
```

muss sie eskalieren.

```text
Autonomous Decision
      ↓ unable
Escalation
      ↓
User / Higher Authority
```

`Unknown` darf nicht automatisch als Zustimmung interpretiert werden.

## Safe Failure

Bei Ausfall der Autonomy Policy:

```text
Autonomy unavailable
        ↓
Restrictive Safe Mode
```

NovaOS darf nicht automatisch auf maximale Autonomie zurückfallen.

## Observability

Autonome Entscheidungen müssen nachvollziehbar sein.

Autorisierte Komponenten sollen beobachten können:

```text
PolicyID
Autonomy Level
Scope
DecisionID
Action
Risk Class
Required Capability
Constraints
Reason
User Override
Execution Result
Policy Version
Provenance
```

## Normative Anforderungen

1. NovaOS MUSS autonome Aktionen durch eine explizite Autonomy Policy begrenzen.
2. Autonomy DARF keine zusätzliche Authority erzeugen.
3. Jede autonome Aktion MUSS weiterhin die erforderlichen Capabilities besitzen.
4. Autonomy Levels MÜSSEN explizit darstellbar sein.
5. Autonomy Policies MÜSSEN einen definierten Scope besitzen.
6. NovaOS SOLL den kleinsten notwendigen Autonomy Scope verwenden.
7. Safety-, Security-, Trust-, Sovereignty- und Hard Constraints MÜSSEN Vorrang besitzen.
8. Explizite User Decisions MÜSSEN Vorrang vor autonomen Soft-Entscheidungen besitzen.
9. User Overrides DÜRFEN NICHT durch adaptive Komponenten selbstständig aufgehoben werden.
10. Risiko und Reversibilität SOLLEN bei Autonomieentscheidungen berücksichtigt werden.
11. Irreversible oder hochriskante Aktionen SOLLEN stärkere Autorisierung oder Bestätigung verlangen können.
12. Policy Learning DARF den eigenen Autonomy Level NICHT selbstständig erhöhen.
13. Fehlende Authority MUSS eine autonome Aktion verhindern.
14. Unsicherheit MUSS eine Eskalation auslösen können.
15. `Unknown` DARF NICHT als Zustimmung interpretiert werden.
16. Bei Ausfall der Autonomy Policy MUSS NovaOS auf einen restriktiven sicheren Zustand zurückfallen.
17. Autonomy Policies MÜSSEN versionierbar und kontrolliert änderbar sein.
18. Autonome Entscheidungen MÜSSEN autorisiert introspektierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-ADAPTIVE-PREDICTION-0001`
- `NPSPEC-ADAPTIVE-FEEDBACK-0001`
- `NPSPEC-ADAPTIVE-POLICYLEARNING-0001`
- `NPSPEC-ADAPTIVE-USEROVERRIDE-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-OBSERVABILITY-INTROSPECTION-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `ADR-ARCH-0093`

## Ergebnis

```text
System Intent
     ↓
Authority
     ↓
Autonomy Policy
     ↓
Risk + Constraints
     ↓
┌───────────────┬────────────────┐
│ Autonomous    │ User Decision  │
└───────┬───────┴───────┬────────┘
        ↓               ↓
           Execution
               ↓
            Verify
               ↓
           Feedback
```

NovaOS erhält damit eine klare Grenze zwischen automatischer Optimierung und autonomem Handeln. Das System kann innerhalb ausdrücklich definierter Bereiche selbstständig entscheiden und reagieren, während Authority, Hard Constraints und explizite Nutzerentscheidungen jederzeit die verbindliche Kontrolle behalten.