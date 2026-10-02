# NPSPEC-TRANSACTION-ROLLBACK-0001 – Nova Transaction Rollback

## Status

Angenommen

## Kategorie

Transaction / Rollback / Recovery

## Zweck

NovaOS definiert ein einheitliches Rollback-Modell für Transaktionen, deren Änderungen nach Fehler, Abbruch oder fehlgeschlagener Verification kontrolliert zurückgenommen werden müssen.

```text
Transaction
    ↓
State Change
    ↓
Failure / Abort
    ↓
Rollback
    ↓
Previous Valid State
    ↓
Verify
```

Rollback stellt nicht blind einen früheren Zustand wieder her, sondern führt eine kontrollierte und validierte Zustandsrücknahme durch.

## Grundprinzipien

```text
Rollback ≠ Undo
Rollback ≠ Compensation
Rollback ≠ Recovery
Rollback ≠ Snapshot Restore
Previous State ≠ Valid State
Rolled Back ≠ Verified
```

Nicht jede Operation ist reversibel.

## Rollback Model

```text
TransactionRollback
├── RollbackID
├── TransactionID
├── TargetID
├── CurrentState
├── TargetState
├── RollbackScope
└── State
```

Optional:

```text
CheckpointID
SnapshotID
PreviousVersionID
FailureID
DependencySet
RollbackPolicy
RollbackBudget
VerificationPolicy
ProvenanceID
```

## Zustände

```text
Requested
Validating
Preparing
RollingBack
Restoring
Verifying
Completed
PartiallyCompleted
Failed
Escalated
Unknown
```

## Rollback Sources

Ein Rollback kann auf unterschiedlichen Zustandsquellen basieren:

```text
Previous Version
Transaction Log
Checkpoint
Snapshot
Previous Configuration
A/B State
Known-Good State
```

Die Quelle muss vor Verwendung validiert werden.

## Rollback Scope

Rollback kann begrenzt werden auf:

```text
Operation
Object
Resource
Configuration
Service
Provider
Storage State
Subsystem
Transaction
```

Der kleinstmögliche ausreichende Scope soll bevorzugt werden.

## Ablauf

```text
Rollback Request
      ↓
Determine Target State
      ↓
Validate
      ↓
Prepare
      ↓
Quiesce Affected Operations
      ↓
Restore Previous State
      ↓
Revalidate Dependencies
      ↓
Verify
```

## Validation

Vor Rollback müssen relevante Eigenschaften geprüft werden:

```text
Integrity
Version Compatibility
Dependencies
Capabilities
Security
Trust
Sovereignty
Resource Availability
Transaction State
```

```text
Old State ≠ Automatically Valid State
```

## Transaction Log Integration

Das Transaction Log kann bestimmen, welche Änderungen tatsächlich durchgeführt wurden.

```text
Transaction Log
      ↓
Applied Operations
      ↓
Rollback Plan
```

Fehlende oder unbekannte Log-Zustände dürfen nicht durch Annahmen ersetzt werden.

## Reversibility

Operationen sollen hinsichtlich ihrer Reversibilität klassifiziert werden können:

```text
Fully Reversible
Conditionally Reversible
Compensatable
Irreversible
Unknown
```

Bei `Irreversible` oder `Unknown` darf kein vollständiges Rollback versprochen werden.

## Compensation

Externe Effekte können häufig nicht zurückgesetzt werden.

Beispiele:

```text
Network Message Sent
Remote Modification
Physical Device Action
External Transaction
```

Dann kann erforderlich sein:

```text
Rollback Local State
        +
Compensate External Effect
```

```text
Compensation ≠ Restoration of History
```

## Dependency Handling

Rollback eines Zustands kann abhängige Komponenten beeinflussen.

```text
Service A v4
     ↓
Depends on Config B v7
```

Wird `Config B` auf `v6` zurückgesetzt, muss geprüft werden, ob `Service A v4` weiterhin kompatibel ist.

## Capability State

Rollback darf widerrufene oder abgelaufene Authority nicht wiederherstellen.

```text
Capability Valid at T1
        ↓
Revoked at T2
        ↓
Rollback to T1

Capability remains revoked
```

```text
State Rollback ≠ Authority Rollback
```

## Security Rollback Protection

Bestimmte sicherheitskritische Zustände dürfen nicht zurückgesetzt werden.

Beispiele:

```text
Revocation State
Minimum Security Version
Trust Decisions
Compromised Keys
Security Counters
Firmware Rollback Protection
```

Security Policy entscheidet, welche Zustände monoton bleiben müssen.

## Resource Rollback

Ressourcenänderungen müssen ihren tatsächlichen Zustand berücksichtigen.

```text
Reserved
Allocated
Mapped
Active
Released
```

Eine bereits anderweitig vergebene Ressource darf nicht durch Rollback erneut als verfügbar angenommen werden.

## Multi-Service Rollback

Bei mehreren Services kann vollständiges Rollback unmöglich sein.

```text
Service A → Rolled Back
Service B → Compensated
Service C → Unknown
```

Dieser Zustand muss explizit dargestellt werden.

## Distributed Rollback

Verteiltes Rollback muss berücksichtigen:

```text
Consistency
Replication
Causality
Membership
Network Partitions
Participant State
```

NovaOS setzt kein universelles globales Rollback voraus.

## Rollback Budget

Rollback kann begrenzt werden durch:

```text
Time
Attempts
CPU
Memory
IO
Network
Recovery Deadline
```

Ein erschöpftes Budget kann zu Recovery Policy, Degradation oder Recovery Mode eskalieren.

## Rollback Loops

Wiederholte Wechsel zwischen Zuständen müssen erkannt werden.

```text
State A
  ↓
State B
  ↓ failure
State A
  ↓ retry
State B
  ↓ failure
...
```

Mögliche Maßnahmen:

```text
Attempt Limit
Cooldown
Disable Update
Select Alternative State
Degrade
Escalate
```

## Verification

Nach Rollback müssen mindestens relevante Eigenschaften geprüft werden:

```text
State Integrity
Dependencies
Capabilities
Resources
Security
Trust
Execution Contracts
Health
```

```text
Rollback Completed ≠ Recovery Completed
```

Erst erfolgreiche Verification erlaubt die Bestätigung eines gültigen Zustands.

## Failure Handling

Schlägt Rollback fehl:

```text
Rollback Failed
      ↓
Contain
      ↓
Recovery Policy
      ↓
Alternative Rollback
Checkpoint Restore
Failover
Degradation
Recovery Mode
```

Ein teilweise zurückgerollter Zustand darf nicht als normaler Zustand verborgen werden.

## Provenance

Rollback muss nachvollziehbar machen:

```text
RollbackID
TransactionID
Reason
Source State
Target State
Operations Reverted
Operations Compensated
Remaining Effects
Verification Result
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
RollbackID
TransactionID
Rollback Scope
Current State
Target State
Rollback Source
Progress
Irreversible Effects
Compensation State
Rollback Budget
Verification State
```

## Normative Anforderungen

1. NovaOS MUSS kontrolliertes Transaction Rollback unterstützen.
2. Rollback MUSS von Undo, Compensation und Recovery getrennt bleiben.
3. Rollback Targets MÜSSEN vor Verwendung validiert werden.
4. Der kleinstmögliche ausreichende Rollback Scope SOLL bevorzugt werden.
5. Operationen SOLLEN hinsichtlich ihrer Reversibilität klassifizierbar sein.
6. Irreversible Operationen DÜRFEN NICHT als vollständig rollbackfähig dargestellt werden.
7. Transaction Logs SOLLEN zur Bestimmung tatsächlich ausgeführter Änderungen verwendet werden können.
8. Unbekannte Transaction States DÜRFEN NICHT durch angenommene Zustände ersetzt werden.
9. Dependencies MÜSSEN nach Rollback revalidierbar sein.
10. Rollback DARF widerrufene oder abgelaufene Capabilities NICHT wiederherstellen.
11. Security-kritische monotone Zustände MÜSSEN vor Rollback geschützt werden können.
12. Resource Rollback MUSS den aktuellen tatsächlichen Ressourcenstatus berücksichtigen.
13. Multi-Service Rollback MUSS partielle Ergebnisse darstellen können.
14. NovaOS DARF kein universelles globales Distributed Rollback voraussetzen.
15. Rollback MUSS durch ein Rollback Budget begrenzbar sein.
16. Wiederholte Rollback-Loops MÜSSEN erkannt und begrenzt werden können.
17. Externe irreversible Effekte MÜSSEN über Compensation behandelbar sein.
18. Partielle Rollbacks DÜRFEN NICHT als vollständiger Erfolg dargestellt werden.
19. Nach Rollback MUSS Verification möglich sein.
20. Fehlgeschlagenes Rollback MUSS an Recovery Policy eskalierbar sein.
21. Rollback Completed DARF NICHT automatisch Recovery Completed bedeuten.
22. Rollback-Zustände MÜSSEN autorisiert introspektierbar und nachvollziehbar sein.

## Abhängigkeiten

- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-CONFIG-0001`
- `NPSPEC-TRANSACTION-RESOURCE-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-0001`
- `NPSPEC-TRANSACTION-LOG-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0138`

## Ergebnis

```text
Transaction Failure
        ↓
Determine Actual State
        ↓
Select Valid Rollback Target
        ↓
Validate
        ↓
Rollback Reversible State
        +
Compensate Irreversible Effects
        ↓
Revalidate Dependencies + Authority
        ↓
Verify
├── Valid → Recovered State
└── Invalid
      ↓
Recovery Policy / Escalation
```

NovaOS erhält damit ein kontrolliertes Transaction-Rollback-Modell, das fehlgeschlagene Zustandsänderungen soweit tatsächlich möglich zurücknimmt, irreversible Effekte explizit behandelt und erst nach erfolgreicher Verification einen wiederhergestellten Zustand bestätigt.