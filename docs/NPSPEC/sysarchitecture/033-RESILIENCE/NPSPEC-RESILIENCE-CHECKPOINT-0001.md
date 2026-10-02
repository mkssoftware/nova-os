# NPSPEC-RESILIENCE-CHECKPOINT-0001 – Nova Resilience Checkpoint

## Status

Angenommen

## Kategorie

Resilience / Recovery / Checkpoint

## Zweck

NovaOS definiert ein systemweites Checkpoint-Modell zur kontrollierten Sicherung eines wiederherstellbaren Systemzustands.

```text
Running State
     ↓
Checkpoint
     ↓
Validated Recovery State
     ↓
Failure
     ↓
Restore
     ↓
Verify
```

Checkpoints ermöglichen eine gezielte Wiederherstellung, ohne eine Komponente vollständig aus ihrem ursprünglichen Grundzustand neu aufbauen zu müssen.

## Grundprinzipien

```text
Checkpoint ≠ Backup
Checkpoint ≠ Snapshot
Checkpoint ≠ Transaction
Checkpoint ≠ Recovery
Checkpoint Created ≠ Checkpoint Valid
Restored ≠ Healthy
Old State ≠ Valid State
```

## Checkpoint Model

```text
Checkpoint
├── CheckpointID
├── TargetID
├── DomainID
├── Version
├── State
├── Timestamp
└── Integrity
```

Optional:

```text
ExecutionID
TransactionID
ParentCheckpointID
DependencySet
ResourceState
CapabilityState
ProviderState
CompatibilityInfo
ProvenanceID
```

## Checkpoint Scope

Checkpoints können unterschiedliche Bereiche erfassen:

```text
Task
Process
Service
Provider
Subsystem
Distributed Service
System
```

Es soll nur der für Recovery erforderliche Zustand gespeichert werden.

## Checkpoint Content

Ein Checkpoint kann enthalten:

```text
Execution State
Memory State
Application State
Open Resource Metadata
Configuration
Dependency References
Version Information
Recovery Metadata
```

Nicht jeder Laufzeitzustand darf direkt übernommen werden.

## External Resources

Externe Ressourcen benötigen besondere Behandlung:

```text
Network Connections
Devices
Files
IPC Channels
DMA
Remote Providers
Locks
```

Diese können nach Restore nicht zwangsläufig im ursprünglichen Zustand existieren.

Deshalb müssen sie:

```text
Revalidate
Reconnect
Reopen
Rebind
Reinitialize
```

werden.

## Creation

Checkpoint-Erstellung folgt einem kontrollierten Ablauf:

```text
Request
   ↓
Prepare
   ↓
Quiesce / Coordinate
   ↓
Capture State
   ↓
Validate
   ↓
Commit
```

Unvollständige Checkpoints dürfen nicht als gültige Recovery Points veröffentlicht werden.

## Consistency

Der gespeicherte Zustand muss intern konsistent sein.

Bei mehreren Komponenten kann Koordination erforderlich sein:

```text
Component A
Component B
Component C
     ↓
Consistent Recovery Point
```

Ein globaler System-Stopp ist dafür nicht grundsätzlich erforderlich.

## Integrity

Checkpoints müssen gegen Beschädigung erkennbar geschützt sein.

Mögliche Mechanismen:

```text
Checksums
Hashes
Authenticated Metadata
Signatures
Version Validation
```

```text
Readable Checkpoint ≠ Valid Checkpoint
```

## Compatibility

Vor Restore muss geprüft werden:

```text
Checkpoint Version
Software Version
State Schema
Architecture
Provider Compatibility
Dependency Compatibility
```

Live Evolution kann explizite State Transformation erlauben.

Inkompatibler Zustand darf nicht stillschweigend übernommen werden.

## Restore

```text
Select Checkpoint
      ↓
Validate
      ↓
Prepare Target
      ↓
Restore State
      ↓
Reconnect Resources
      ↓
Verify
```

Ein Restore muss transaktional behandelt werden können.

## Capability State

Capabilities dürfen nicht blind aus einem alten Checkpoint wiederhergestellt werden.

```text
Stored Capability Reference
          ↓
Revalidation
          ↓
Valid Authority?
```

Widerrufene, abgelaufene oder anderweitig ungültige Authority darf durch Restore nicht wieder aktiviert werden.

## Security und Trust

Vor Restore müssen relevante Zustände erneut geprüft werden:

```text
Integrity
Trust
Capabilities
Security Policy
Sovereignty
```

```text
Previously Trusted ≠ Currently Trusted
```

## Checkpoint Policy

Checkpoint-Erstellung kann abhängig sein von:

```text
Criticality
Recovery Target
State Change
Time Interval
Resource Budget
Execution Contract
User Policy
```

## Incremental Checkpoints

NovaOS darf inkrementelle Checkpoints unterstützen.

```text
Checkpoint A
     ↓
Delta B
     ↓
Delta C
```

Abhängigkeiten zwischen Checkpoints müssen dabei erhalten bleiben.

## Retention

Nicht benötigte Checkpoints sollen kontrolliert entfernt werden.

Retention kann berücksichtigen:

```text
Age
Count
Storage Budget
Criticality
Recovery Policy
Dependency
```

Mindestens ein erforderlicher Recovery Point darf nicht versehentlich durch normale Reclamation verloren gehen.

## Failure During Checkpoint

Ein Fehler während der Erstellung darf den letzten gültigen Checkpoint nicht beschädigen.

```text
Valid Checkpoint A
       ↓
Creating B
       ↓ failure
Discard B
       ↓
A remains valid
```

## Distributed Checkpoints

Verteilte Checkpoints müssen Konsistenz und partielle Fehler berücksichtigen.

```text
Node A ─┐
Node B ─┼→ Recovery Point
Node C ─┘
```

NovaOS darf keine perfekte globale Uhr oder atomaren globalen Snapshot voraussetzen.

Kausale und transaktionale Beziehungen müssen erhalten bleiben.

## Realtime Integration

Checkpoint-Erstellung darf Hard-Realtime-Garantien nicht unkontrolliert verletzen.

Checkpoint-Arbeit kann:

```text
Deferred
Budgeted
Incremental
Background
```

ausgeführt werden.

## Recovery Integration

```text
Failure
   ↓
Contain
   ↓
Diagnose
   ↓
Select Checkpoint
   ↓
Restore
   ↓
Revalidate
   ↓
Verify
```

Ein Checkpoint ist lediglich ein möglicher Recovery Point.

## Verification

Nach Restore müssen mindestens geprüft werden:

```text
State Integrity
Dependencies
Capabilities
Resources
Execution Contracts
Health
```

```text
Restore Complete ≠ Recovery Complete
```

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
CheckpointID
TargetID
DomainID
Version
Creation Time
Integrity State
Compatibility State
Dependencies
Storage Cost
Recovery Eligibility
Last Verification
```

## Normative Anforderungen

1. NovaOS MUSS kontrollierte Checkpoints für wiederherstellbare Systemzustände unterstützen können.
2. Checkpoint MUSS von Backup, Snapshot, Transaction und Recovery getrennt bleiben.
3. Checkpoints MÜSSEN stabile Identitäten besitzen.
4. Der Checkpoint Scope MUSS explizit definierbar sein.
5. Unvollständige Checkpoints DÜRFEN NICHT als gültige Recovery Points gelten.
6. Checkpoint-Zustände MÜSSEN auf Integrität prüfbar sein.
7. Externe Ressourcen MÜSSEN nach Restore revalidiert werden.
8. Inkompatible Checkpoints DÜRFEN NICHT stillschweigend wiederhergestellt werden.
9. Capability Authority MUSS nach Restore erneut validiert werden.
10. Widerrufene Capabilities DÜRFEN durch Restore NICHT reaktiviert werden.
11. Security-, Trust- und Sovereignty-Zustände MÜSSEN erneut geprüft werden.
12. Restore MUSS transaktional behandelbar sein.
13. Fehler bei Checkpoint-Erstellung DÜRFEN den letzten gültigen Checkpoint NICHT beschädigen.
14. Inkrementelle Checkpoints MÜSSEN ihre Abhängigkeiten erhalten.
15. Checkpoint Retention MUSS Resource Budgets berücksichtigen.
16. Distributed Checkpoints MÜSSEN partielle Fehler und Konsistenz berücksichtigen.
17. Distributed Checkpoints DÜRFEN keine perfekte globale Uhr voraussetzen.
18. Checkpoint-Erstellung DARF Hard-Realtime-Garantien NICHT unkontrolliert verletzen.
19. Restore MUSS vor Rückkehr zum Normalbetrieb verifiziert werden.
20. Checkpoint- und Restore-Zustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-FAILUREDOMAIN-0001`
- `NPSPEC-RESILIENCE-RESTART-0001`
- `NPSPEC-RESILIENCE-FAILOVER-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-PROCESS-CHECKPOINT-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-CAPABILITY-REVOCATION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-RESOURCE-ECONOMY-0001`
- `NPSPEC-REALTIME-DEADLINE-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0122`

## Ergebnis

```text
Healthy Execution
       ↓
Create Checkpoint
       ↓
Validate + Commit
       ↓
Failure
       ↓
Contain + Diagnose
       ↓
Select Valid Checkpoint
       ↓
Restore
       ↓
Revalidate External State
       ↓
Verify
   ├── Valid → Resume
   └── Invalid → Escalate
```

NovaOS erhält damit einen kontrollierten Checkpoint-Mechanismus, der konsistente Recovery Points erzeugt, deren Integrität und Kompatibilität sicherstellt und nach einem Fehler eine gezielte Wiederherstellung ermöglicht, ohne veraltete Authority oder ungültigen externen Zustand ungeprüft zu reaktivieren.