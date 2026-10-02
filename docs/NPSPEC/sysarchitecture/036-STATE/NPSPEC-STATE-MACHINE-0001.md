# NPSPEC-STATE-MACHINE-0001 – Nova State Machine

## Status

Angenommen

## Kategorie

State / State Machine / System Architecture

## Zweck

NovaOS definiert ein einheitliches State-Machine-Modell für Komponenten, deren Verhalten durch explizite Zustände und kontrollierte Zustandsübergänge beschrieben wird.

```text
Current State
     ↓
Event / Condition
     ↓
Validate Transition
     ↓
Execute Transition
     ↓
New State
     ↓
Verify
```

State Machines bilden damit eine determinierbare Grundlage für Kernel-Komponenten, Dienste, Treiber, Ressourcen, Transaktionen und Recovery-Prozesse.

## Grundprinzipien

```text
State ≠ Event
State ≠ Action
Event ≠ Transition
Transition Request ≠ Transition Success
Valid Transition ≠ Authorized Transition
Committed State ≠ Verified State
Unknown State ≠ Valid State
```

## State Machine Model

```text
StateMachine
├── StateMachineID
├── StateMachineType
├── Version
├── OwnerID
├── CurrentState
├── States
├── Transitions
└── Invariants
```

Optional:

```text
DesiredStateID
TransactionID
ExecutionContractID
CapabilityRequirements
TimeoutPolicy
RecoveryPolicy
ProvenanceID
```

## State Definition

Ein Zustand beschreibt eine klar definierte Systembedingung.

Beispiel:

```text
ServiceState
├── Stopped
├── Starting
├── Running
├── Stopping
├── Failed
└── Unknown
```

Jeder Zustand kann eigene Invarianten besitzen.

```text
Running:
Process exists
Resources allocated
Required dependencies available
```

## Transition Model

```text
Transition
├── TransitionID
├── FromState
├── ToState
├── Trigger
├── Preconditions
├── Actions
└── Postconditions
```

Optional:

```text
RequiredCapabilities
Deadline
RollbackPolicy
CompensationPolicy
```

## Transition Validation

Vor einem Zustandswechsel muss geprüft werden:

```text
Current State
Trigger
Preconditions
Capabilities
Dependencies
Resources
Security
Trust
Constraints
```

Nur ein gültiger und autorisierter Übergang darf ausgeführt werden.

## Illegal Transitions

Nicht definierte Übergänge müssen abgelehnt werden.

```text
Stopped → Running
```

kann beispielsweise nur erlaubt sein über:

```text
Stopped
   ↓
Starting
   ↓
Running
```

Implementierungen dürfen keine Zustände überspringen, sofern dies nicht ausdrücklich spezifiziert ist.

## Atomic State Transition

Kritische Zustandswechsel sollen atomar sichtbar gemacht werden.

```text
Old State
   ↓
Prepare
   ↓
Transition
   ↓
Commit
   ↓
New State
```

Andere Komponenten dürfen keinen ungültigen Zwischenzustand als regulären Zustand interpretieren.

## Transactions

Komplexe State Transitions können Nova Transactions verwenden.

```text
Transition
   ↓
Begin Transaction
   ↓
Validate
   ↓
Prepare
   ↓
Commit
   ↓
Update State
   ↓
Verify
```

```text
Prepared ≠ New State
```

## Concurrency

Mehrere konkurrierende Transition Requests müssen koordiniert werden.

```text
State v8
├── Request A
└── Request B
```

Mögliche Verfahren:

```text
Compare-and-Swap
Version Check
Serialization
Lock
Transaction
Conflict Resolution
```

Ein Transition Plan für eine veraltete State-Version muss revalidiert werden.

## Events

Events können Transitionen auslösen.

```text
Event
  ↓
State Machine
  ↓
Transition
```

Ein Event erzwingt jedoch keinen Zustandswechsel.

```text
Event Received ≠ Transition Allowed
```

## Guards

Transitions können Guards besitzen.

```text
CurrentState == Ready
AND CapabilityValid
AND ResourceAvailable
AND DependencyHealthy
```

Nur wenn alle verpflichtenden Guards erfüllt sind, darf die Transition fortgesetzt werden.

## Actions

Eine Transition kann Aktionen ausführen.

```text
Exit Actions
     ↓
Transition Actions
     ↓
Entry Actions
```

Side Effects müssen entsprechend dem API- und Transaction-Contract behandelt werden.

## Hierarchical State Machines

Komplexe Komponenten dürfen hierarchische States verwenden.

```text
Running
├── Active
├── Idle
└── Degraded
```

Gemeinsame Invarianten können auf übergeordneten States definiert werden.

## Timeout

Transitions können zeitliche Grenzen besitzen.

```text
Starting
   ↓
Deadline exceeded
   ↓
Recovery / Failed / Unknown
```

```text
Timeout ≠ Transition Never Executed
```

Der tatsächliche Zustand muss anschließend ermittelt werden.

## Failure

Fehler während einer Transition müssen explizit behandelt werden.

```text
Transition Failure
      ↓
Determine Actual State
      ↓
Rollback / Compensation / Recovery
      ↓
Verify
```

Ein fehlgeschlagener Übergang bedeutet nicht automatisch, dass der vorherige Zustand weiterhin gilt.

## Unknown State

Kann der tatsächliche Zustand nicht bestimmt werden:

```text
CurrentState = Unknown
```

Unknown muss explizit behandelt werden.

```text
Unknown ≠ Previous State
Unknown ≠ Failed
Unknown ≠ Healthy
```

## Recovery

State Machines müssen definierte Recovery-Übergänge unterstützen können.

```text
Failed
   ↓
Recovering
   ↓
Ready
```

oder:

```text
Unknown
   ↓
Reconstruct State
   ↓
Verify
   ↓
Known State
```

## Desired und Actual State

State Machines integrieren sich in das Nova State Model.

```text
Desired State
      ↓
State Machine
      ↓
Transitions
      ↓
Actual State
```

Die State Machine bestimmt zulässige Übergänge, während Reconciliation entscheidet, welche Übergänge zum Desired State erforderlich sind.

## Determinismus

Deterministische State Machines müssen bei identischem:

```text
Initial State
Input
Event Ordering
Constraints
```

denselben definierten Transition-Pfad erzeugen können.

Externe nichtdeterministische Einflüsse müssen für Replay erfassbar sein.

## Security

State Transitions erzeugen keine Authority.

Jede sicherheitsrelevante Transition muss die erforderlichen Capabilities prüfen.

Capability Revocation während einer länger laufenden Transition kann eine Revalidation erforderlich machen.

## Provenance

Kritische Transitionen sollen erfassen:

```text
StateMachineID
TransitionID
Previous State
New State
Trigger
Actor
Timestamp
TransactionID
Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
StateMachineID
Version
CurrentState
AvailableTransitions
ActiveTransition
Invariants
Guards
LastTransition
FailureState
RecoveryState
```

## Normative Anforderungen

1. NovaOS MUSS explizite State Machines unterstützen können.
2. States und Transitions MÜSSEN eindeutig identifizierbar sein.
3. Nicht definierte State Transitions MÜSSEN abgelehnt werden.
4. Transitions MÜSSEN Preconditions und Postconditions definieren können.
5. Verpflichtende Guards MÜSSEN vor einer Transition erfüllt sein.
6. Valid Transition DARF NICHT automatisch als autorisierte Transition gelten.
7. Sicherheitsrelevante Transitions MÜSSEN erforderliche Capabilities validieren.
8. Kritische State Transitions SOLLEN atomar sichtbar werden.
9. Komplexe Transitions SOLLEN mit Nova Transactions integrierbar sein.
10. Concurrent Transition Requests MÜSSEN koordiniert werden.
11. Veraltete Transition Plans MÜSSEN revalidierbar sein.
12. Events DÜRFEN keine ungültigen Transitionen erzwingen.
13. Side Effects MÜSSEN explizit behandelbar sein.
14. Hierarchische State Machines MÜSSEN unterstützt werden können.
15. Transition Timeouts MÜSSEN explizit behandelbar sein.
16. Timeout DARF NICHT automatisch als Nichtausführung interpretiert werden.
17. Nach Transition Failure MUSS der tatsächliche Zustand bestimmbar oder als Unknown markierbar sein.
18. Unknown State MUSS explizit repräsentierbar sein.
19. Recovery Transitions MÜSSEN definierbar sein.
20. State Machines MÜSSEN mit Desired und Actual State integrierbar sein.
21. Deterministische Modi MÜSSEN reproduzierbare Transitionen unterstützen können.
22. Capability Revocation MUSS bei relevanten laufenden Transitionen berücksichtigt werden.
23. Kritische Transitionen SOLLEN Provenance besitzen.
24. State Machines MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STATE-GLOBAL-0001`
- `NPSPEC-STATE-DESIRED-0001`
- `NPSPEC-STATE-ACTUAL-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-CONCURRENCY-STRUCTURED-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0156`

## Ergebnis

```text
Current State
      ↓
Trigger / Event
      ↓
Validate Transition
      ↓
Guards + Capabilities + Constraints
      ↓
Prepare
      ↓
Execute
      ↓
Commit State
      ↓
Verify
      ↓
New Actual State
```

NovaOS erhält damit ein einheitliches State-Machine-Modell, das Zustandsänderungen als explizite, validierte, autorisierte und überprüfbare Transitionen behandelt und damit eine gemeinsame Grundlage für Systemsteuerung, Reconciliation, Transaktionen und Self-Healing bildet.