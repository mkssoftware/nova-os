# NPSPEC-RESILIENCE-ROLLBACK-0001 – Nova Resilience Rollback

## Status

Angenommen

## Kategorie

Resilience / Recovery / Rollback

## Zweck

NovaOS definiert einen kontrollierten Rollback-Mechanismus, mit dem fehlerhafte oder unerwünschte Zustandsänderungen auf einen zuvor validierten Zustand zurückgesetzt werden können.

```text
Valid State A
    ↓
Change
    ↓
State B
    ↓
Failure
    ↓
Rollback
    ↓
State A'
    ↓
Verify
```

Rollback ist eine Recovery-Strategie und darf nicht automatisch mit vollständiger Wiederherstellung gleichgesetzt werden.

## Grundprinzipien

```text
Rollback ≠ Undo
Rollback ≠ Restore
Rollback ≠ Checkpoint
Rollback ≠ Backup
Rollback ≠ Recovery
Previous State ≠ Valid State
Rolled Back ≠ Healthy
```

## Rollback Model

```text
RollbackOperation
├── RollbackID
├── TargetID
├── DomainID
├── SourceState
├── TargetState
├── Reason
└── State
```

Optional:

```text
TransactionID
CheckpointID
SnapshotID
VersionID
FailureID
DependencySet
ExecutionContractID
RollbackBudget
ProvenanceID
```

## Rollback Sources

Rollback kann auf unterschiedlichen Zustandsquellen basieren:

```text
Previous Version
Transaction State
Checkpoint
Snapshot
Configuration Version
Known-Good State
A/B System Slot
```

Die Quelle muss vor Verwendung validiert werden.

## Rollback Scope

Rollback kann unterschiedliche Bereiche betreffen:

```text
Object
Configuration
Process
Service
Provider
Driver
Storage
Subsystem
System
```

Es gilt:

```text
Smallest Sufficient Rollback Scope
```

Ein lokaler Fehler soll nicht unnötig einen systemweiten Rollback verursachen.

## Rollback Lifecycle

```text
Request
   ↓
Select Target State
   ↓
Validate
   ↓
Prepare
   ↓
Quiesce
   ↓
Rollback
   ↓
Revalidate
   ↓
Verify
   ↓
Commit / Escalate
```

## Target Validation

Vor Rollback muss geprüft werden:

```text
Integrity
Version Compatibility
Dependency Compatibility
Security Policy
Trust State
Sovereignty
Resource Availability
```

Ein älterer Zustand ist nicht automatisch ein zulässiger Zustand.

## Transactional Rollback

Rollback soll selbst transaktional ausgeführt werden können.

```text
Prepare
   ↓
Apply
   ↓
Verify
   ├── Valid → Commit
   └── Invalid → Abort / Escalate
```

Ein fehlgeschlagener Rollback darf das System nicht in einem undefinierten Zwischenzustand zurücklassen.

## State Dependencies

Zustände können voneinander abhängig sein.

```text
State A
├── Object Version
├── Configuration
├── Provider
└── Dependency
```

Wird nur ein Teil zurückgesetzt, müssen abhängige Zustände weiterhin kompatibel sein.

## External State

Nicht jeder externe Effekt kann zurückgerollt werden.

Beispiele:

```text
Sent Network Message
Physical Device Action
External Payment
Printed Output
Remote System Modification
```

```text
Local Rollback ≠ External Undo
```

Nicht reversible Effekte müssen explizit behandelt werden.

## Capability Handling

Alte Authority darf durch Rollback nicht wiederhergestellt werden.

```text
Old Capability
      ↓
Rollback
      X
Automatic Reactivation
```

Capabilities müssen nach Rollback erneut validiert werden.

Widerrufene oder abgelaufene Authority bleibt ungültig.

## Security Rollback Protection

Angreifer dürfen Rollback nicht verwenden, um bekannte Sicherheitskorrekturen zu umgehen.

NovaOS muss Rollback-Schutz unterstützen für:

```text
Security Version
Key State
Revocation State
Trust Policy
Firmware State
Boot State
```

```text
Older ≠ Allowed
```

## Boot Rollback

NovaOS kann bei fehlgeschlagenem Boot auf einen bekannten funktionierenden Systemzustand zurückkehren.

```text
Slot A
  ↓ Update
Slot B
  ↓ Boot Failure
Rollback
  ↓
Slot A
```

Boot Health entscheidet, ob eine neue Version dauerhaft akzeptiert wird.

## Storage Rollback

Storage Rollback kann auf:

```text
Snapshots
Versioned Objects
Transaction Logs
Filesystem State
```

basieren.

Neuere Daten dürfen nicht unbeabsichtigt überschrieben werden.

## Distributed Rollback

Bei verteilten Systemen ist ein lokaler Rollback nicht automatisch global konsistent.

```text
Node A → Version 5
Node B → Version 5
Node C → Rollback Version 4
```

NovaOS muss:

```text
Consistency
Replication
Transactions
Causality
Membership
```

berücksichtigen.

Ein universeller globaler Rollback wird nicht vorausgesetzt.

## Realtime Integration

Rollback kann zeitliche Garantien beeinflussen.

```text
Rollback Duration
      +
Reinitialization
      +
Verification
      ↓
Recovery Deadline
```

Hard-Realtime-Systeme dürfen nur Rollback-Pfade verwenden, deren zeitliche Eigenschaften ausreichend begrenzt sind.

## Rollback Budget

Rollback muss ressourcenbegrenzt sein.

```text
RollbackBudget
├── MaxAttempts
├── MaximumTime
├── CPU
├── Memory
├── IO
└── Storage
```

Wiederholte erfolglose Rollbacks müssen eskalieren.

## Rollback Loop Protection

NovaOS muss Schleifen verhindern:

```text
Version A
   ↓
Version B
   ↓ failure
Version A
   ↓ automatic update
Version B
   ↓ failure
...
```

Mögliche Gegenmaßnahmen:

```text
Failure Counter
Version Quarantine
Cooldown
Update Suppression
Manual Confirmation
Safe Mode
```

## Verification

Nach Rollback muss geprüft werden:

```text
Integrity
Health
Dependencies
Capabilities
Trust
Configuration
Execution Contracts
```

```text
Rollback Completed ≠ Recovery Verified
```

## Escalation

Scheitert Rollback:

```text
Rollback
   ↓ failed
Alternative Checkpoint
   ↓
Restart / Failover
   ↓
Recovery Environment
   ↓
Fail-safe
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
RollbackID
TargetID
Source State
Target State
Reason
Current Phase
Version
Dependencies
Rollback Budget
Integrity State
Verification Result
Final Health State
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierten Rollback auf zuvor validierte Zustände unterstützen können.
2. Rollback MUSS von Undo, Restore, Checkpoint und vollständiger Recovery getrennt bleiben.
3. NovaOS SOLL die kleinste ausreichende Rollback-Domain verwenden.
4. Rollback-Ziele MÜSSEN vor Verwendung validiert werden.
5. Ein älterer Zustand DARF NICHT automatisch als gültig gelten.
6. Rollback SOLL transaktional ausführbar sein.
7. Fehlgeschlagene Rollbacks DÜRFEN keinen undefinierten Zwischenzustand hinterlassen.
8. Abhängige Zustände MÜSSEN nach Rollback revalidiert werden.
9. Nicht reversible externe Effekte MÜSSEN explizit behandelt werden.
10. Lokaler Rollback DARF NICHT als Undo externer Effekte interpretiert werden.
11. Widerrufene oder abgelaufene Capabilities DÜRFEN durch Rollback NICHT reaktiviert werden.
12. Security-, Trust- und Revocation-Zustände MÜSSEN Rollback-Schutz besitzen können.
13. Sicherheitskritische Updates DÜRFEN NICHT unautorisiert zurückgerollt werden.
14. Boot Rollback MUSS mit Boot Health und A/B Boot integrierbar sein.
15. Storage Rollback MUSS neuere Daten und Transaktionszustände berücksichtigen.
16. Distributed Rollback MUSS Consistency und Causality berücksichtigen.
17. NovaOS DARF keinen universellen atomaren globalen Rollback voraussetzen.
18. Rollback-Versuche MÜSSEN durch Budgets begrenzbar sein.
19. Rollback Loops MÜSSEN erkannt und unterbrochen werden können.
20. Rollback MUSS vor Rückkehr zum Normalbetrieb verifiziert werden.
21. `Rolled Back` DARF NICHT automatisch als `Healthy` gelten.
22. Rollback-Zustände und Ergebnisse MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-RESTART-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-BOOT-AB-0001`
- `NPSPEC-BOOT-ROLLBACK-0001`
- `NPSPEC-BOOT-HEALTH-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0123`

## Ergebnis

```text
Failure
   ↓
Contain
   ↓
Select Known-Good State
   ↓
Validate
   ↓
Rollback
   ↓
Revalidate Dependencies
   ↓
Verify
├── Valid → Resume
└── Invalid
      ↓
Alternative Recovery / Escalate
```

NovaOS erhält damit einen kontrollierten Rollback-Mechanismus, der fehlerhafte Zustandsänderungen auf validierte frühere Zustände zurückführen kann, ohne veraltete Authority, Sicherheitszustände oder inkompatible Abhängigkeiten ungeprüft wiederherzustellen.