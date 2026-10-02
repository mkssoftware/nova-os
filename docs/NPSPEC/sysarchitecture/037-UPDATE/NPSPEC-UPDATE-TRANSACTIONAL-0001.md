# NPSPEC-UPDATE-TRANSACTIONAL-0001 – Nova Transactional Update

## Status

Angenommen

## Kategorie

Update / Transaction / System Lifecycle

## Zweck

NovaOS definiert Updates als kontrollierte Transaktionen, deren Änderungen gemeinsam geplant, validiert, vorbereitet, angewendet, verifiziert und erst danach abgeschlossen werden.

```text
Begin
  ↓
Validate
  ↓
Prepare
  ↓
Apply
  ↓
Commit
  ↓
Verify
  ↓
Complete
```

Schlägt eine kritische Phase fehl, muss NovaOS einen definierten Abbruch-, Rollback-, Kompensations- oder Recovery-Pfad besitzen.

## Grundprinzipien

```text
Update Transaction ≠ Database Transaction
Transaction ≠ Atomicity Alone
Prepared ≠ Applied
Applied ≠ Committed
Committed ≠ Verified
Abort ≠ Rollback Completed
Rollback ≠ Compensation
Failure ≠ Undefined State
Unknown ≠ Success
```

## Transaction Model

```text
UpdateTransaction
├── TransactionID
├── UpdateSet
├── Scope
├── BaseVersions
├── TargetVersions
├── StagedChanges
├── Dependencies
├── Constraints
└── State
```

Optional:

```text
ExecutionContractID
ResourceBudget
Deadline
SnapshotID
ABSlot
MigrationPlan
RollbackPlan
CompensationPlan
VerificationPlan
RecoveryPolicy
ProvenanceID
```

## Transaction States

```text
Created
   ↓
Validating
   ↓
Preparing
   ↓
Prepared
   ↓
Applying
   ↓
Committing
   ↓
Committed
   ↓
Verifying
   ↓
Completed
```

Fehlerpfade:

```text
Aborting
Aborted
RollingBack
Compensating
Recovering
Failed
Unknown
```

## Transaction Scope

Eine Update-Transaktion besitzt einen expliziten Scope.

Beispiele:

```text
Package
Package Set
Service Group
Driver Set
System Module Set
Boot Environment
System Image
Configuration Set
```

NovaOS setzt keine universelle globale Update-Transaktion voraus.

## Validation

Vor Änderungen werden mindestens geprüft:

```text
Package Integrity
Signatures
Trust
Dependencies
Compatibility
State Versions
Capabilities
Security Policy
Sovereignty
Resources
Migration Requirements
Recovery Availability
```

Fehlgeschlagene harte Anforderungen blockieren die Transaktion.

## Prepare

Während `Prepare` werden alle notwendigen Voraussetzungen geschaffen.

Beispiele:

```text
Download Packages
Verify Content
Stage Artifacts
Reserve Resources
Create Snapshot
Prepare A/B Slot
Prepare Migration
Prepare Recovery Path
```

Der aktive Zustand soll dabei unverändert bleiben.

## Revalidation

Zwischen Vorbereitung und Commit können sich relevante Zustände ändern.

Daher müssen kritische Voraussetzungen unmittelbar vor Commit erneut validiert werden.

```text
Prepared State
     ↓
Revalidate
     ↓
Commit Allowed?
```

Geprüft werden insbesondere:

```text
State Versions
Capabilities
Trust State
Revocation State
Dependencies
Resources
Security Constraints
```

## Apply

`Apply` überführt vorbereitete Änderungen in ihren Zielbereich, ohne automatisch deren endgültige Gültigkeit zu erklären.

```text
Staged Changes
      ↓
Apply
      ↓
Candidate State
```

## Commit

Commit markiert den definierten Übergang zum neuen autoritativen Zustand.

```text
Old Authoritative State
          ↓
       Commit
          ↓
New Authoritative State
```

Der Commit-Zustand muss persistent rekonstruierbar sein.

## Atomic Integration

Innerhalb des definierten Update-Scope wird `NPSPEC-UPDATE-ATOMIC-0001` verwendet.

```text
Transactional Update
        ↓
Prepare Complete Set
        ↓
Atomic Commit
```

Transactional beschreibt den gesamten kontrollierten Ablauf.

Atomic beschreibt insbesondere die Sichtbarkeit des Zustandsübergangs.

## Verification

Nach Commit muss der neue Zustand geprüft werden.

```text
Committed
   ↓
Activate
   ↓
Health Check
   ↓
Integrity Check
   ↓
Contract Verification
   ↓
Operational Verification
```

```text
Committed ≠ Verified
```

Erst nach erfolgreicher Verification darf die Transaktion als vollständig abgeschlossen gelten.

## Abort

Vor Commit soll eine Transaktion ohne Änderung des autoritativen Zustands abbrechbar sein.

```text
Prepared
   ↓
Abort
   ↓
Discard Staged Changes
```

Reservierte Ressourcen müssen kontrolliert freigegeben werden.

## Rollback

Nach einem bereits erfolgten Zustandswechsel kann Rollback erforderlich sein.

```text
New State
   ↓
Verification Failed
   ↓
Rollback
   ↓
Previous Known-Good Content
   ↓
New State Version
```

Rollback schreibt die Historie nicht um.

## Compensation

Nicht vollständig reversible Änderungen können Kompensationsaktionen benötigen.

```text
Irreversible Action
       ↓
Compensation
```

```text
Compensation ≠ Rollback
```

Beispiele:

```text
Reconfigure External State
Restore Compatible Configuration
Deploy Replacement
Reconcile Dependent Service
```

## State Migration

State Migration ist Bestandteil derselben kontrollierten Transaktion.

```text
Component Update
      +
State Migration
      ↓
Transaction
```

Migrationen müssen hinsichtlich Reversibilität klassifiziert werden:

```text
Reversible
Conditionally Reversible
Compensatable
Irreversible
Unknown
```

## Dependency Transactions

Mehrere voneinander abhängige Pakete werden als gemeinsamer Update Set behandelt.

```text
Package A
Package B
Package C
    ↓
UpdateTransaction
```

Kein Teil darf committed werden, wenn dadurch harte Dependency-Invarianten verletzt werden.

## Crash Recovery

Der Transaktionszustand muss nach Crash oder Stromausfall rekonstruierbar sein.

```text
Transaction Journal
        ↓
Recover State
        ↓
Continue / Abort / Rollback / Recover
```

Mögliche Zustände:

```text
NotCommitted
Committed
VerificationPending
RollbackPending
RecoveryRequired
Unknown
```

`Unknown` muss konservativ behandelt werden.

## Concurrency

Update-Transaktionen müssen konkurrierende Änderungen erkennen.

```text
Expected Version
      ≠
Current Version
      ↓
Conflict
```

Mögliche Reaktionen:

```text
Abort
Replan
Retry
Reconcile
```

## Distributed Updates

Verteilte Updates besitzen keine automatische globale ACID-Garantie.

Je nach Subsystem können verwendet werden:

```text
Local Transactions
Staged Rollout
Versioned Deployment
Consensus
Compensation
Reconciliation
```

Partielle Fehler müssen explizit modelliert werden.

## Security

Eine Update-Transaktion darf Security-Mechanismen nicht umgehen.

Insbesondere dürfen Rollback oder Recovery nicht:

```text
Revoked Keys Reactivate
Security State Downgrade
Invalid Trust Restore
Capabilities Expand
Policy Restrictions Remove
```

## Provenance

Der gesamte Transaktionsablauf soll nachvollziehbar sein:

```text
TransactionID
UpdateSet
BaseVersions
TargetVersions
Validation
Prepare
Commit
Verification
Rollback
Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
TransactionID
Scope
UpdateSet
Current State
Base Versions
Target Versions
Prepared State
Commit State
Verification State
Rollback State
Recovery State
Failure Reason
```

## Normative Anforderungen

1. Systemkritische Updates MÜSSEN transaktional ausführbar sein.
2. Jede Update-Transaktion MUSS eine eindeutige TransactionID besitzen.
3. Jede Transaktion MUSS einen expliziten Scope besitzen.
4. Harte Anforderungen MÜSSEN vor Prepare validiert werden.
5. Kritische dynamische Anforderungen MÜSSEN vor Commit revalidiert werden.
6. Staged Changes DÜRFEN vor Commit NICHT automatisch autoritativ werden.
7. Der Commit Point MUSS eindeutig definiert sein.
8. Commit MUSS persistent rekonstruierbar sein.
9. `Committed` DARF NICHT als `Verified` interpretiert werden.
10. Kritische Updates MÜSSEN nach Commit verifiziert werden.
11. Vor Commit SOLL ein kontrollierter Abort möglich sein.
12. Nach Commit MUSS bei kritischen Fehlern Rollback, Compensation oder Recovery möglich sein.
13. Rollback DARF Historie NICHT überschreiben.
14. Compensation MUSS von Rollback unterschieden werden.
15. State Migration MUSS in die Transaktion integrierbar sein.
16. Irreversible Änderungen MÜSSEN explizit gekennzeichnet werden.
17. Abhängige Pakete MÜSSEN als konsistenter Update Set behandelbar sein.
18. Harte Dependency-Invarianten DÜRFEN durch Commit NICHT verletzt werden.
19. Crash- und Power-Loss-Recovery MÜSSEN den Transaktionszustand rekonstruieren können.
20. `Unknown` DARF NICHT als erfolgreicher Abschluss interpretiert werden.
21. Concurrent State Changes MÜSSEN erkannt werden können.
22. Verteilte Updates DÜRFEN NICHT implizit globale ACID-Semantik voraussetzen.
23. Rollback und Recovery DÜRFEN aktuelle Security- und Revocation-Zustände NICHT abschwächen.
24. Transaktionsentscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
25. Transaction State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-DEPENDENCY-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-STATE-TRANSACTIONAL-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-STATE-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-VERIFY-CONTRACT-0001`
- `ADR-ARCH-0169`

## Ergebnis

```text
Update Request
      ↓
Begin Transaction
      ↓
Validate
      ↓
Prepare + Stage
      ↓
Revalidate
      ↓
Apply
      ↓
Commit
      ↓
Verify
     ↙ ↘
   OK   Failure
   ↓       ↓
Complete  Rollback /
          Compensation /
          Recovery
```

NovaOS erhält damit einen vollständigen transaktionalen Update-Lebenszyklus, der Vorbereitung, atomaren Zustandswechsel, Verifikation, Rollback, Kompensation und Recovery zu einem kontrollierten Prozess verbindet und verhindert, dass fehlgeschlagene Updates einen undefinierten oder inkonsistenten Systemzustand hinterlassen.