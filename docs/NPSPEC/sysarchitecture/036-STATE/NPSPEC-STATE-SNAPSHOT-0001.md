# NPSPEC-STATE-SNAPSHOT-0001 – Nova State Snapshot

## Status

Angenommen

## Kategorie

State / Snapshot / Recovery

## Zweck

NovaOS definiert State Snapshots als konsistente, versionierte Momentaufnahme eines definierten Systemzustands.

Snapshots dienen insbesondere:

```text
Recovery
Rollback
Checkpointing
Debugging
Replay
Migration
Live Evolution
Verification
```

Ein Snapshot ist keine vollständige Kopie des gesamten Systems, sondern umfasst einen explizit definierten State Scope.

## Grundprinzipien

```text
Snapshot ≠ Backup
Snapshot ≠ Checkpoint
Snapshot ≠ Transaction
Snapshot ≠ Current State
Snapshot Created ≠ Snapshot Valid
Snapshot Restore ≠ Rollback
Stored Snapshot ≠ Trusted Snapshot
```

## Snapshot Model

```text
StateSnapshot
├── SnapshotID
├── SnapshotVersion
├── Scope
├── StateVersion
├── CreationTime
├── StateData
├── Dependencies
└── IntegrityState
```

Optional:

```text
OwnerID
TransactionID
ExecutionContractID
ParentSnapshotID
ProvenanceID
SecurityLabel
TrustState
Expiration
StorageLocation
```

## Snapshot Scope

Snapshots müssen einen expliziten Scope besitzen.

Beispiele:

```text
Object
Process
Service
Driver
Resource
Configuration
Subsystem
Transaction
System
Distributed Component
```

```text
Snapshot Scope ≠ Entire System
```

Der kleinste ausreichende Scope soll bevorzugt werden.

## Snapshot Creation

Grundablauf:

```text
Select Scope
    ↓
Determine State
    ↓
Stabilize / Coordinate
    ↓
Capture
    ↓
Validate
    ↓
Persist Metadata
    ↓
Snapshot Ready
```

Ein Snapshot muss erkennen lassen, auf welcher State-Version er basiert.

## Consistency

Snapshots benötigen ein definiertes Konsistenzmodell.

Mögliche Formen:

```text
Atomic
Transaction-consistent
Quiesced
Version-consistent
Crash-consistent
Application-consistent
Best-effort
```

Der Consumer darf keine stärkere Konsistenz annehmen als tatsächlich garantiert wird.

## Concurrent Changes

Das System muss für Snapshot-Erstellung nicht grundsätzlich global angehalten werden.

Mögliche Verfahren:

```text
Copy-on-Write
Versioning
Transactions
Quiescing
Generation Tracking
Immutable State
```

```text
Global Pause ≠ Required
```

## Dependencies

Ein Snapshot kann von anderem State abhängig sein.

```text
Snapshot
├── Object State
├── Resource Mapping
├── Configuration
└── External Dependency
```

Nicht enthaltene Abhängigkeiten müssen explizit beschrieben werden.

## Restore

Ein Snapshot kann als Quelle für eine Wiederherstellung dienen.

```text
Snapshot
   ↓
Validate
   ↓
Check Compatibility
   ↓
Prepare Restore
   ↓
Restore
   ↓
Verify Actual State
```

```text
Snapshot Restore ≠ Automatic Valid State
```

Nach Restore muss der tatsächliche Zustand überprüft werden.

## Rollback

Snapshots können Rollback unterstützen.

```text
Current State
     ↓
Rollback Decision
     ↓
Snapshot
     ↓
Restore
     ↓
Verify
```

Der Snapshot selbst entscheidet nicht, ob Rollback zulässig ist.

Insbesondere monotone Sicherheitszustände dürfen nicht unzulässig zurückgesetzt werden.

## Security State

Folgende Zustände dürfen durch alte Snapshots nicht automatisch rückgängig gemacht werden:

```text
Capability Revocation
Compromised Keys
Minimum Security Version
Trust Revocation
Security Counters
Rollback Protection
```

```text
Old Snapshot ≠ Old Authority Restored
```

## Checkpoints

Ein Checkpoint kann einen oder mehrere Snapshots verwenden.

```text
Checkpoint
├── State Snapshot
├── Execution Position
├── Dependencies
└── Recovery Metadata
```

Snapshot und Checkpoint bleiben getrennte Konzepte.

## Incremental Snapshots

Snapshots dürfen inkrementell gespeichert werden.

```text
Snapshot A
   ↓
Delta
   ↓
Snapshot B
```

Abhängigkeiten zu Basis-Snapshots müssen eindeutig nachvollziehbar sein.

## Storage

Snapshot-Daten können je nach Scope gespeichert werden als:

```text
Memory
Persistent Storage
Versioned Object
Snapshot Volume
Remote Storage
```

Integrität und Zugriffsschutz müssen erhalten bleiben.

## Distributed Snapshots

Verteilte Snapshots benötigen kein universelles globales Freeze.

```text
Node A Snapshot
Node B Snapshot
Node C Snapshot
        ↓
Consistency Metadata
```

Der Snapshot muss erkennen lassen, welche Konsistenzgarantien tatsächlich gelten.

## Live Evolution

State Snapshots können Component Replacement unterstützen.

```text
Old Component
      ↓
Snapshot State
      ↓
Validate Compatibility
      ↓
Transform State
      ↓
New Component
      ↓
Verify
```

```text
ABI Compatible ≠ Snapshot Compatible
```

## Determinismus und Replay

Snapshots können einen definierten Ausgangspunkt für Replay bilden.

```text
Snapshot
   +
Recorded Events
   ↓
Replay
```

Externe nichtdeterministische Eingaben müssen separat erfasst werden.

## Integrity

Snapshots müssen Integritätsprüfung unterstützen.

Beispiele:

```text
Checksum
Hash
Authenticated Metadata
Signature
Version Validation
Dependency Validation
```

Manipulierte oder unvollständige Snapshots dürfen nicht ungeprüft wiederhergestellt werden.

## Capability Security

Snapshot-Zugriff benötigt explizite Authority.

Mögliche Rechte:

```text
CreateSnapshot
ReadSnapshot
RestoreSnapshot
DeleteSnapshot
ExportSnapshot
```

```text
Read State ≠ Restore State
```

Restore benötigt grundsätzlich stärkere Authority als reine Beobachtung.

## Provenance

Kritische Snapshots sollen erfassen:

```text
SnapshotID
Source State
State Version
Creator
Reason
Creation Time
Scope
Transaction
Integrity
Restore History
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
SnapshotID
Version
Scope
Source State
Consistency Model
Creation Time
Dependencies
Integrity State
Compatibility
Storage Location
Restore Eligibility
```

## Normative Anforderungen

1. NovaOS MUSS State Snapshots unterstützen können.
2. Jeder Snapshot MUSS einen expliziten Scope besitzen.
3. Snapshot und Backup MÜSSEN getrennte Konzepte bleiben.
4. Snapshot und Checkpoint MÜSSEN getrennte Konzepte bleiben.
5. Snapshots MÜSSEN ihre zugrunde liegende State-Version identifizieren können.
6. Das Konsistenzmodell eines Snapshots MUSS explizit sein.
7. Consumer DÜRFEN keine stärkere Konsistenz voraussetzen als garantiert wird.
8. Snapshot-Erstellung DARF keinen universellen globalen Systemstopp voraussetzen.
9. Snapshot-Abhängigkeiten MÜSSEN darstellbar sein.
10. Snapshot Restore MUSS Compatibility prüfen können.
11. Restore MUSS den resultierenden Actual State verifizieren.
12. Snapshot Restore DARF NICHT automatisch als Rollback interpretiert werden.
13. Alte Snapshots DÜRFEN monotone Security States NICHT unzulässig zurücksetzen.
14. Snapshot-Zugriff MUSS Capability-basiert kontrollierbar sein.
15. Read Authority DARF NICHT automatisch Restore Authority einschließen.
16. Inkrementelle Snapshots MÜSSEN ihre Basisabhängigkeiten eindeutig referenzieren.
17. Persistente Snapshots MÜSSEN Integritätsprüfung unterstützen.
18. Manipulierte Snapshots DÜRFEN NICHT ungeprüft wiederhergestellt werden.
19. Distributed Snapshots DÜRFEN keine universelle globale Konsistenz voraussetzen.
20. Snapshot Compatibility MUSS von ABI Compatibility getrennt bleiben.
21. Snapshots MÜSSEN als Ausgangspunkt für deterministisches Replay verwendbar sein können.
22. Kritische Snapshots SOLLEN Provenance besitzen.
23. Snapshots MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STATE-GLOBAL-0001`
- `NPSPEC-STATE-DESIRED-0001`
- `NPSPEC-STATE-ACTUAL-0001`
- `NPSPEC-STATE-MACHINE-0001`
- `NPSPEC-STATE-RECONCILIATION-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-ARCH-DETERMINISM-0001`
- `NPSPEC-ARCH-LIVEEVOLUTION-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `ADR-ARCH-0158`

## Ergebnis

```text
Defined State Scope
       ↓
Coordinate State
       ↓
Capture Snapshot
       ↓
Validate Integrity
       ↓
Version + Provenance
       ↓
Store
       ↓
Recovery / Replay / Migration
       ↓
Restore
       ↓
Verify Actual State
```

NovaOS erhält damit ein einheitliches Snapshot-Modell für konsistente und überprüfbare Zustandsaufnahmen, ohne Snapshots mit Backups, Checkpoints oder Rollbacks gleichzusetzen und ohne veraltete Sicherheitszustände unkontrolliert wiederherzustellen.