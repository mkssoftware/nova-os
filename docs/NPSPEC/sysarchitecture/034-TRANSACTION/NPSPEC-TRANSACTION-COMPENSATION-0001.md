# NPSPEC-TRANSACTION-COMPENSATION-0001 – Nova Transaction Compensation

## Status

Angenommen

## Kategorie

Transaction / Compensation / Recovery

## Zweck

NovaOS definiert Compensation als kontrollierten Mechanismus für Transaktionen, deren bereits ausgeführte Effekte nicht direkt zurückgerollt werden können.

```text
Transaction
    ↓
Irreversible Effect
    ↓
Later Failure
    ↓
Compensation
    ↓
Equivalent Valid State
    ↓
Verify
```

Compensation stellt nicht zwingend den ursprünglichen Zustand wieder her, sondern erzeugt einen neuen gültigen Zustand, der die Auswirkungen einer fehlgeschlagenen Transaktion soweit möglich ausgleicht.

## Grundprinzipien

```text
Compensation ≠ Rollback
Compensation ≠ Undo
Compensation ≠ Exact Reversal
Compensation ≠ Recovery
Compensated ≠ Verified
Compensation Success ≠ Original State Restored
```

## Compensation Model

```text
Compensation
├── CompensationID
├── TransactionID
├── OperationID
├── OriginalEffect
├── CompensationAction
├── TargetID
└── State
```

Optional:

```text
FailureID
DependencySet
ExecutionContractID
RequiredCapabilities
CompensationPolicy
CompensationBudget
Deadline
RecoveryPolicy
ProvenanceID
```

## Zustände

```text
Required
Planned
Validating
Prepared
Executing
Executed
Verifying
Completed
PartiallyCompleted
Failed
Escalated
Unknown
```

## Einsatzbereich

Compensation wird verwendet, wenn ein bereits sichtbarer Effekt nicht direkt zurückgenommen werden kann.

Beispiele:

```text
Remote Modification
Network Message
External Service Operation
Physical Device Action
Cross-Service State Change
Published Event
Consumed Resource
```

## Reversibility Classification

Operationen sollen klassifiziert werden können als:

```text
Reversible
Conditionally Reversible
Compensatable
Irreversible
Unknown
```

```text
Compensatable ≠ Reversible
```

Diese Information soll bereits vor Ausführung einer kritischen Transaktion verfügbar sein.

## Compensation Action

Eine Operation kann eine zugehörige Compensation definieren.

```text
Operation A
    ↓
Effect A
    ↓
Transaction Failure
    ↓
Compensation A
```

Beispiel:

```text
Reserve Resource
→ Release Resource

Activate Provider
→ Deactivate Provider

Create Remote Object
→ Delete / Invalidate Remote Object
```

Die Compensation-Semantik ist operationsspezifisch.

## Compensation Plan

Bei mehreren Effekten kann ein Plan erforderlich sein.

```text
Operation A
Operation B
Operation C
     ↓
Failure
     ↓
Compensate C
Compensate B
Compensate A
```

Reverse Order soll bevorzugt werden, wenn Abhängigkeiten dies erfordern.

Die tatsächliche Reihenfolge muss aus Dependencies und Operation Semantics bestimmt werden.

## Validation

Vor Ausführung einer Compensation müssen relevante Bedingungen erneut geprüft werden:

```text
Current State
Capabilities
Dependencies
Security
Trust
Sovereignty
Resource Availability
Target Availability
Execution Contract
```

```text
Authorized Original Operation
≠
Authorized Compensation
```

## Capability Security

Compensation benötigt eigene gültige Authority.

Eine ursprüngliche Capability darf nicht automatisch als weiterhin gültige Compensation Authority betrachtet werden.

```text
Original Capability Revoked
        ↓
Compensation Requires Revalidation
```

Security- oder Revocation-Zustände dürfen nicht durch Compensation umgangen werden.

## Idempotency

Compensation kann nach Kommunikations- oder Systemfehlern wiederholt werden müssen.

```text
Compensation Request
        ↓
Timeout
        ↓
UnknownExecutionState
```

Compensation Actions sollen deshalb möglichst:

```text
Idempotent
Deduplicated
State-aware
```

implementiert werden.

```text
Timeout ≠ Compensation Failed
```

## Partial Compensation

Nicht alle Effekte müssen erfolgreich kompensierbar sein.

```text
Effect A → Compensated
Effect B → Compensated
Effect C → Failed
Effect D → Irreversible
```

Das Ergebnis muss als partieller Zustand erhalten bleiben.

```text
Partial Compensation ≠ Success
```

## Multi-Service Compensation

Bei Multi-Service Transactions können unterschiedliche Services eigene Compensation Actions bereitstellen.

```text
Transaction
├── Service A → Compensation A
├── Service B → Compensation B
└── Service C → Compensation C
```

Der Coordinator koordiniert diese Aktionen, erhält dadurch aber keine zusätzliche Authority über die Participants.

## Distributed Compensation

Verteilte Compensation muss berücksichtigen:

```text
Network Partition
Participant Failure
Unknown Execution State
Replication
Consistency
Causality
Membership
```

Eine globale sofortige Compensation wird nicht vorausgesetzt.

Compensation kann selbst eine länger laufende verteilte Transaktion sein.

## Compensation Log

Recovery-kritische Compensation-Schritte müssen im Transaction Log abbildbar sein.

```text
COMPENSATION_REQUIRED
COMPENSATION_BEGIN
COMPENSATION_EXECUTED
COMPENSATION_VERIFIED
COMPENSATION_FAILED
```

Dadurch kann eine unterbrochene Compensation nach Neustart fortgesetzt werden.

## Compensation Budget

Compensation muss begrenzbar sein durch:

```text
Attempts
Time
CPU
Memory
IO
Network
Energy
```

Bei erschöpftem Budget erfolgt Eskalation an die Recovery Policy.

## Deadline

Compensation kann eigene Deadlines besitzen.

Eine ursprüngliche Transaction Deadline darf nicht automatisch bedeuten, dass notwendige Sicherheits- oder Konsistenzmaßnahmen nach Ablauf unterlassen werden.

## Verification

Nach Compensation muss geprüft werden:

```text
Expected Compensated State
Actual State
Dependencies
Integrity
Capabilities
Security
Consistency
Remaining Effects
```

```text
Compensation Executed ≠ Compensation Verified
```

## Recovery

Schlägt Compensation fehl:

```text
Compensation Failed
        ↓
Contain
        ↓
Recovery Policy
        ↓
Retry / Alternative Compensation
Degradation
Manual Intervention
Recovery Mode
```

Irreversible verbleibende Effekte müssen sichtbar bleiben.

## Provenance

Nachvollziehbar sein müssen:

```text
CompensationID
TransactionID
Original Operation
Original Effect
Reason
Compensation Action
Authority
Result
Remaining Effects
Verification Result
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
CompensationID
TransactionID
Current State
Original Effect
Planned Action
Dependencies
Attempts
Compensation Budget
Remaining Effects
Verification State
Recovery State
```

## Normative Anforderungen

1. NovaOS MUSS Compensation für nicht direkt rollbackfähige Transaktionseffekte unterstützen können.
2. Compensation MUSS von Rollback und Undo getrennt bleiben.
3. Operationen SOLLEN ihre Reversibility Semantics deklarieren können.
4. Compensatable DARF NICHT als Reversible interpretiert werden.
5. Compensation Actions MÜSSEN operationsspezifisch definierbar sein.
6. Mehrere Compensation Actions MÜSSEN abhängigkeitsgerecht geordnet werden können.
7. Compensation MUSS den aktuellen Zustand vor Ausführung revalidieren.
8. Compensation MUSS eigene gültige Capability Authority besitzen.
9. Widerrufene Authority DARF NICHT durch Compensation wiederhergestellt oder umgangen werden.
10. Compensation Actions SOLLEN möglichst idempotent oder deduplizierbar sein.
11. Timeout DARF NICHT automatisch als fehlgeschlagene Compensation interpretiert werden.
12. `UnknownExecutionState` MUSS explizit behandelbar sein.
13. Partielle Compensation MUSS als eigener Zustand darstellbar sein.
14. Irreversible verbleibende Effekte DÜRFEN NICHT verborgen werden.
15. Multi-Service Transactions MÜSSEN Participant-spezifische Compensation unterstützen können.
16. Distributed Compensation DARF keine globale sofortige Reversibilität voraussetzen.
17. Recovery-kritische Compensation-Schritte MÜSSEN im Transaction Log abbildbar sein.
18. Unterbrochene Compensation MUSS nach Recovery fortsetzbar sein können.
19. Compensation MUSS durch ein Compensation Budget begrenzbar sein.
20. Fehlgeschlagene Compensation MUSS an Recovery Policy eskalierbar sein.
21. Compensation MUSS nach Ausführung verifiziert werden können.
22. Compensation-Zustände MÜSSEN autorisiert introspektierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-0001`
- `NPSPEC-TRANSACTION-LOG-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-DISTCOMM-IDEMPOTENCY-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0139`

## Ergebnis

```text
Transaction Failure
        ↓
Determine Applied Effects
        ↓
Reversible?
├── Yes → Rollback
└── No
      ↓
Compensatable?
├── Yes
│     ↓
│  Plan Compensation
│     ↓
│  Validate Authority + State
│     ↓
│  Execute
│     ↓
│  Verify
│
└── No
      ↓
Contain + Recovery Policy
      ↓
Expose Remaining Effect
```

NovaOS erhält damit ein kontrolliertes Compensation-Modell für Transaktionen, das irreversible oder extern sichtbare Effekte nicht fälschlich als rollbackfähig behandelt, sondern durch explizite Gegenmaßnahmen in einen neuen validierten und nachvollziehbaren Systemzustand überführt.