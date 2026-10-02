# NPSPEC-TRANSACTION-SYSTEM-0001 – Nova Transaction System

## Status

Angenommen

## Kategorie

Transaction / System Architecture / State Management

## Zweck

NovaOS definiert ein systemweites Transaktionsmodell für kontrollierte Zustandsänderungen über Kernel, Storage, Objekte, Konfigurationen, Capabilities, Services und verteilte Komponenten.

```text
Current State
     ↓
Begin
     ↓
Validate
     ↓
Prepare
     ↓
Commit
     ↓
Verify
     ↓
New Valid State
```

Ziel ist, partielle, inkonsistente oder unkontrollierte Zustandsänderungen zu vermeiden.

## Grundprinzipien

```text
Transaction ≠ Database Transaction
Transaction ≠ Lock
Transaction ≠ Snapshot
Transaction ≠ Rollback
Commit ≠ Verification
Failure ≠ Automatic Rollback
```

NovaOS verwendet Transaktionen als allgemeines Systemprinzip und setzt keine universelle globale ACID-Transaktion voraus.

## Transaction Model

```text
Transaction
├── TransactionID
├── OwnerID
├── Scope
├── Operations
├── Dependencies
├── Constraints
└── State
```

Optional:

```text
ParentTransactionID
ExecutionContractID
ResourceBudget
Deadline
IsolationPolicy
RollbackPolicy
RecoveryPolicy
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
Completed
Aborting
Aborted
Failed
Unknown
```

## Transaction Lifecycle

```text
Begin
  ↓
Execute / Stage Changes
  ↓
Validate
  ↓
Prepare
  ↓
Commit
  ↓
Verify
```

Bei Fehler:

```text
Failure
   ↓
Abort / Compensate / Rollback / Recover
```

## Transaction Scope

Transaktionen können unterschiedliche Bereiche umfassen:

```text
Object
File
Storage
Configuration
Capability Operation
Process State
Service
Provider
System Update
Distributed Operation
```

Der Scope muss explizit definiert sein.

## Staged Changes

Änderungen sollen soweit möglich zunächst vorbereitet werden.

```text
Current State
     ↓
Staged State
     ↓
Validation
     ↓
Commit
```

Andere Komponenten dürfen vorbereitete Zustände nicht unbeabsichtigt als committed Zustand interpretieren.

## Validation

Vor Commit müssen relevante Bedingungen geprüft werden:

```text
Semantic Validity
State Validity
Dependencies
Capabilities
Security
Trust
Sovereignty
Resource Availability
Execution Contract
```

```text
Technically Possible ≠ Valid Transaction
```

## Prepare

Die Prepare-Phase stellt sicher, dass notwendige Voraussetzungen für den Commit vorhanden sind.

```text
Validate
   ↓
Reserve Resources
Lock / Version State
Prepare Providers
Prepare Storage
   ↓
Prepared
```

`Prepared` bedeutet nicht `Committed`.

## Commit

Commit veröffentlicht den neuen Zustand kontrolliert.

```text
Prepared State
      ↓
Commit Point
      ↓
Committed State
```

Der Commit Point muss für den jeweiligen Transaction Scope eindeutig definiert sein.

## Verification

Nach Commit kann eine Verification erforderlich sein.

```text
Commit
   ↓
Verify Result
   ├── Valid → Complete
   └── Invalid → Recovery
```

```text
Committed ≠ Verified
```

Ein technisch erfolgreicher Commit beweist nicht automatisch, dass das gewünschte Systemergebnis erreicht wurde.

## Atomicity

NovaOS soll atomare Zustandsübergänge unterstützen, wo dies technisch sinnvoll möglich ist.

```text
State A
  ↓
State B
```

Andere Komponenten sollen keinen ungültigen Zwischenzustand beobachten.

Vollständige Atomicity über beliebige verteilte Systeme wird nicht vorausgesetzt.

## Isolation

Parallel ausgeführte Transaktionen müssen kontrolliert interagieren.

```text
Transaction A
      ↕
Isolation Policy
      ↕
Transaction B
```

Isolation kann abhängig vom Subsystem unterschiedliche Garantien besitzen.

## Conflict Detection

Konflikte müssen erkannt werden können.

Beispiele:

```text
Version Conflict
Concurrent Modification
Resource Conflict
Capability Revocation
Dependency Change
Provider Change
```

Konflikte können zu:

```text
Retry
Revalidation
Abort
Merge
Serialization
```

führen.

## Nested Transactions

NovaOS darf verschachtelte Transaktionen unterstützen.

```text
Transaction A
├── Transaction B
└── Transaction C
```

Child-Commit bedeutet nicht automatisch Parent-Commit.

Die Sichtbarkeit verschachtelter Änderungen muss explizit definiert sein.

## Capability Security

Eine Transaktion besitzt keine implizite Authority.

```text
Transaction
    +
Capabilities
    ↓
Authorized Operations
```

Capability Revocation während einer laufenden Transaktion muss berücksichtigt werden.

```text
Valid at Begin ≠ Valid at Commit
```

Kritische Authority muss vor Commit erneut validiert werden können.

## Resource Integration

Transaktionen können Ressourcen reservieren.

```text
CPU
Memory
Storage
IO
Network
Device
```

Reservationen müssen begrenzt und nach Abschluss oder Abort freigegeben werden.

## Deadline

Transaktionen können eine Deadline besitzen.

```text
Begin
  ↓
Work
  ↓
Commit
  ↓
Deadline
```

Eine überschrittene Deadline muss entsprechend Execution Contract und Policy behandelt werden.

## Rollback

Wenn ein Zustand reversibel ist:

```text
Transaction Failure
       ↓
Rollback
       ↓
Previous Valid State
```

Rollback ist jedoch nicht für jede Operation möglich.

## Compensation

Bei irreversiblen Operationen können Kompensationsaktionen erforderlich sein.

```text
Action A
   ↓
External Effect
   ↓
Transaction Failure
   ↓
Compensation
```

```text
Compensation ≠ Undo
```

## Recovery

Bei unklarem Transaktionszustand:

```text
Unknown Transaction State
          ↓
Recovery
          ↓
Determine Durable State
          ↓
Complete / Abort / Compensate
```

`Unknown` darf nicht automatisch als `Aborted` interpretiert werden.

## Persistence

Persistente Transaktionen müssen Crash Recovery unterstützen können.

Dafür können verwendet werden:

```text
Transaction Log
Version Metadata
Journal
Checkpoint
Persistent Intent
```

Nach Neustart muss bestimmbar sein, ob eine Transaktion:

```text
Committed
Aborted
Incomplete
Unknown
```

ist.

## Distributed Transactions

Verteilte Transaktionen können mehrere Nodes oder Provider umfassen.

```text
Coordinator
├── Participant A
├── Participant B
└── Participant C
```

NovaOS setzt keine universelle globale ACID-Semantik voraus.

Je nach System können verwendet werden:

```text
Two-Phase Commit
Consensus
Versioning
Idempotency
Compensation
Saga-like Workflows
Application-specific Protocols
```

## Partial Failure

```text
Participant A → Committed
Participant B → Unknown
Participant C → Unreachable
```

Ein partieller Fehler muss als expliziter Zustand behandelt werden.

```text
No Response ≠ Abort
Timeout ≠ Transaction Failed
```

## Integration mit Execution Contracts

Transaktionen müssen relevante Anforderungen des Execution Contracts berücksichtigen:

```text
Resource Budget
Deadline
Determinism
Trust
Security
Sovereignty
Provider Constraints
```

## Determinismus

Deterministische Ausführungsmodi müssen definierte Transaction Ordering Rules verwenden können.

```text
Input
  ↓
Transaction Sequence
  ↓
Deterministic State
```

Record & Replay muss Transaction IDs und Commit-Reihenfolgen erfassen können.

## Provenance

Transaktionsänderungen sollen nachvollziehbar sein.

```text
TransactionID
Owner
Operations
Previous State
New State
Authority
Commit Point
Verification Result
```

Secrets oder Capability Tokens dürfen dabei nicht protokolliert werden.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
TransactionID
Scope
Owner
State
Operations
Dependencies
Resource Reservations
Deadline
Conflicts
Commit State
Verification State
Recovery State
```

## Normative Anforderungen

1. NovaOS MUSS ein systemweites Transaktionsmodell bereitstellen.
2. Transaktionen MÜSSEN stabile Transaction IDs besitzen.
3. Der Transaction Scope MUSS explizit definiert sein.
4. NovaOS MUSS `Begin → Validate → Prepare → Commit → Verify` unterstützen können.
5. Vorbereitete Zustände DÜRFEN NICHT automatisch als committed gelten.
6. Commit und Verification MÜSSEN getrennt bleiben.
7. Transaktionen DÜRFEN keine implizite Authority erzeugen.
8. Kritische Capabilities MÜSSEN vor Commit revalidierbar sein.
9. Konflikte zwischen parallelen Transaktionen MÜSSEN erkennbar sein.
10. Ungültige Zwischenzustände SOLLEN vor anderen Komponenten verborgen bleiben.
11. Resource Reservations MÜSSEN begrenzbar und freigebbar sein.
12. Transaktionen MÜSSEN Deadlines und Cancellation berücksichtigen können.
13. Rollback DARF nur für tatsächlich reversible Zustände vorausgesetzt werden.
14. Irreversible Operationen MÜSSEN Compensation unterstützen können.
15. `Unknown` DARF NICHT automatisch als `Aborted` interpretiert werden.
16. Persistente Transaktionen MÜSSEN Crash Recovery unterstützen können.
17. Distributed Transactions DÜRFEN keine universelle globale ACID-Semantik voraussetzen.
18. Partielle verteilte Fehler MÜSSEN explizit darstellbar sein.
19. Transaction Recovery MUSS mit Resilience und Recovery Policies integrierbar sein.
20. Transaction-Zustände und Entscheidungen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0133`

## Ergebnis

```text
Intent
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
├── Valid → Complete
└── Invalid
      ↓
Abort / Rollback / Compensate / Recover
```

NovaOS erhält damit ein einheitliches systemweites Transaktionsmodell, das Zustandsänderungen über Subsystemgrenzen hinweg kontrollierbar, validierbar und wiederherstellbar macht, ohne eine unrealistische universelle globale ACID-Transaktion vorauszusetzen.