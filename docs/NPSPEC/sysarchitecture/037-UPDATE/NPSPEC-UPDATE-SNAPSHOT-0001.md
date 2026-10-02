# NPSPEC-UPDATE-SNAPSHOT-0001 – Nova Update Snapshot

## Status

Angenommen

## Kategorie

Update / Snapshot / Recovery

## Zweck

NovaOS definiert Update Snapshots als konsistente, versionierte Wiederherstellungspunkte für updatekritische Zustände.

Vor riskanten Änderungen kann NovaOS den relevanten Ausgangszustand sichern und bei fehlgeschlagener Installation, Aktivierung oder Verifikation kontrolliert darauf zurückgreifen.

```text
Current State
     ↓
Create Snapshot
     ↓
Apply Update
     ↓
Verify
    ↙   ↘
Success  Failure
   ↓       ↓
Commit   Restore
```

## Grundprinzipien

```text
Snapshot ≠ Backup
Snapshot ≠ Checkpoint
Snapshot ≠ A/B Slot
Snapshot ≠ Transaction
Snapshot Created ≠ Snapshot Valid
Snapshot Available ≠ Rollback Possible
Restore ≠ Verified Recovery
Old State ≠ Secure State
```

## Snapshot Model

```text
UpdateSnapshot
├── SnapshotID
├── UpdateID
├── TransactionID
├── Scope
├── StateVersion
├── CreationTime
├── Consistency
└── IntegrityState
```

Optional:

```text
BuildID
Generation
Dependencies
StorageReferences
StateSchema
SecurityState
RollbackPolicy
RetentionPolicy
ProvenanceID
```

## Snapshot Scope

Snapshots müssen einen expliziten Scope besitzen.

Beispiele:

```text
Configuration
System Component
Driver
Service
Package Set
Filesystem Subvolume
System State
Update Transaction
```

NovaOS soll den kleinsten ausreichenden Snapshot-Scope verwenden.

## Snapshot Creation

Ein Snapshot wird vor kritischen Änderungen erzeugt.

```text
Observe State
    ↓
Determine Scope
    ↓
Create Snapshot
    ↓
Verify Snapshot
    ↓
Begin Update
```

Ein Update darf bei vorgeschriebenem Snapshot erst fortgesetzt werden, wenn dessen Erstellung erfolgreich bestätigt wurde.

## Konsistenz

Snapshots können unterschiedliche Konsistenzstufen besitzen:

```text
Atomic
Transaction-Consistent
Quiesced
Version-Consistent
Crash-Consistent
Application-Consistent
Best-Effort
```

Die verwendete Konsistenzstufe muss dokumentiert werden.

```text
Snapshot Exists ≠ Snapshot Is Sufficient
```

## Snapshot-Techniken

NovaOS darf abhängig vom Storage-System unterschiedliche Verfahren verwenden:

```text
Copy-on-Write
Filesystem Snapshot
Versioned Objects
Immutable State
Block Snapshot
State Serialization
```

Die NPSPEC schreibt keine einzelne Storage-Technologie vor.

## Update Integration

Der Snapshot wird mit der Update-Transaktion verbunden.

```text
TransactionID
     ↕
SnapshotID
     ↕
Base State
```

Dadurch ist eindeutig nachvollziehbar, welcher Zustand vor einer bestimmten Update-Transaktion existierte.

## State Versioning

Snapshots müssen den erfassten State-Versionen zugeordnet werden können.

```text
State v42
   ↓
Snapshot S1
   ↓
Update
   ↓
State v43
```

Bei Wiederherstellung wird die alte Versionsnummer nicht wiederverwendet.

```text
Restore S1
   ↓
State v44
```

```text
Rollback ≠ Version Reuse
```

## Restore

Ein Snapshot darf nicht blind wiederhergestellt werden.

Vor Restore müssen mindestens geprüft werden:

```text
Snapshot Integrity
Compatibility
Dependencies
Current State
Security Constraints
State Schema
Recovery Policy
```

Danach:

```text
Validate
   ↓
Restore
   ↓
Reconcile
   ↓
Verify
```

## Security State

Monotone Sicherheitsinformationen dürfen durch Snapshot Restore nicht zurückgesetzt werden.

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
Snapshot Restore
≠
Security State Rollback
```

## State Migration

Ein Update kann das State Schema verändern.

```text
Snapshot
State Schema v1

Update
State Schema v2
```

Vor Restore muss geprüft werden, ob der aktuelle Zustand mit dem Snapshot kompatibel zurückgeführt werden kann.

Irreversible Migrationen müssen explizit berücksichtigt werden.

## Atomic Updates

Snapshots ergänzen atomare Updates.

```text
Snapshot
   +
Atomic Update
   +
Verification
```

Atomicität schützt den Übergang.

Der Snapshot stellt einen zusätzlichen bekannten Wiederherstellungspunkt bereit.

## A/B Updates

Snapshots und A/B erfüllen unterschiedliche Aufgaben.

```text
A/B
→ vollständige alternative Systemgeneration

Snapshot
→ versionierter Zustand eines definierten Scopes
```

Beide Mechanismen dürfen gemeinsam verwendet werden.

## Failure Handling

Bei Update-Fehlern kann der Update Manager entscheiden:

```text
Abort
Rollback
Snapshot Restore
A/B Fallback
Compensation
Recovery Mode
```

Die Auswahl erfolgt anhand des Update- und Recovery-Plans.

## Snapshot Verification

Snapshots müssen vor kritischer Verwendung verifiziert werden können.

Zu prüfen sind beispielsweise:

```text
Integrity
Completeness
State Version
Dependencies
Storage Availability
Schema Compatibility
```

```text
Stored ≠ Recoverable
```

## Retention

Snapshots dürfen nach Policy aufbewahrt werden.

Beispiele:

```text
Until Update Verified
Until Next Known-Good Version
Last N Updates
Defined Time Window
Manual Retention
```

Updatekritische Snapshots dürfen nicht entfernt werden, solange sie für einen aktiven Recovery-Pfad erforderlich sind.

## Resource Economy

Snapshots verbrauchen Ressourcen.

NovaOS muss berücksichtigen können:

```text
Storage
Metadata
I/O
Memory
Creation Latency
Retention Cost
```

Copy-on-Write und inkrementelle Verfahren sollen bevorzugt werden, wenn sie technisch sinnvoll sind.

## Crash Recovery

Nach Crash muss die Verbindung zwischen Update-Transaktion und Snapshot rekonstruierbar sein.

```text
Transaction Recovery
       ↓
Snapshot Reference
       ↓
Continue / Restore / Recover
```

`Unknown` darf nicht als gültiger Snapshot-Zustand interpretiert werden.

## Provenance

NovaOS soll nachvollziehen können:

```text
SnapshotID
UpdateID
TransactionID
Source State
StateVersion
CreationTime
Consistency
Integrity
Restore History
Verification Result
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Available Snapshots
Snapshot Scope
State Version
Consistency
Integrity State
Associated Update
Associated Transaction
Restore Compatibility
Retention State
Last Verification
```

## Normative Anforderungen

1. NovaOS MUSS Update Snapshots für geeignete kritische Updates unterstützen können.
2. Jeder Snapshot MUSS eine eindeutige SnapshotID besitzen.
3. Jeder Snapshot MUSS einen expliziten Scope besitzen.
4. Snapshot und zugehörige Update-Transaktion MÜSSEN verknüpfbar sein.
5. Die verwendete Konsistenzstufe MUSS bestimmbar sein.
6. Vorgeschriebene Snapshots MÜSSEN vor dem kritischen Update-Schritt erstellt werden.
7. Kritische Snapshots MÜSSEN auf Integrität prüfbar sein.
8. Snapshot Restore MUSS vor Ausführung validiert werden.
9. Restore DARF alte State-Versionen NICHT wiederverwenden.
10. Restore MUSS einen neuen nachvollziehbaren State erzeugen.
11. Snapshot Restore DARF monotone Security States NICHT zurücksetzen.
12. State-Schema-Kompatibilität MUSS vor Restore geprüft werden können.
13. Irreversible Migrationen MÜSSEN berücksichtigt werden.
14. Snapshots DÜRFEN mit atomaren Updates kombiniert werden.
15. Snapshots DÜRFEN mit A/B-Updates kombiniert werden.
16. Snapshot Restore DARF NICHT automatisch als erfolgreiche Recovery gelten.
17. Wiederhergestellter Zustand MUSS verifiziert werden.
18. Updatekritische Snapshots DÜRFEN vor Abschluss des benötigten Recovery-Zeitraums NICHT entfernt werden.
19. Snapshot-Retention MUSS durch Policy steuerbar sein.
20. Snapshot-Erstellung SOLL Resource Economy berücksichtigen.
21. Nach Crash MUSS die Snapshot-Zuordnung einer Update-Transaktion rekonstruierbar sein.
22. `Unknown` DARF NICHT als gültiger oder recoverbarer Snapshot-Zustand interpretiert werden.
23. Snapshot- und Restore-Vorgänge MÜSSEN nachvollziehbare Provenance besitzen.
24. Snapshot-Zustand MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-UPDATE-MANAGER-0001`
- `NPSPEC-UPDATE-ATOMIC-0001`
- `NPSPEC-UPDATE-TRANSACTIONAL-0001`
- `NPSPEC-UPDATE-AB-0001`
- `NPSPEC-STATE-SNAPSHOT-0001`
- `NPSPEC-STATE-VERSIONING-0001`
- `NPSPEC-STATE-ROLLBACK-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-RESILIENCE-CHECKPOINT-0001`
- `NPSPEC-RESILIENCE-ROLLBACK-0001`
- `NPSPEC-RESILIENCE-VERIFICATION-0001`
- `NPSPEC-SECURITY-CODEINTEGRITY-0001`
- `ADR-ARCH-0171`

## Ergebnis

```text
Known-Good State
       ↓
Create Snapshot
       ↓
Verify Snapshot
       ↓
Apply Update
       ↓
Verify Update
      ↙   ↘
   Healthy Failed
      ↓       ↓
   Finalize Restore Snapshot
              ↓
          Reconcile
              ↓
            Verify
```

NovaOS erhält damit einen versionierten und verifizierbaren Snapshot-Mechanismus für Updates, der einen definierten Ausgangszustand schützt und fehlgeschlagene Änderungen kontrolliert zurückführen kann, ohne dabei State-Historie oder aktuelle Sicherheitszustände unzulässig zurückzusetzen.