# NPSPEC-TRANSACTION-LOG-0001 – Nova Transaction Log

## Status

Angenommen

## Kategorie

Transaction / Logging / Recovery

## Zweck

NovaOS definiert ein persistentes Transaction Log zur nachvollziehbaren und wiederherstellbaren Aufzeichnung kritischer Transaktionszustände.

```text
Transaction
    ↓
Transaction Log
    ↓
Durable State
    ↓
Crash / Restart
    ↓
Recovery
```

Das Transaction Log ermöglicht nach Absturz, Stromausfall oder partiellen Fehlern die Bestimmung, welche Transaktionen committed, abgebrochen, unvollständig oder unbekannt sind.

## Grundprinzipien

```text
Transaction Log ≠ Debug Log
Transaction Log ≠ Audit Log
Transaction Log ≠ Provenance
Log Entry ≠ Commit
Written ≠ Durable
Commit Recorded ≠ Verified
Missing Entry ≠ Abort
```

## Log Model

```text
TransactionLogEntry
├── LogSequenceID
├── TransactionID
├── RecordType
├── TransactionState
├── Timestamp
├── Integrity
└── Payload
```

Optional:

```text
ParentTransactionID
ParticipantID
OperationID
PreviousState
NewState
CommitID
ExecutionContractID
ProvenanceID
```

## Record Types

Das Transaction Log kann unter anderem enthalten:

```text
BEGIN
OPERATION
PREPARE
PREPARED
COMMIT_DECISION
COMMIT
ABORT
COMPENSATE
CHECKPOINT
VERIFY
COMPLETE
RECOVERY
```

Subsysteme dürfen zusätzliche typisierte Records definieren.

## Log Sequence

Einträge müssen eindeutig geordnet werden können.

```text
LSN 100 → BEGIN
LSN 101 → OPERATION
LSN 102 → PREPARED
LSN 103 → COMMIT
LSN 104 → COMPLETE
```

`LogSequenceID` muss monoton innerhalb seines definierten Log-Kontexts sein.

## Write-Ahead Principle

Wenn Recovery von einem Log Record abhängt, muss dieser ausreichend persistent sein, bevor die entsprechende irreversible Zustandsänderung sichtbar wird.

```text
Write Intent
    ↓
Persist Log
    ↓
Apply Change
```

```text
State Change Before Required Durable Record
= Invalid Recovery Ordering
```

## Durability

NovaOS muss zwischen geschriebenen und tatsächlich persistenten Records unterscheiden.

```text
Buffered
   ↓
Written
   ↓
Durable
```

Flush-, Barrier- und Storage-Garantien müssen entsprechend dem verwendeten Provider berücksichtigt werden.

## Commit Record

Eine persistente Commit Decision muss eindeutig identifizierbar sein.

```text
Prepared
   ↓
Durable Commit Decision
   ↓
Commit Effects
```

Ein Commit Record bedeutet jedoch nicht automatisch, dass alle Auswirkungen bereits vollständig angewendet oder verifiziert wurden.

## Crash Recovery

Nach einem Neustart:

```text
Read Transaction Log
        ↓
Validate Records
        ↓
Reconstruct Transactions
        ↓
Determine State
        ↓
Redo / Abort / Compensate / Recover
```

Mögliche rekonstruierte Zustände:

```text
Committed
Aborted
Incomplete
Recovering
Unknown
```

## Redo

Committed, aber noch nicht vollständig angewendete Operationen können erneut ausgeführt werden, sofern deren Semantik dies erlaubt.

```text
Committed
   +
Incomplete Effect
   ↓
Redo
```

Redo muss idempotent oder anderweitig gegen doppelte Effekte geschützt sein.

## Undo und Compensation

Nicht committed Änderungen können, sofern möglich, zurückgesetzt werden.

```text
Uncommitted Change
       ↓
Undo / Rollback
```

Für irreversible externe Effekte:

```text
Compensation
```

```text
Compensation ≠ Exact Undo
```

## Checkpoints

Transaction Logs können Checkpoints verwenden.

```text
Old Log Records
      ↓
Checkpoint
      ↓
Known Consistent State
```

Dadurch muss Recovery nicht unbegrenzt die gesamte Historie durchsuchen.

Ein Checkpoint darf nur verwendet werden, wenn seine Konsistenz und Integrität validiert wurden.

## Log Truncation

Nicht mehr benötigte Records dürfen kontrolliert entfernt werden.

Voraussetzungen können sein:

```text
Transaction Completed
State Durable
Checkpoint Valid
Recovery Dependency Removed
Retention Policy Satisfied
```

Benötigte Recovery-Informationen dürfen nicht vorzeitig gelöscht werden.

## Multi-Service Transactions

Das Log kann Zustände mehrerer Participants erfassen:

```text
TransactionID
├── Service A → Prepared
├── Service B → Committed
└── Service C → Unknown
```

Dadurch kann Multi-Service-Recovery den tatsächlichen bekannten Zustand rekonstruieren.

## Distributed Transactions

Verteilte Transaction Logs müssen lokale und remote Zustände unterscheiden.

```text
Local Durable State ≠ Global Transaction State
```

Ein lokaler Commit Record beweist nicht automatisch den globalen Abschluss einer verteilten Transaktion.

## Integrity

Transaction Logs müssen gegen unbeabsichtigte Beschädigung geschützt werden können.

Mögliche Mechanismen:

```text
Checksums
Sequence Validation
Record Length Validation
Versioning
Authenticated Metadata
```

Beschädigte Records dürfen nicht ungeprüft für Recovery verwendet werden.

## Security

Transaction Logs können sensible Metadaten enthalten.

Sie dürfen insbesondere keine unnötigen:

```text
Capability Tokens
Credentials
Private Keys
Secrets
Sensitive Payloads
```

persistieren.

Zugriff muss Capability-basiert kontrolliert werden.

## Storage Failure

Bei Log-Fehlern:

```text
Log Write Failure
      ↓
Transaction Cannot Establish Required Durability
      ↓
Abort / Degrade / Recover
```

Eine Transaktion darf keine Durability behaupten, wenn die erforderliche Log-Persistenz nicht bestätigt werden konnte.

## Performance

Transaction Logging soll unterstützen:

```text
Batching
Group Commit
Sequential Writes
Asynchronous Flush
Checkpointing
```

Optimierungen dürfen erforderliche Durability- oder Ordering-Garantien nicht verletzen.

## Determinismus

Transaction Logs können Deterministic Replay unterstützen.

Dafür können aufgezeichnet werden:

```text
TransactionID
Operation Order
Commit Order
Relevant Decisions
External Results
```

Transaction Log und Replay Log dürfen getrennte Strukturen bleiben.

## Provenance

Transaction Log und Provenance ergänzen sich:

```text
Transaction Log
→ Was muss für Recovery bekannt sein?

Provenance
→ Wie und warum entstand der Zustand?
```

Recovery-kritische Informationen dürfen nicht ausschließlich von optionaler Provenance abhängen.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
Log State
Current LogSequenceID
Active Transactions
Durable Commit Point
Checkpoint
Recovery Position
Pending Flushes
Integrity State
Storage Provider
```

Sensitive Log Payloads müssen dabei geschützt bleiben.

## Normative Anforderungen

1. NovaOS MUSS persistentes Transaction Logging für recovery-kritische Transaktionen unterstützen.
2. Transaction Log und Debug Log MÜSSEN getrennte Konzepte bleiben.
3. Records MÜSSEN Transaction IDs besitzen.
4. Records MÜSSEN eindeutig geordnet werden können.
5. Recovery-kritische Records MÜSSEN vor abhängigen irreversiblen Zustandsänderungen ausreichend persistent sein.
6. `Written` und `Durable` MÜSSEN unterscheidbar sein.
7. Persistente Commit Decisions MÜSSEN eindeutig identifizierbar sein.
8. Commit Records DÜRFEN NICHT automatisch als Verification interpretiert werden.
9. Crash Recovery MUSS Transaktionszustände rekonstruieren können.
10. Fehlende Records DÜRFEN NICHT automatisch als Abort interpretiert werden.
11. Redo MUSS gegen unbeabsichtigte doppelte Effekte geschützt sein.
12. Irreversible Operationen MÜSSEN Compensation unterstützen können.
13. Checkpoints SOLLEN Recovery-Zeiten begrenzen können.
14. Log Truncation DARF benötigte Recovery-Informationen NICHT entfernen.
15. Distributed Logs MÜSSEN lokalen und globalen Zustand unterscheiden.
16. Beschädigte Log Records DÜRFEN NICHT ungeprüft verwendet werden.
17. Transaction Logs MÜSSEN Capability-basiert geschützt werden.
18. Secrets und Capability Tokens DÜRFEN NICHT unnötig persistiert werden.
19. Fehlgeschlagene Log-Persistenz DARF NICHT als erfolgreiche Durability dargestellt werden.
20. Performance-Optimierungen DÜRFEN Ordering- und Durability-Garantien NICHT verletzen.
21. Transaction Logging SOLL Deterministic Recovery und Replay unterstützen können.
22. Transaction-Log-Zustände MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-CONFIG-0001`
- `NPSPEC-TRANSACTION-RESOURCE-0001`
- `NPSPEC-TRANSACTION-MULTISERVICE-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-CHECKSUM-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-DISTCOMM-IDEMPOTENCY-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-RECOVERYPOLICY-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-REALTIME-REPLAY-0001`
- `NPSPEC-OBSERVABILITY-PROVENANCE-0001`
- `ADR-ARCH-0137`

## Ergebnis

```text
Transaction
    ↓
Record Intent
    ↓
Persist Required State
    ↓
Execute / Commit
    ↓
Record Result
    ↓
Crash?
├── No  → Verify → Complete
└── Yes
      ↓
Read Log
      ↓
Reconstruct State
      ↓
Redo / Abort / Compensate / Recover
      ↓
Verify
```

NovaOS erhält damit ein persistentes Transaction Log, das kritische Zustandsübergänge so aufzeichnet, dass Transaktionen nach Abstürzen und partiellen Fehlern zuverlässig rekonstruiert, fortgesetzt, zurückgesetzt oder kompensiert werden können.