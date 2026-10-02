# NPSPEC-STATE-TRANSACTIONAL-0001 – Nova Transactional State

## Status

Angenommen

## Kategorie

State / Transaction / Consistency

## Zweck

NovaOS definiert Transactional State als Zustandsmodell, bei dem zusammengehörige Änderungen kontrolliert vorbereitet, validiert und als definierte Einheit veröffentlicht werden.

```text
Current State
     ↓
Begin Transaction
     ↓
Stage Changes
     ↓
Validate
     ↓
Prepare
     ↓
Commit
     ↓
Verify
     ↓
New Actual State
```

Damit werden ungültige oder nur teilweise sichtbare Zwischenzustände vermieden, wo dies technisch und semantisch möglich ist.

## Grundprinzipien

```text
Transactional State ≠ Database State
Transaction ≠ Lock
Prepared ≠ Committed
Committed ≠ Verified
Abort ≠ Rollback Completed
Rollback ≠ Compensation
Transaction Success ≠ Desired State Satisfied
Atomicity ≠ Global Atomicity
```

NovaOS setzt keine universelle globale ACID-Transaktion über das gesamte System voraus.

## Transactional State Model

```text
TransactionalState
├── TransactionID
├── Scope
├── BaseVersions
├── StagedChanges
├── Dependencies
├── Constraints
└── State
```

Optional:

```text
OwnerID
ExecutionContractID
ResourceBudget
Deadline
IsolationPolicy
RollbackPolicy
CompensationPolicy
RecoveryPolicy
ProvenanceID
```

## Transaction States

```text
Created
Active
Validating
Preparing
Prepared
Committing
Committed
Verifying
Completed
Aborting
Aborted
Failed
Unknown
```

## Staged State

Änderungen werden zunächst als nicht veröffentlichter State vorbereitet.

```text
Actual State v10
       ↓
Transaction
       ↓
Staged State
├── Change A
├── Change B
└── Change C
```

Andere Komponenten dürfen Staged State nicht automatisch als aktuellen Actual State behandeln.

## Base Versions

Eine Transaction muss relevante Ausgangsversionen erfassen können.

```text
State A:v10
State B:v21
      ↓
Transaction
```

Vor Commit kann geprüft werden:

```text
Current Version == Expected Version?
```

Dadurch werden konkurrierende Änderungen erkannt.

## Validation

Vor Prepare oder Commit müssen relevante Bedingungen geprüft werden:

```text
State Versions
State Machine Rules
Capabilities
Security Policy
Trust
Sovereignty
Dependencies
Resources
Execution Contract
```

```text
Valid State Change ≠ Authorized State Change
```

## Prepare

Die Prepare-Phase stellt sicher, dass eine Änderung voraussichtlich durchgeführt werden kann.

Sie kann beispielsweise:

```text
Resources reserve
Dependencies validate
State versions lock/check
Storage prepare
Provider prepare
```

Der vorbereitete Zustand ist noch nicht automatisch sichtbar.

## Commit

Commit definiert den logischen Veröffentlichungspunkt.

```text
Prepared State
      ↓
Commit Point
      ↓
New Actual State
```

Der Commit Point muss für den jeweiligen Transaction Scope eindeutig definiert sein.

## Atomic Visibility

Innerhalb eines unterstützten Scopes sollen zusammengehörige Änderungen atomar sichtbar werden.

```text
Before Commit:
A:v1
B:v5

After Commit:
A:v2
B:v6
```

Ein Beobachter soll nicht unbeabsichtigt einen logisch ungültigen Mischzustand sehen.

## Isolation

Transactional State kann unterschiedliche Isolation Policies besitzen.

Beispiele:

```text
Version-based
Snapshot-based
Serialized
Optimistic
Subsystem-defined
```

Die tatsächlich garantierte Isolation muss explizit sein.

## Conflict Detection

Konflikte können entstehen durch:

```text
Concurrent State Update
Capability Revocation
Dependency Change
Provider Change
Resource Loss
Policy Change
```

Mögliche Reaktionen:

```text
Revalidate
Retry
Replan
Abort
Serialize
Merge
```

## Rollback

Vor Commit können staged Änderungen grundsätzlich verworfen werden.

Nach bereits sichtbaren Änderungen kann ein expliziter Rollback erforderlich sein.

```text
Rollback
   ↓
Restore State Content
   ↓
Create New Version
   ↓
Verify
```

State History wird dabei nicht zurückgeschrieben.

## Compensation

Irreversible externe Effekte können nicht durch reinen State Rollback entfernt werden.

```text
External Effect
      ↓
Compensation
```

```text
Compensation ≠ Exact Reversal
```

## Unknown Transaction State

Nach Crash, Timeout oder Kommunikationsverlust kann der Commit-Status unbekannt sein.

```text
Transaction State = Unknown
```

NovaOS darf daraus nicht automatisch ableiten:

```text
Committed
Aborted
Failed
```

Transaction Log und Recovery müssen den Zustand rekonstruieren.

## Desired State Integration

Transactional State kann Reconciliation absichern.

```text
Desired State
      ↓
Reconciliation Plan
      ↓
Transaction
      ↓
Commit
      ↓
Actual State
      ↓
Verify Desired State
```

Ein erfolgreicher Commit bedeutet nicht automatisch, dass der gesamte Desired State erfüllt wurde.

## Distributed State

Verteilte Transactions können nur die Garantien ihres jeweiligen Protokolls bieten.

Mögliche Mechanismen:

```text
Two-Phase Commit
Consensus
Versioning
Idempotency
Compensation
Saga-like Coordination
```

```text
Local Commit ≠ Global Commit
No Response ≠ Abort
```

NovaOS verlangt keine universelle globale Transaction.

## Security

Transactions erzeugen keine Authority.

Capabilities müssen entsprechend der Operation validiert werden.

Bei sicherheitskritischen Änderungen kann vor Commit eine erneute Capability-Prüfung erforderlich sein.

Historische oder staged Authority darf aktuelle Revocation nicht überschreiben.

## Resource Management

Reservierte Ressourcen müssen begrenzt sein.

```text
Transaction
├── Memory Reservation
├── IO Reservation
├── Storage Reservation
└── Time Budget
```

Bei Abort oder Failure müssen nicht mehr benötigte Reservierungen freigegeben werden.

## Recovery

Persistente Transactions benötigen Recovery-Informationen.

```text
Crash
  ↓
Transaction Log
  ↓
Determine State
  ↓
Redo / Rollback / Compensation
  ↓
Verify
```

## Verification

Nach Commit muss der Actual State überprüfbar sein.

```text
Commit
   ↓
Observe Actual State
   ↓
Validate Postconditions
   ↓
Verified
```

```text
Committed ≠ Verified
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
TransactionID
Scope
Base Versions
Staged Changes
Transaction State
Isolation Policy
Conflicts
Dependencies
Commit State
Verification State
Recovery State
```

## Normative Anforderungen

1. NovaOS MUSS transactional State Changes unterstützen können.
2. Transactional State MUSS einen expliziten Scope besitzen.
3. Staged State MUSS von Actual State unterscheidbar bleiben.
4. Prepared State DARF NICHT als committed State interpretiert werden.
5. Transactions MÜSSEN relevante Base Versions erfassen können.
6. Concurrent Changes MÜSSEN vor Commit erkannt werden können.
7. State Changes MÜSSEN vor kritischem Commit validierbar sein.
8. Transactions DÜRFEN keine Authority erzeugen.
9. Sicherheitskritische Authority MUSS vor Commit revalidierbar sein.
10. Commit Points MÜSSEN innerhalb ihres Scopes eindeutig definiert sein.
11. Zusammengehörige Änderungen SOLLEN innerhalb unterstützter Scopes atomar sichtbar werden.
12. Isolation Guarantees MÜSSEN explizit sein.
13. Konflikte MÜSSEN explizit behandelbar sein.
14. Rollback DARF State History NICHT zurückschreiben.
15. Irreversible Effekte MÜSSEN Compensation verwenden können.
16. Unknown Transaction State MUSS explizit repräsentierbar sein.
17. Unknown DARF NICHT automatisch als Commit oder Abort interpretiert werden.
18. Transactional State MUSS mit Desired-State-Reconciliation integrierbar sein.
19. Distributed Transactions DÜRFEN keine universelle globale Atomicity voraussetzen.
20. Local Commit DARF NICHT automatisch als Global Commit interpretiert werden.
21. Resource Reservations MÜSSEN begrenzt und freigebbar sein.
22. Persistente Transactions MÜSSEN Recovery unterstützen können.
23. Nach Commit MUSS der Actual State verifizierbar sein.
24. Transactional State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STATE-GLOBAL-0001`
- `NPSPEC-STATE-DESIRED-0001`
- `NPSPEC-STATE-ACTUAL-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-RECONCILIATION-0001`
- `NPSPEC-STATE-SNAPSHOT-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-STATE-HISTORY-0001`
- `NPSPEC-STATE-ROLLBACK-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-LOG-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-TRANSACTION-BARRIER-0001`
- `ADR-ARCH-0162`

## Ergebnis

```text
Current State
      ↓
Begin Transaction
      ↓
Capture Base Versions
      ↓
Stage Changes
      ↓
Validate
      ↓
Prepare
      ↓
Revalidate Authority + State
      ↓
Commit
      ↓
Publish New State Versions
      ↓
Verify Actual State
      ↓
Complete / Recover
```

NovaOS erhält damit ein einheitliches Transactional-State-Modell, das zusammengehörige Zustandsänderungen kontrolliert vorbereitet, konfliktgesichert veröffentlicht und anschließend verifiziert, ohne globale Atomicity vorauszusetzen oder Commit mit erfolgreicher Zustandsverifikation gleichzusetzen.