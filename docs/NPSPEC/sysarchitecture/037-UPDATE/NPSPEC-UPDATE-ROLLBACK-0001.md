# NPSPEC-UPDATE-ROLLBACK-0001 – Nova Update Rollback

## Status

Angenommen

## Kategorie

Update / Rollback / Recovery

## Zweck

NovaOS definiert Update Rollback als kontrollierte Rückkehr zu einem vorherigen bekannten funktionsfähigen Update-Zustand, wenn Installation, Aktivierung oder Verifikation einer neuen Version fehlschlägt.

```text
Known-Good State
      ↓
Update
      ↓
Failure
      ↓
Rollback
      ↓
Recovered State
      ↓
Verify
```

Rollback stellt früheren Inhalt wieder her, ohne State-Historie, aktuelle Sicherheitsentscheidungen oder monotone Security States zurückzusetzen.

## Grundprinzipien

```text
Rollback ≠ Undo
Rollback ≠ Snapshot Restore
Rollback ≠ Compensation
Rollback ≠ History Rewrite
Previous Version ≠ Safe Version
Known-Good ≠ Currently Allowed
Restored ≠ Verified
Rollback Possible ≠ Rollback Safe
```

## Rollback Model

```text
UpdateRollback
├── RollbackID
├── UpdateID
├── TransactionID
├── Scope
├── CurrentVersion
├── TargetVersion
├── Source
├── Policy
└── State
```

Optional:

```text
SnapshotID
ABSlot
StateMigration
Dependencies
SecurityState
VerificationPlan
RecoveryPolicy
ProvenanceID
```

## Rollback Sources

NovaOS darf unterschiedliche Rollback-Quellen verwenden:

```text
Previous Package Version
A/B Slot
Snapshot
Checkpoint
Transaction Log
Known-Good System Image
Previous Configuration
Recovery Environment
```

Die Quelle muss vor Verwendung validiert werden.

## Rollback Scope

Rollback erfolgt im kleinsten ausreichenden Scope.

Beispiele:

```text
Component
Driver
Service
Package Set
Configuration
Subsystem
Boot Environment
System
```

```text
Component Failure
≠
Automatic System-Wide Rollback
```

## Rollback-Auslöser

Rollback kann ausgelöst werden durch:

```text
Installation Failure
Activation Failure
Boot Failure
Health Check Failure
Contract Violation
Integrity Failure
Migration Failure
Dependency Failure
Explicit Recovery Decision
```

## Rollback-Ablauf

```text
Detect Failure
      ↓
Contain
      ↓
Select Rollback Target
      ↓
Validate Target
      ↓
Validate Dependencies
      ↓
Prepare Rollback
      ↓
Apply
      ↓
Reconcile
      ↓
Verify
```

## Target Validation

Vor Rollback müssen mindestens geprüft werden:

```text
Integrity
Compatibility
Dependencies
State Compatibility
Security Policy
Trust State
Revocation State
Minimum Secure Version
Recovery Requirements
```

Ein technisch vorhandener alter Zustand darf nicht automatisch verwendet werden.

## State Versioning

Rollback stellt früheren Inhalt wieder her, erzeugt aber einen neuen State.

```text
State v42
   ↓
Update
   ↓
State v43
   ↓
Rollback Content from v42
   ↓
State v44
```

```text
Rollback ≠ Version Reuse
```

Die Historie bleibt erhalten.

## A/B Rollback

Bei A/B-Systemen kann der vorherige Slot als Rollback-Ziel dienen.

```text
Slot A = Known-Good
Slot B = Failed Update

B
↓
Rollback
↓
A
```

Vor Aktivierung von A muss geprüft werden, ob der Slot weiterhin mit aktuellem State und aktuellen Security-Anforderungen kompatibel ist.

## Snapshot Rollback

Ein Update Snapshot kann als Rollback-Quelle dienen.

```text
Snapshot
   ↓
Validate
   ↓
Restore
   ↓
Reconcile
   ↓
Verify
```

Snapshot Restore ist ein Mechanismus innerhalb des Rollback-Prozesses und nicht mit Rollback selbst gleichzusetzen.

## State Migration

Updates können State-Schemas verändern.

```text
System v1 + State v1
        ↓
Update
        ↓
System v2 + State v2
```

Ein Rollback auf System v1 ist nur zulässig, wenn State v2 kompatibel zurückgeführt oder anderweitig sicher behandelt werden kann.

Migrationen werden klassifiziert:

```text
Reversible
Conditionally Reversible
Compensatable
Irreversible
Unknown
```

## Irreversible Änderungen

Ist vollständiger Rollback nicht möglich:

```text
Rollback Impossible
      ↓
Compensation / Recovery
```

NovaOS darf keinen vollständigen Rollback vortäuschen, wenn irreversible Änderungen bestehen.

## Security Rollback Protection

Monotone Security States dürfen nicht zurückgesetzt werden.

Beispiele:

```text
Revoked Keys
Revoked Certificates
Minimum Secure Version
Compromise Markers
Security Counters
Trust Revocations
```

```text
Old Software State
      +
Current Security State
```

Ein kryptografisch gültiger alter Build kann trotzdem als Rollback-Ziel unzulässig sein.

## Dependency Validation

Rollback muss den resultierenden Dependency Graph prüfen.

```text
Rollback Component A
        ↓
Check B, C, D
        ↓
Consistent?
```

Ein Rollback darf keine inkompatible Kombination aktiver Komponenten erzeugen.

## Transaction Integration

Rollback wird kontrolliert innerhalb des Transaktionsmodells ausgeführt.

```text
Begin Rollback
      ↓
Validate
      ↓
Prepare
      ↓
Apply
      ↓
Verify
      ↓
Commit
```

Ein fehlgeschlagener Rollback muss selbst in Recovery übergehen können.

## Verification

Rollback ist erst erfolgreich, wenn der resultierende Zustand verifiziert wurde.

```text
Rollback Applied
      ↓
Integrity Check
      ↓
Dependency Check
      ↓
Health Check
      ↓
Contract Verification
      ↓
Operational Verification
```

```text
Rollback Applied ≠ Recovery Complete
```

## Rollback Loops

NovaOS muss wiederholte Rollback-Zyklen erkennen.

```text
Update
 ↓
Failure
 ↓
Rollback
 ↓
Update Retry
 ↓
Failure
```

Schwellwerte, Backoff und Recovery Policy müssen endlose Update-/Rollback-Schleifen verhindern.

## Crash Recovery

Der Rollback-Zustand muss persistent rekonstruierbar sein.

Nach Crash:

```text
Rollback Pending
Rollback Applying
Rollback Committed
Verification Pending
Recovery Required
Unknown
```

```text
Unknown ≠ Successful Rollback
```

## Distributed Updates

Bei verteilten Updates darf kein universeller globaler Rollback vorausgesetzt werden.

Stattdessen können verwendet werden:

```text
Local Rollback
Compensation
Versioned Deployment
Reconciliation
Provider Replacement
```

## Provenance

Rollback-Entscheidungen müssen nachvollziehbar sein:

```text
RollbackID
UpdateID
TransactionID
Failure Reason
Source Version
Target Content
Security Validation
Rollback Result
Verification Result
Timestamp
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Rollback Availability
Rollback Target
Rollback Scope
Current State
Security Compatibility
State Compatibility
Dependency Compatibility
Rollback Progress
Verification State
Failure Reason
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierten Update Rollback unterstützen.
2. Jeder Rollback MUSS einen expliziten Scope besitzen.
3. Rollback-Ziele MÜSSEN vor Verwendung validiert werden.
4. Vorherige Version DARF NICHT automatisch als zulässiges Rollback-Ziel gelten.
5. Rollback MUSS Integrität, Trust und Security Policy berücksichtigen.
6. Rollback MUSS Dependency-Kompatibilität prüfen.
7. State-Kompatibilität MUSS vor Rollback geprüft werden.
8. Rollback MUSS State Migration berücksichtigen.
9. Irreversible Änderungen MÜSSEN explizit erkannt werden.
10. Nicht vollständig reversible Änderungen MÜSSEN Compensation oder Recovery ermöglichen.
11. Rollback DARF alte State-Versionen NICHT wiederverwenden.
12. Rollback DARF State-Historie NICHT überschreiben.
13. Monotone Security States DÜRFEN NICHT zurückgesetzt werden.
14. Revoked Credentials DÜRFEN durch Rollback NICHT reaktiviert werden.
15. Minimum-Secure-Version-Regeln MÜSSEN Rollback blockieren können.
16. A/B-Slots MÜSSEN als Rollback-Quelle verwendbar sein können.
17. Update Snapshots MÜSSEN als Rollback-Quelle verwendbar sein können.
18. Rollback MUSS transaktional ausführbar sein.
19. Fehlgeschlagener Rollback MUSS in einen definierten Recovery-Pfad übergehen können.
20. Rollback MUSS nach Anwendung verifiziert werden.
21. `Applied` DARF NICHT als `Verified` interpretiert werden.
22. Wiederholte Update-/Rollback-Schleifen MÜSSEN erkannt und begrenzt werden.
23. Rollback-State MUSS nach Crash rekonstruierbar sein.
24. `Unknown` DARF NICHT als erfolgreicher Rollback interpretiert werden.
25. Verteilte Updates DÜRFEN NICHT universellen globalen Rollback voraussetzen.
26. Rollback-Entscheidungen MÜSSEN nachvollziehbare Provenance besitzen.
27. Rollback-State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-AB-0001`
- `NPSPEC-UPDATE-SNAPSHOT-0001`
- `NPSPEC-STATE-ROLLBACK-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-TRANSACTION-COMPENSATION-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-TRUST-REVOCATION-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `ADR-ARCH-0172`

## Ergebnis

```text
Update Failure
      ↓
Contain Failure
      ↓
Select Rollback Target
      ↓
Validate Target
      ↓
Check State + Dependencies
      ↓
Check Security State
      ↓
Transactional Rollback
      ↓
Reconcile
      ↓
Verify
     ↙   ↘
 Success  Failure
    ↓       ↓
Recovered  Recovery Mode
```

NovaOS erhält damit einen sicheren Update-Rollback-Mechanismus, der frühere bekannte Zustände wiederherstellen kann, ohne Historie, aktuelle Sicherheitsentscheidungen oder monotone Security States unzulässig zurückzusetzen.