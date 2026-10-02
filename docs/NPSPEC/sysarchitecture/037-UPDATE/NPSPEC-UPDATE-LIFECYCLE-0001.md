# NPSPEC-UPDATE-LIFECYCLE-0001 – Nova Update Lifecycle

## Status

Angenommen

## Kategorie

Update / Lifecycle / Update Management

## Zweck

NovaOS definiert einen einheitlichen Lebenszyklus für Updates von der ersten Erkennung bis zur endgültigen Verifikation, Bereinigung oder Ablösung.

Der Lifecycle verbindet Repository, Package, Dependency Management, Staging, Transaktionen, Aktivierung, Verifikation, Rollback und Provenance zu einem gemeinsamen Zustandsmodell.

```text
Discover
   ↓
Resolve
   ↓
Validate
   ↓
Stage
   ↓
Prepare
   ↓
Activate
   ↓
Verify
   ↓
Commit
   ↓
Maintain
   ↓
Retire
```

## Grundprinzipien

```text
Available ≠ Applicable
Downloaded ≠ Valid
Valid ≠ Trusted
Trusted ≠ Authorized
Staged ≠ Active
Active ≠ Verified
Verified ≠ Permanently Healthy
Failed ≠ Undefined
Retired ≠ Forgotten
```

## Lifecycle Model

```text
UpdateLifecycle
├── UpdateID
├── TargetID
├── CurrentVersion
├── TargetVersion
├── LifecycleState
├── UpdatePolicy
├── VerificationState
└── ProvenanceID
```

Optional:

```text
PackageID
ContentID
TransactionID
StagingID
SnapshotID
RollbackTarget
ActivationPolicy
ResourceBudget
SecurityState
FailureReason
```

## Lifecycle States

```text
Discovered
Resolved
Validated
Downloaded
Staged
Prepared
PendingActivation
Activating
Active
Verifying
Verified
Committed
Superseded
Retired
```

Fehlerzustände:

```text
Blocked
Failed
RollbackPending
RollingBack
RolledBack
RecoveryRequired
Unknown
```

## Discovery

Updates können erkannt werden über:

```text
Repository
Local Package
Offline Media
Recovery Source
Enterprise Source
Developer Source
```

```text
Discovered
≠
Approved
```

## Resolution

Der Update Manager bestimmt den vollständigen Update-Kontext.

```text
Target
 ↓
Candidate
 ↓
Dependencies
 ↓
Compatibility
 ↓
UpdateSet
```

Dabei werden Versionen, Provider, Dependencies und Konflikte aufgelöst.

## Validation

Vor weiterer Verarbeitung werden geprüft:

```text
Integrity
Signature
Trust
Authorization
Compatibility
Dependencies
Security Policy
Sovereignty Policy
Resource Requirements
```

Fehlgeschlagene harte Prüfungen führen zu `Blocked` oder `Failed`.

## Download

Benötigte Artefakte werden vollständig oder als Delta bezogen.

```text
Repository
    ↓
Download
    ↓
ContentID Verification
```

Download und Trust bleiben getrennte Zustände.

## Staging

Validierte Artefakte werden außerhalb des aktiven Zustands vorbereitet.

```text
Downloaded
    ↓
Stage
    ↓
Prepare Dependencies
    ↓
Prepare Migration
    ↓
Ready
```

Der aktive Systemzustand bleibt dabei möglichst unverändert.

## Prepare

Vor Aktivierung werden alle kritischen Voraussetzungen vorbereitet.

Dazu können gehören:

```text
Snapshot
A/B Slot
Immutable Generation
State Migration
Recovery Path
Resource Reservation
Driver Quiescence
Boot Configuration
```

## Revalidation

Unmittelbar vor Aktivierung müssen veränderliche Bedingungen erneut geprüft werden.

```text
State Version
Trust
Revocation
Capabilities
Dependencies
Resources
Security Policy
```

```text
Previously Valid
≠
Currently Valid
```

## Activation

Die Aktivierung erfolgt abhängig vom Update-Typ:

```text
Atomic Switch
Live Update
Hotpatch
Driver Rebind
A/B Switch
Next Boot
Firmware Reset
Bootloader Trial Boot
Rolling Deployment
Canary Deployment
```

Die Aktivierung muss einen eindeutig bestimmbaren neuen Zustand erzeugen.

## Verification

Nach Aktivierung wird der tatsächlich erreichte Zustand geprüft.

```text
Expected State
      ↓
Observed State
      ↓
Health
      ↓
Contracts
      ↓
Security
      ↓
Verified
```

`Active` allein reicht nicht für erfolgreichen Abschluss.

## Commit

Ein Update darf erst nach den erforderlichen Prüfungen als endgültig erfolgreich markiert werden.

```text
Active
 ↓
Verified
 ↓
Commit
```

Danach können temporäre Ressourcen kontrolliert freigegeben werden.

## Operational Monitoring

Auch nach Commit kann der Zustand weiter überwacht werden.

Später erkannte Probleme können auslösen:

```text
Mitigation
Rollback
Hotpatch
Replacement Update
Recovery
```

Commit bedeutet daher nicht, dass zukünftige Fehler ausgeschlossen sind.

## Superseded

Wird eine Version durch eine neuere Version ersetzt:

```text
Version N
   ↓
Version N+1
   ↓
N = Superseded
```

Superseded Komponenten können weiterhin für Rollback oder Recovery benötigt werden.

## Retirement

Ein Update-Artefakt darf endgültig entfernt werden, wenn keine relevanten Referenzen mehr bestehen.

Zu prüfen sind:

```text
Active State
Rollback
Snapshot
A/B Slot
Recovery
Transaction
Provenance
Audit
Security Investigation
```

```text
Not Active
≠
Safe to Delete
```

## Rollback Lifecycle

```text
Verification Failure
       ↓
RollbackPending
       ↓
RollingBack
       ↓
RolledBack
       ↓
Verify Restored State
```

Rollback erzeugt einen neuen Systemzustand und schreibt Historie nicht um.

## Recovery Lifecycle

Kann ein normaler Rollback nicht durchgeführt werden:

```text
Failed
  ↓
RecoveryRequired
  ↓
Recovery Environment
  ↓
Repair / Restore
  ↓
Verify
```

## Security Lifecycle

Neue Security-Informationen können bereits bestehende Lifecycle-Zustände verändern.

Beispiele:

```text
Signer Revoked
Package Revoked
New Vulnerability
Minimum Secure Version Raised
Trust Policy Changed
```

Ein bereits gestagtes oder rollbackfähiges Artefakt kann dadurch unzulässig werden.

## Crash Recovery

Nach Crash oder Stromverlust muss der Lifecycle rekonstruierbar sein.

```text
Persistent State
      ↓
Determine Last Safe Phase
      ↓
Resume / Abort / Rollback / Recover
```

`Unknown` darf nicht automatisch zu `Committed` werden.

## Provenance

Jeder relevante Lifecycle-Übergang soll nachvollziehbar sein.

```text
UpdateID
Previous State
New State
Timestamp
Reason
Actor
PackageID
ContentID
TransactionID
Verification Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Current Lifecycle State
Target Version
Current Version
Update Progress
Staging State
Activation State
Verification State
Rollback State
Recovery State
Security State
Failure Reason
```

## Normative Anforderungen

1. NovaOS MUSS einen einheitlichen Update Lifecycle definieren.
2. Jeder Update-Vorgang MUSS eindeutig identifizierbar sein.
3. Lifecycle-Zustände MÜSSEN explizit unterscheidbar sein.
4. Discovery DARF NICHT als Update-Autorisierung gelten.
5. Dependencies und Compatibility MÜSSEN vor Aktivierung auflösbar sein.
6. Integrität, Trust und Authorization MÜSSEN vor kritischer Aktivierung geprüft werden.
7. Updates SOLLEN vor Aktivierung gestaged werden.
8. Veränderliche Bedingungen MÜSSEN vor Aktivierung revalidiert werden.
9. Aktivierung MUSS einen eindeutig bestimmbaren Zustand erzeugen.
10. `Active` DARF NICHT als `Verified` interpretiert werden.
11. Kritische Updates MÜSSEN nach Aktivierung verifiziert werden.
12. Commit DARF erst nach den erforderlichen Verifikationen erfolgen.
13. Fehlgeschlagene Updates MÜSSEN definierte Fehlerzustände besitzen.
14. Rollback MUSS als eigener Lifecycle modelliert werden.
15. Recovery MUSS bei nicht normal behebbaren Fehlern erreichbar sein.
16. Rollback DARF Historie NICHT überschreiben.
17. Security-Änderungen MÜSSEN bestehende Update-Artefakte invalidieren können.
18. Revoked Artefakte DÜRFEN NICHT erneut aktiviert werden.
19. Lifecycle-State MUSS Crash und Neustart überleben können.
20. `Unknown` DARF NICHT als erfolgreicher Zustand interpretiert werden.
21. Superseded Artefakte DÜRFEN erhalten bleiben, solange sie benötigt werden.
22. Retirement MUSS aktive Referenzen berücksichtigen.
23. Recovery-relevante Artefakte DÜRFEN NICHT vorzeitig entfernt werden.
24. Lifecycle-Übergänge MÜSSEN nachvollziehbare Provenance besitzen.
25. Lifecycle-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-PACKAGE-0001`
- `NPSPEC-UPDATE-DEPENDENCY-0001`
- `NPSPEC-UPDATE-REPOSITORY-0001`
- `NPSPEC-UPDATE-SIGNING-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-ROLLBACK-0001`
- `NPSPEC-UPDATE-STAGED-0001`
- `NPSPEC-UPDATE-VERIFY-0001`
- `NPSPEC-UPDATE-PROVENANCE-0001`
- `NPSPEC-UPDATE-VULNERABILITY-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `ADR-ARCH-0188`

## Ergebnis

```text
Discover
   ↓
Resolve
   ↓
Validate
   ↓
Download
   ↓
Stage
   ↓
Prepare
   ↓
Revalidate
   ↓
Activate
   ↓
Verify
  ↙   ↘
Success Failure
  ↓       ↓
Commit  Rollback / Recovery
  ↓
Monitor
  ↓
Superseded
  ↓
Retire
```

NovaOS erhält damit ein gemeinsames Zustandsmodell für den vollständigen Lebenszyklus eines Updates. Jeder Schritt von der ersten Erkennung bis zur Aktivierung, Verifikation, Ablösung und endgültigen Bereinigung bleibt explizit, rekonstruierbar und sicher kontrollierbar.