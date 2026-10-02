# NPSPEC-TRANSACTION-MULTISERVICE-0001 – Nova Multi-Service Transactions

## Status

Angenommen

## Kategorie

Transaction / Services / Distributed Coordination

## Zweck

NovaOS definiert ein Transaktionsmodell für Zustandsänderungen, die mehrere Services, Provider oder Subsysteme gemeinsam betreffen.

```text
Transaction
├── Service A
├── Service B
└── Service C
        ↓
Coordinated Commit
```

Ziel ist ein kontrollierter Gesamtzustand, ohne eine universelle globale ACID-Transaktion vorauszusetzen.

## Grundprinzipien

```text
Multi-Service Transaction ≠ Global ACID
Service Success ≠ Transaction Success
Timeout ≠ Failure
No Response ≠ Abort
Commit ≠ Verification
Compensation ≠ Rollback
Coordinator ≠ Universal Authority
```

## Transaction Model

```text
MultiServiceTransaction
├── TransactionID
├── CoordinatorID
├── Participants
├── Operations
├── Dependencies
├── CommitPolicy
└── State
```

Optional:

```text
ParentTransactionID
ExecutionContractID
Deadline
ResourceBudget
ConsistencyPolicy
RecoveryPolicy
CompensationPolicy
ProvenanceID
```

## Zustände

```text
Created
Active
Validating
Preparing
Prepared
Committing
Committed
Verifying
Compensating
Aborting
Completed
Failed
Unknown
```

## Participants

Ein Participant kann sein:

```text
Service
Provider
Subsystem
Storage Service
Network Service
Device Service
Remote Service
Distributed Service
```

Jeder Participant behält seine eigene Authority und interne Implementierung.

## Ablauf

```text
Begin
  ↓
Discover Participants
  ↓
Validate
  ↓
Prepare Participants
  ↓
Commit Decision
  ↓
Commit
  ↓
Verify
```

Bei Fehlern:

```text
Failure
   ↓
Abort / Retry / Compensate / Recover
```

## Coordinator

Ein Coordinator verwaltet den Transaktionsablauf.

```text
Coordinator
├── Participant A
├── Participant B
└── Participant C
```

Der Coordinator darf nur Operationen koordinieren, für die entsprechende Authority vorhanden ist.

```text
Coordination Authority ≠ Participant Authority
```

## Prepare Phase

Participants prüfen vor Commit:

```text
Operation Validity
Capabilities
Dependencies
Resources
Security
Trust
Sovereignty
Execution Contract
Local State
```

Ein Participant kann antworten:

```text
Prepared
Rejected
Unavailable
Unknown
```

`Unknown` darf nicht als `Prepared` interpretiert werden.

## Commit Decision

Die Commit Policy bestimmt, unter welchen Bedingungen committed werden darf.

Beispiele:

```text
All Required Participants Prepared
Quorum Prepared
Policy-defined Participant Set
```

Die Semantik muss für die jeweilige Transaktion explizit definiert sein.

## Commit

Nach einer gültigen Commit Decision:

```text
Coordinator
     ↓
Commit Decision
 ┌───┼───┐
 ↓   ↓   ↓
 A   B   C
```

Participants müssen Commit möglichst idempotent verarbeiten können.

## Partial Commit

Verteilte Systeme können in einen partiellen Zustand geraten.

```text
Service A → Committed
Service B → Committed
Service C → Unknown
```

Dieser Zustand darf nicht als vollständig erfolgreich dargestellt werden.

```text
Partial Commit ≠ Completed Transaction
```

NovaOS muss Recovery einleiten können.

## Unknown Execution State

Nach Kommunikationsverlust kann unbekannt sein, ob eine Operation ausgeführt wurde.

```text
Request
   ↓
Network Failure
   ↓
UnknownExecutionState
```

Blindes Wiederholen ist nur zulässig, wenn die Operation entsprechend idempotent oder anderweitig abgesichert ist.

## Idempotency

Participants sollen für kritische Operationen stabile Identifikatoren verwenden können.

```text
TransactionID
+
OperationID
+
IdempotencyKey
```

Dadurch kann eine wiederholte Anfrage erkannt werden.

## Compensation

Nicht alle externen Effekte können atomar zurückgerollt werden.

```text
Service A → Effect A
Service B → Failure
      ↓
Compensation A
```

```text
Compensation ≠ Undo
```

Eine Compensation erzeugt einen neuen kontrollierten Zustand und garantiert nicht die exakte Wiederherstellung des vorherigen Zustands.

## Local Transactions

Jeder Participant kann intern eigene Transaktionen verwenden.

```text
Multi-Service Transaction
├── Service A → Local Transaction
├── Service B → Local Transaction
└── Service C → Local Transaction
```

Die globale Koordination darf die interne Transaktionsimplementierung nicht unnötig vorgeben.

## Capability Security

Jede Service-Operation benötigt ihre eigene Authority.

```text
Transaction
   ↓
Service A Capability
Service B Capability
Service C Capability
```

Eine Capability für Service A darf keine implizite Authority über Service B erzeugen.

Capabilities müssen vor kritischen Commit-Schritten revalidierbar sein.

## Deadline

Eine Multi-Service Transaction kann eine gemeinsame Deadline besitzen.

```text
Begin
  ↓
Prepare
  ↓
Commit
  ↓
Deadline
```

Participants können zusätzlich eigene lokale Deadlines besitzen.

```text
Global Deadline ≠ Local Timeout
```

## Cancellation

Cancellation muss an relevante Participants propagiert werden können.

```text
Cancel
  ↓
Coordinator
  ↓
Participants
```

Bereits committed externe Effekte können dadurch nicht automatisch rückgängig gemacht werden.

## Failure Handling

Mögliche Fehler:

```text
Participant Failure
Coordinator Failure
Network Partition
Timeout
Capability Revocation
Resource Failure
Dependency Failure
Commit Failure
```

Recovery muss anhand des tatsächlich bekannten Zustands erfolgen.

## Coordinator Failure

Der Ausfall des Coordinators darf nicht automatisch bedeuten, dass die gesamte Transaktion abgebrochen wurde.

Participants müssen ihren Zustand ausreichend erhalten können, um Recovery zu ermöglichen.

```text
Coordinator Lost
      ↓
Recover Transaction State
      ↓
Determine Decision
```

## Recovery

Nach einem Fehler:

```text
Collect Participant States
        ↓
Determine Durable State
        ↓
Complete / Abort / Compensate
        ↓
Verify
```

Kann kein eindeutiger Zustand bestimmt werden, bleibt die Transaktion `Unknown` und muss entsprechend isoliert oder eskaliert werden.

## Consistency

Multi-Service Transactions können unterschiedliche Consistency Policies verwenden.

```text
Strong
Bounded
Eventual
Application-defined
```

Die gewählte Semantik muss für Consumer erkennbar sein.

## Distributed Integration

Participants können auf unterschiedlichen Nodes liegen.

```text
Node A              Node B
Service A ←──────→ Service B
        Transaction
```

Netzwerkpartitionen, Membership und Failure Domains müssen berücksichtigt werden.

Eine Netzwerkpartition darf nicht automatisch als Service-Ausfall interpretiert werden.

## Execution Contract

Die Transaktion muss relevante Anforderungen berücksichtigen:

```text
Deadline
Resource Budget
Determinism
Security
Trust
Sovereignty
Provider Constraints
```

Ein alternativer Participant darf nur verwendet werden, wenn der Execution Contract weiterhin erfüllt wird.

## Verification

Nach Commit müssen relevante Ergebnisse überprüft werden.

```text
Commit
   ↓
Participant Verification
   ↓
Cross-Service Verification
   ↓
Complete
```

```text
All Commit Responses Received ≠ Desired State Verified
```

## Provenance

Nachvollziehbar sein sollen:

```text
TransactionID
Coordinator
Participants
Operations
Prepare Results
Commit Decision
Participant States
Compensations
Recovery Actions
Verification Result
```

Secrets und Capability Tokens dürfen nicht protokolliert werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
TransactionID
CoordinatorID
Participants
Participant States
Current Phase
Deadline
Commit Decision
Unknown Participants
Compensation State
Recovery State
Verification State
```

## Normative Anforderungen

1. NovaOS MUSS Transaktionen über mehrere Services koordinieren können.
2. Multi-Service Transactions DÜRFEN keine universelle globale ACID-Semantik voraussetzen.
3. Participants MÜSSEN ihre eigene Authority behalten.
4. Coordination Authority DARF NICHT automatisch Participant Authority verleihen.
5. Jeder Participant MUSS seine Operation vor Commit validieren können.
6. `Unknown` DARF NICHT als `Prepared`, `Aborted` oder `Committed` angenommen werden.
7. Commit Policies MÜSSEN explizit definierbar sein.
8. Partielle Commits MÜSSEN als eigener Zustand behandelbar sein.
9. Kritische wiederholbare Operationen SOLLEN Idempotency unterstützen.
10. Timeout DARF NICHT automatisch als fehlgeschlagene Operation interpretiert werden.
11. Irreversible Effekte MÜSSEN Compensation unterstützen können.
12. Compensation DARF NICHT als exaktes Rollback angenommen werden.
13. Participants SOLLEN lokale Transaktionen verwenden können.
14. Capabilities MÜSSEN pro Participant kontrolliert werden.
15. Capability Revocation MUSS vor kritischen Commit-Schritten berücksichtigt werden.
16. Deadlines und Cancellation MÜSSEN propagierbar sein.
17. Coordinator Failure DARF NICHT automatisch Transaction Abort bedeuten.
18. Transaction State MUSS nach Coordinator- oder Participant-Ausfällen rekonstruierbar sein.
19. Netzwerkpartitionen DÜRFEN NICHT automatisch als Service Failure interpretiert werden.
20. Alternative Provider MÜSSEN den Execution Contract weiterhin erfüllen.
21. Multi-Service Commit MUSS anschließend verifizierbar sein.
22. Transaction State MUSS autorisiert introspektierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-CONFIG-0001`
- `NPSPEC-TRANSACTION-RESOURCE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-DISTRIBUTED-IPC-0001`
- `NPSPEC-DISTCOMM-IDEMPOTENCY-0001`
- `NPSPEC-DISTCOMM-DEADLINE-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0136`

## Ergebnis

```text
Transaction Intent
        ↓
Resolve Participants
        ↓
Validate Authority + Constraints
        ↓
Prepare Participants
        ↓
Commit Decision
        ↓
Commit
        ↓
Verify
├── Valid → Complete
└── Failure / Unknown
        ↓
Recover / Compensate / Escalate
```

NovaOS erhält damit ein Multi-Service-Transaktionsmodell, das zusammengehörige Zustandsänderungen über mehrere Services und Provider kontrolliert koordiniert, ohne eine unrealistische globale ACID-Garantie vorauszusetzen und ohne Service-Grenzen, Capability Authority oder verteilte Fehlerzustände zu verschleiern.