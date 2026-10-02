# NPSPEC-STATE-RECONCILIATION-0001 – Nova State Reconciliation

## Status

Angenommen

## Kategorie

State / Reconciliation / System Model

## Zweck

NovaOS definiert State Reconciliation als kontrollierten Prozess, der Desired State und Actual State vergleicht und zulässige Änderungen plant, ausführt und verifiziert.

```text
Desired State
      ↓
    Compare
      ↑
Actual State
      ↓
Difference
      ↓
Reconciliation
      ↓
Verified State
```

Reconciliation bildet die zentrale Verbindung zwischen deklarativer Systembeschreibung und tatsächlichem Systemzustand.

## Grundprinzipien

```text
Reconciliation ≠ Blind Enforcement
Reconciliation ≠ Command Execution
Desired State ≠ Authority
Difference ≠ Failure
Planned State ≠ Actual State
Executed ≠ Committed
Committed ≠ Verified
Unknown ≠ Satisfied
```

## Reconciliation Model

```text
Reconciliation
├── ReconciliationID
├── StateID
├── DesiredVersion
├── ActualVersion
├── Difference
├── Plan
├── Policy
└── State
```

Optional:

```text
ExecutionContractID
TransactionID
ResourceBudget
Deadline
RequiredCapabilities
RecoveryPolicy
ProvenanceID
```

## Zustände

```text
Created
Observing
Comparing
Planning
Validating
Executing
Verifying
Satisfied
Blocked
Failed
Cancelled
Unknown
```

## Reconciliation Loop

Der grundlegende Zyklus lautet:

```text
Observe
   ↓
Compare
   ↓
Plan
   ↓
Validate
   ↓
Execute
   ↓
Verify
   ↓
Observe Again
```

Der Zyklus endet, wenn der Desired State erfüllt ist oder eine definierte Abbruchbedingung erreicht wurde.

## Difference Detection

NovaOS bestimmt die Differenz zwischen Desired und Actual State.

```text
Desired
   ↓
Semantic Comparison
   ↑
Actual
```

Mögliche Ergebnisse:

```text
Satisfied
Drifted
Partially Satisfied
Blocked
Conflicting
Unknown
```

Die Bewertung muss zum jeweiligen State Type passen.

## Planning

Aus der Differenz wird ein Transition Plan erzeugt.

```text
Actual State A
      ↓
Transition 1
      ↓
State B
      ↓
Transition 2
      ↓
Desired State C
```

Der Plan muss die Regeln der jeweiligen State Machine respektieren.

## Validation

Vor der Ausführung müssen mindestens relevante:

```text
State Versions
Capabilities
Security Policy
Trust
Sovereignty
Dependencies
Resources
Execution Constraints
Transaction Requirements
```

validiert werden.

```text
Valid Plan ≠ Authorized Plan
```

## Execution

Der validierte Plan wird über definierte State Transitions ausgeführt.

Bei mehrteiligen Änderungen soll das Nova Transaction System verwendet werden.

```text
Plan
  ↓
Prepare
  ↓
Execute
  ↓
Commit
  ↓
Verify
```

## Concurrent State Changes

Actual oder Desired State kann sich während der Reconciliation ändern.

```text
Plan based on:
Desired v10
Actual v20

      ↓

Desired becomes v11
```

Der bestehende Plan muss dann abhängig von seiner Gültigkeit:

```text
Continue
Revalidate
Replan
Cancel
```

können.

## Idempotenz

Reconciliation-Aktionen sollen nach Möglichkeit idempotent sein.

```text
Apply Desired State
      ↓
Repeat
      ↓
Same Valid Result
```

Nicht idempotente Aktionen benötigen explizite Schutzmechanismen.

## Failure Handling

Fehler führen nicht automatisch zur sofortigen Wiederholung.

```text
Failure
   ↓
Classify
   ↓
Retry
Backoff
Alternative Plan
Rollback
Compensation
Recovery
Degraded Mode
```

Retry- und Recovery-Schleifen müssen begrenzt werden.

## Unknown State

Kann der tatsächliche Zustand nicht zuverlässig bestimmt werden:

```text
Actual State = Unknown
```

muss Reconciliation abhängig von Risiko und Policy:

```text
Reobserve
Diagnose
Recover
Enter Safe State
Require User Decision
```

können.

Unknown darf nicht als erfüllter Desired State behandelt werden.

## Security

Reconciliation besitzt keine implizite Authority.

Jede relevante Änderung benötigt die erforderlichen Capabilities.

```text
Desired State
      ≠
Permission to enforce State
```

Capability Revocation während einer Reconciliation muss berücksichtigt werden.

## Resource Economy

Reconciliation muss Ressourcenbudgets respektieren.

```text
CPU
Memory
IO
Network
Energy
Time
```

Ein System darf nicht durch aggressive Reconciliation selbst destabilisiert werden.

## Backoff und Stabilität

Bei wiederholten Abweichungen sollen Mechanismen wie:

```text
Backoff
Cooldown
Rate Limit
Hysteresis
Retry Budget
```

verwendet werden.

Damit werden Oszillation und Reconciliation Storms verhindert.

## Distributed Reconciliation

Verteilte Zustände benötigen ihr jeweiliges Consistency Model.

```text
Desired Cluster State
        ↓
Node A
Node B
Node C
```

```text
No Response ≠ Failure
Local State ≠ Global Truth
```

Eine universelle globale Transaktion oder sofortige Konsistenz wird nicht vorausgesetzt.

## Self-Healing

Self-Healing kann Reconciliation verwenden:

```text
Failure
   ↓
Actual State Drift
   ↓
Diagnose
   ↓
Desired State
   ↓
Reconciliation
   ↓
Repair
   ↓
Verify
```

Self-Healing darf dabei keine zusätzlichen Rechte erzeugen.

## User Decisions

Explizite Benutzerentscheidungen dürfen nicht durch adaptive Reconciliation überschrieben werden.

Priorität:

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

## Verification

Nach der Ausführung muss der Actual State erneut beobachtet werden.

```text
Execute
   ↓
Observe Actual
   ↓
Compare Desired
   ↓
Verified?
```

Der gemeldete Erfolg einer Operation reicht nicht aus.

## Provenance

Kritische Reconciliation-Vorgänge sollen erfassen:

```text
ReconciliationID
Desired Version
Actual Version
Detected Difference
Selected Plan
Executed Transitions
TransactionID
Result
Verification
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ReconciliationID
State
Desired Version
Actual Version
Difference
Current Plan
Active Transition
Blocking Reason
Retry Count
Transaction
Verification State
```

## Normative Anforderungen

1. NovaOS MUSS Desired und Actual State reconciliieren können.
2. Reconciliation MUSS mit einer Beobachtung des Actual State beginnen können.
3. Unterschiede MÜSSEN explizit erkennbar sein.
4. Difference DARF NICHT automatisch als Failure interpretiert werden.
5. Reconciliation Plans MÜSSEN gültige State Transitions verwenden.
6. Pläne MÜSSEN vor kritischer Ausführung validiert werden.
7. Reconciliation DARF keine Authority erzeugen.
8. Erforderliche Capabilities MÜSSEN vor Änderungen validiert werden.
9. Mehrteilige Änderungen SOLLEN Transactions verwenden.
10. Änderungen von Desired oder Actual State MÜSSEN erkannt werden können.
11. Veraltete Pläne MÜSSEN revalidiert oder verworfen werden können.
12. Reconciliation-Aktionen SOLLEN idempotent sein.
13. Nicht idempotente Aktionen MÜSSEN explizit geschützt werden.
14. Retry MUSS begrenzt und policygesteuert sein.
15. Reconciliation Storms MÜSSEN durch Backoff oder vergleichbare Mechanismen verhindert werden.
16. Unknown State DARF NICHT als Satisfied interpretiert werden.
17. Resource Budgets MÜSSEN respektiert werden.
18. Distributed Reconciliation DARF keine universelle sofortige Konsistenz voraussetzen.
19. Self-Healing MUSS Reconciliation verwenden können.
20. Explizite User Decisions DÜRFEN durch adaptive Optimierung NICHT überschrieben werden.
21. Nach Änderungen MUSS der Actual State erneut überprüfbar sein.
22. Operation Success DARF NICHT automatisch als State Satisfaction gelten.
23. Kritische Reconciliation-Vorgänge SOLLEN Provenance besitzen.
24. Reconciliation MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STATE-GLOBAL-0001`
- `NPSPEC-STATE-DESIRED-0001`
- `NPSPEC-STATE-ACTUAL-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-RESILIENCE-SELFHEALING-0001`
- `NPSPEC-RESILIENCE-RETRY-0001`
- `NPSPEC-RESILIENCE-BACKOFF-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-AUTONOMY-CONSTRAINT-0001`
- `ADR-ARCH-0157`

## Ergebnis

```text
Desired State
      ↓
Observe Actual State
      ↓
Compare
      ↓
Difference?
├── No → Satisfied
└── Yes
      ↓
Plan Transitions
      ↓
Validate
      ↓
Authorize
      ↓
Execute
      ↓
Verify Actual State
      ↓
Satisfied?
├── Yes → Maintain
└── No  → Replan / Recover / Escalate
```

NovaOS erhält damit einen einheitlichen Reconciliation-Mechanismus, der deklarierte Zielzustände kontrolliert mit dem tatsächlichen Systemzustand abgleicht und notwendige Änderungen sicher, transaktional, ressourcenbewusst und überprüfbar durchführt.