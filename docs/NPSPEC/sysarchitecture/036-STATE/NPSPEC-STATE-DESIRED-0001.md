# NPSPEC-STATE-DESIRED-0001 – Nova Desired State

## Status

Angenommen

## Kategorie

State / Desired State / Reconciliation

## Zweck

NovaOS definiert Desired State als deklarative Beschreibung des Zustands, den ein System, Subsystem, Dienst, Objekt oder eine Ressource erreichen und erhalten soll.

```text
Desired State
     ↓
Reconciliation
     ↓
Actual State
```

Komponenten beschreiben damit primär das gewünschte Ergebnis statt eine feste Folge imperativer Einzelschritte.

## Grundprinzipien

```text
Desired State ≠ Actual State
Desired State ≠ Command
Desired State ≠ Execution Plan
Desired State ≠ Authority
Desired State Change ≠ Applied Change
Configured ≠ Reconciled
Reconciled ≠ Verified
```

## Desired State Model

```text
DesiredState
├── StateID
├── StateType
├── OwnerID
├── Version
├── DesiredValue
├── Constraints
└── LifecycleState
```

Optional:

```text
Priority
Dependencies
ExecutionContractID
TransactionPolicy
Deadline
SecurityRequirements
TrustRequirements
SovereigntyRequirements
ProvenanceID
```

## Lifecycle

```text
Declared
   ↓
Validated
   ↓
Pending
   ↓
Reconciling
   ↓
Satisfied
```

Fehlerzustände:

```text
Blocked
Conflict
Failed
Unknown
```

## Deklaratives Modell

Beispiel:

```text
Desired:
Service = Running
```

NovaOS entscheidet anhand des aktuellen Zustands, welche Aktionen erforderlich sind.

```text
Actual = Stopped
      ↓
Difference
      ↓
Start Service
```

Der Desired State beschreibt nicht zwingend `Start Service`, sondern das Ziel `Running`.

## Reconciliation

Der grundlegende Ablauf lautet:

```text
Observe Actual State
        ↓
Compare with Desired State
        ↓
Calculate Difference
        ↓
Plan Transition
        ↓
Validate
        ↓
Execute
        ↓
Verify
```

Besteht weiterhin eine Abweichung, kann der Zyklus erneut ausgeführt werden.

## State Satisfaction

Ein Desired State ist erfüllt, wenn der relevante Actual State die definierten Bedingungen erfüllt.

```text
Desired State
      ≈
Actual State
      ↓
Satisfied
```

Dabei kann die Definition von Gleichheit je nach State Type variieren.

Beispiele:

```text
Exact Match
Allowed Range
Minimum Requirement
Semantic Equivalence
Policy-defined Match
```

## Versioning

Desired State muss versionierbar sein.

```text
Desired v12
    ↓
Update
    ↓
Desired v13
```

Ein laufender Reconciliation-Prozess muss erkennen können, wenn während seiner Arbeit eine neuere Version veröffentlicht wurde.

```text
Working on v12
      ↓
v13 appears
      ↓
Revalidate / Replan
```

## Ownership

Jeder autoritative Desired State benötigt einen definierten Owner.

```text
Owner
  ↓
Desired State
```

Mehrere Komponenten dürfen Wünsche oder Vorschläge liefern, aber konkurrierende autoritative Änderungen benötigen eine definierte Arbitration Policy.

## Constraints

Desired State kann harte Bedingungen enthalten:

```text
Security
Capabilities
Trust
Sovereignty
Resources
Deadline
Dependencies
```

Optimierung darf diese Bedingungen nicht verletzen.

## Capability Security

Desired State erzeugt keine Authority.

```text
Desired:
Device = Configured

        ≠

Permission to configure Device
```

Die tatsächlich ausgeführten Aktionen benötigen weiterhin die erforderlichen Capabilities.

## Dependencies

Desired States können voneinander abhängen.

```text
Desired A
   ↓
requires
   ↓
Desired B
```

NovaOS muss Abhängigkeiten erkennen und zyklische oder nicht erfüllbare Abhängigkeiten behandeln können.

## Transactions

Zusammengehörige Desired-State-Änderungen können transaktional angewendet werden.

```text
Desired Changes
      ↓
Transaction
      ↓
Validate
      ↓
Apply
      ↓
Verify
```

Ein teilweise angewandter Zustand darf nicht automatisch als erfüllt gelten.

## Conflict Handling

Konflikte können entstehen durch:

```text
Concurrent Updates
Policy Changes
Capability Revocation
Resource Limits
Dependency Changes
Provider Changes
```

Konflikte müssen explizit erkannt und nach definierter Policy behandelt werden.

## Failure und Recovery

Kann Desired State nicht erreicht werden:

```text
Reconciliation Failure
        ↓
Retry / Alternative Plan
        ↓
Rollback / Compensation
        ↓
Degraded State / Recovery
```

Endlose Reconciliation-Schleifen müssen durch Limits, Backoff und Eskalation verhindert werden.

## Distributed State

Desired State kann auch verteilte Komponenten betreffen.

```text
Desired Cluster State
        ↓
Node A
Node B
Node C
```

Dabei darf NovaOS keine universelle sofortige Konsistenz voraussetzen.

Ein nicht erreichbarer Node bedeutet nicht automatisch, dass dessen Zustand falsch ist.

## User Decisions

Explizite Benutzerentscheidungen besitzen Vorrang vor adaptiver Optimierung.

```text
Hard System Constraints
        ↓
Explicit User Decision
        ↓
Soft Preferences
        ↓
Adaptive Optimization
```

Autonome Mechanismen dürfen den Desired State nicht außerhalb ihrer erlaubten Authority verändern.

## Self-Healing

Self-Healing kann Desired State zur Wiederherstellung verwenden.

```text
Failure
   ↓
Actual State deviates
   ↓
Desired State remains
   ↓
Reconciliation
   ↓
Recovery
```

Damit beschreibt Desired State auch den Zustand, den NovaOS nach einem Fehler wiederherstellen soll.

## Provenance

Kritische Änderungen sollen nachvollziehbar sein:

```text
Who changed it
What changed
Previous Version
New Version
Reason
Source
Timestamp
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
StateID
Desired Value
Version
Owner
Lifecycle State
Actual State
Difference
Dependencies
Constraints
Last Reconciliation
Blocking Reason
```

## Normative Anforderungen

1. NovaOS MUSS Desired State getrennt von Actual State modellieren.
2. Desired State MUSS deklarativ beschreibbar sein.
3. Desired State DARF NICHT automatisch als ausgeführter Zustand interpretiert werden.
4. Desired State MUSS eindeutig identifizierbar und versionierbar sein.
5. Autoritativer Desired State MUSS einen definierten Owner besitzen.
6. Reconciliation MUSS Desired und Actual State vergleichen können.
7. Zustandsabweichungen MÜSSEN explizit erkennbar sein.
8. State Satisfaction MUSS je State Type definierbar sein.
9. Neue Desired-State-Versionen MÜSSEN laufende Reconciliation beeinflussen können.
10. Desired State DARF keine Authority erzeugen.
11. Ausführende Aktionen MÜSSEN erforderliche Capabilities validieren.
12. Abhängigkeiten zwischen Desired States MÜSSEN darstellbar sein.
13. Konflikte MÜSSEN explizit behandelbar sein.
14. Zusammengehörige Änderungen SOLLEN Transactions verwenden.
15. Teilweise Anwendung DARF NICHT automatisch als erfüllt gelten.
16. Harte Constraints DÜRFEN durch Reconciliation NICHT verletzt werden.
17. Reconciliation MUSS Failure- und Recovery-Pfade besitzen.
18. Endlose Reconciliation-Schleifen MÜSSEN begrenzt werden.
19. Distributed Desired State DARF keine universelle sofortige Konsistenz voraussetzen.
20. Explizite User Decisions DÜRFEN durch adaptive Optimierung NICHT überschrieben werden.
21. Self-Healing MUSS Desired State als Recovery-Ziel verwenden können.
22. Kritische Desired-State-Änderungen SOLLEN Provenance besitzen.
23. Desired State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STATE-GLOBAL-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-RESILIENCE-SELFHEALING-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-0001`
- `ADR-ARCH-0154`

## Ergebnis

```text
Declare Desired State
        ↓
Validate
        ↓
Observe Actual State
        ↓
Compare
        ↓
Plan Difference
        ↓
Authorize
        ↓
Execute
        ↓
Verify
        ↓
Satisfied?
├── Yes → Maintain
└── No  → Reconcile Again / Recover
```

NovaOS erhält damit ein deklaratives Desired-State-Modell, bei dem Komponenten den gewünschten Systemzustand beschreiben und NovaOS kontrolliert, transaktional und überprüfbar dafür sorgt, dass der tatsächliche Zustand diesem Ziel entspricht.