# NPSPEC-FILESYSTEM-TRANSACTION-0001 – Nova Filesystem Transactions

## Status

Angenommen

## Kategorie

Filesystem / Transaction / Consistency

## Zweck

NovaOS unterstützt transaktionale Filesystem-Operationen, damit zusammengehörige Änderungen entweder als konsistenter neuer Zustand sichtbar werden oder kontrolliert verworfen werden.

```text
Begin
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
```

Filesystem-Transaktionen können Dateien, Verzeichnisse, Metadaten, Relations, Projections und Namespace-Strukturen gemeinsam verändern.

## Grundprinzipien

```text
Transaction ≠ Lock
Transaction ≠ Snapshot
Transaction ≠ Backup
Prepared ≠ Committed
Committed ≠ Verified
Abort ≠ Rollback
Atomic ≠ Globally Atomic
Failure ≠ Undefined State
```

## Transaction Model

```text
FilesystemTransaction
├── TransactionID
├── Scope
├── BaseVersions
├── Changes
├── State
└── OwnerID
```

Optional:

```text
ResourceBudget
Deadline
IsolationPolicy
RollbackPolicy
SnapshotID
ProvenanceID
```

## Zustände

```text
Created
Staging
Validating
Prepared
Committing
Committed
Verifying
Completed
```

Fehlerzustände:

```text
Aborting
Aborted
RollingBack
Failed
RecoveryRequired
Unknown
```

`Unknown` darf nicht als erfolgreicher Commit interpretiert werden.

## Transaction Scope

Eine Transaktion besitzt einen expliziten Scope.

Beispiele:

```text
Single Object
Object Set
Directory
Namespace Subtree
Projection Set
Volume
Cross-Volume Operation
```

NovaOS setzt keine universelle globale Filesystem-Transaktion voraus.

## Staging

Änderungen werden zunächst als noch nicht autoritativer Zustand vorbereitet.

```text
Current State
     ↓
Staged Changes
     ↓
Candidate State
```

Während `Staging` darf der neue Zustand nicht teilweise als committed Zustand sichtbar werden.

## Unterstützte Operationen

Transaktionen sollen mindestens folgende Operationen kombinieren können:

```text
Create
Write
Rename
Move
Delete
Metadata Update
Relation Update
Projection Update
Namespace Update
```

Beispiel:

```text
Create Object
+
Write Metadata
+
Create Relation
+
Add Projection
        ↓
Single Transaction
```

## ObjectID

Die Object Identity muss innerhalb einer Transaktion konsistent bleiben.

```text
ObjectID
   ↓
Version N
   ↓
Transaction
   ↓
Version N+1
```

Rename, Move oder Projection-Änderungen erzeugen keine neue `ObjectID`.

Eine echte neue Datei erhält dagegen eine neue `ObjectID`.

## Versionierung

Vor Commit müssen relevante Basisversionen geprüft werden.

```text
Expected Version
       ↓
Current Version
       ↓
Match?
```

Bei zwischenzeitlichen Änderungen muss die Transaktion:

```text
Retry
Revalidate
Conflict
Abort
```

entsprechend ihrer Policy behandeln.

## Validierung

Vor Commit müssen mindestens relevante Bedingungen geprüft werden:

```text
Object Versions
Namespace State
Target Existence
Capabilities
Security Policy
Storage Availability
Relations
Projection Rules
Resource Budget
```

```text
Valid Earlier
≠
Valid Now
```

Kritische Bedingungen müssen unmittelbar vor Commit revalidierbar sein.

## Atomic Visibility

Der Commit muss innerhalb des definierten Transaction Scope atomar sichtbar werden.

Andere Beobachter sehen entweder:

```text
Old State
```

oder:

```text
New State
```

aber keinen logisch unzulässigen Zwischenzustand.

## Namespace Transactions

Namespace-Operationen können gemeinsam committed werden.

```text
Rename
Move
Mount
Projection Add
Projection Remove
Overlay Change
```

Beispiel:

```text
Remove Old Projection
+
Add New Projection
+
Update Relation
        ↓
Commit
```

## NovaFile Integration

Änderungen an einem NovaFile können Payload, Metadata und Relationships gemeinsam umfassen.

```text
NovaFile
├── Payload
├── Metadata
└── Relationships
       ↓
Filesystem Transaction
```

Dadurch darf beispielsweise kein Zustand sichtbar werden, bei dem der neue Payload bereits aktiv ist, aber noch veraltete verpflichtende Metadaten gelten.

## Relations

Relations können Teil derselben Transaktion wie ihre Objekte sein.

```text
Create Object B
+
Create Relation A → B
        ↓
Commit
```

Damit wird verhindert, dass eine Relation auf ein noch nicht committed Objekt zeigt.

## Projections

Projection-Änderungen können transaktional mit Objekt- und Namespace-Änderungen verbunden werden.

```text
Modify Object
+
Update Metadata
+
Update Projection
        ↓
Commit
```

## Cross-Volume Transactions

Operationen über mehrere Volumes dürfen unterstützt werden, dürfen jedoch nicht automatisch globale ACID-Semantik voraussetzen.

Falls native Atomicity nicht möglich ist, muss NovaOS explizite Mechanismen verwenden:

```text
Prepare
Copy / Stage
Validate
Commit Marker
Cleanup
```

oder kontrollierte Compensation beziehungsweise Recovery.

```text
Cross-Volume
≠
Automatically Atomic
```

## Delete

Löschen innerhalb einer Transaktion wird zunächst logisch vorgemerkt.

```text
Object
  ↓
Pending Delete
  ↓
Commit
  ↓
Deleted
```

Ein Abort vor Commit erhält den ursprünglichen Zustand.

## Abort

Vor dem Commit Point kann eine Transaktion kontrolliert abgebrochen werden.

```text
Staged Changes
     ↓
Abort
     ↓
Discard
```

Der autoritative Zustand bleibt unverändert.

## Rollback

Nach einem bereits erfolgten Commit ist ein Rollback eine neue Zustandsänderung.

```text
Version N
   ↓
Commit
   ↓
Version N+1
   ↓
Rollback
   ↓
Version N+2
```

Historie wird nicht zurückgeschrieben.

## Crash Consistency

Nach Crash oder Stromverlust muss der Zustand rekonstruierbar sein.

```text
Persistent Transaction State
          ↓
Recovery
          ↓
Committed?
Prepared?
Incomplete?
Unknown?
```

NovaOS muss entscheiden können zwischen:

```text
Complete
Abort
Rollback
Recover
```

Ein undefinierter Mischzustand darf nicht stillschweigend als erfolgreich gelten.

## Capability Integration

Alle Operationen innerhalb einer Transaktion bleiben capability-gebunden.

```text
Transaction
   ↓
Requested Operations
   ↓
Capability Validation
   ↓
Commit
```

Eine Transaktion verleiht keine zusätzliche Authority.

Capabilities müssen bei sicherheitsrelevanten Änderungen vor Commit revalidierbar sein.

```text
Authorized at Begin
≠
Automatically Authorized at Commit
```

## Concurrency

Parallele Transaktionen müssen Konflikte erkennen können.

```text
Transaction A
      ↓
   Object X
      ↑
Transaction B
```

Konfliktbehandlung kann umfassen:

```text
Serialize
Retry
Reject
Rebase
Abort
```

Die gewählte Policy muss deterministisch nachvollziehbar sein.

## Resource Economy

Filesystem-Transaktionen dürfen Ressourcen reservieren oder budgetieren.

Beispiele:

```text
Storage Space
Memory
I/O
Metadata Space
Journal Space
Temporary Objects
```

Vor kritischen Commit-Schritten soll geprüft werden, ob notwendige Ressourcen verfügbar sind.

## Verification

Nach Commit können kritische Transaktionen verifiziert werden.

```text
Committed State
      ↓
Read / Validate
      ↓
Expected State?
```

`Committed` und `Verified` bleiben getrennte Zustände.

## Provenance

NovaOS soll für relevante Transaktionen nachvollziehen können:

```text
TransactionID
OwnerID
Affected ObjectIDs
Base Versions
Result Versions
Operations
Commit State
Failure Reason
Timestamp
ProvenanceID
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
TransactionID
State
Scope
Owner
Affected Objects
Staged Changes
Conflicts
Resource Usage
Commit State
Verification State
```

Introspection darf keine zusätzliche Authority auf beteiligte Objekte übertragen.

## Normative Anforderungen

1. NovaOS MUSS Filesystem-Transaktionen unterstützen.
2. Jede Transaktion MUSS eine eindeutige `TransactionID` besitzen.
3. Jede Transaktion MUSS einen expliziten Scope besitzen.
4. Staged Changes DÜRFEN vor Commit NICHT als autoritativer Zustand gelten.
5. Zusammengehörige Änderungen SOLLEN innerhalb ihres Transaction Scope atomar sichtbar werden.
6. Object-, Metadata-, Relation-, Projection- und Namespace-Änderungen MÜSSEN gemeinsam transaktional ausführbar sein können.
7. Rename und Move DÜRFEN die `ObjectID` nicht verändern.
8. Relevante Basisversionen MÜSSEN vor Commit validierbar sein.
9. Konflikte zwischen parallelen Transaktionen MÜSSEN erkennbar sein.
10. Sicherheitsrelevante Bedingungen MÜSSEN vor Commit revalidierbar sein.
11. Eine Transaktion DARF keine zusätzliche Capability verleihen.
12. Abort vor Commit MUSS staged Änderungen kontrolliert verwerfen können.
13. Rollback nach Commit MUSS als neue Zustandsänderung behandelt werden.
14. Rollback DARF Historie NICHT überschreiben.
15. Crash Recovery MUSS den persistenten Transaction State auswerten können.
16. `Unknown` DARF NICHT als erfolgreicher Commit interpretiert werden.
17. Cross-Volume Transactions DÜRFEN NICHT automatisch globale Atomicity voraussetzen.
18. Nicht atomar ausführbare Operationen MÜSSEN Recovery- oder Compensation-Semantik besitzen.
19. NovaFile Payload, Metadata und Relationships MÜSSEN gemeinsam transaktional änderbar sein können.
20. Relations DÜRFEN NICHT committed auf nicht committed Targets verweisen.
21. Projection-Änderungen MÜSSEN transaktional mit zugrunde liegenden Objektänderungen kombinierbar sein.
22. Kritische Ressourcen SOLLEN vor Commit reserviert oder validiert werden.
23. `Committed` DARF NICHT automatisch als `Verified` interpretiert werden.
24. Kritische Transaktionen MÜSSEN nach Commit verifizierbar sein.
25. Transaktionen MÜSSEN nachvollziehbare Provenance besitzen können.
26. Transaction State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-RELATION-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-NOVAFILE-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-TRANSACTION-SYSTEM-0001`
- `NPSPEC-TRANSACTION-ROLLBACK-0001`

## Ergebnis

```text
Filesystem Operations
        ↓
Begin Transaction
        ↓
Stage Changes
        ↓
Validate
        ↓
Prepare
        ↓
Revalidate
        ↓
Commit
     ↙      ↘
 Success    Failure
    ↓          ↓
 Verify    Abort / Recovery
    ↓
Completed
```

NovaOS erhält damit eine transaktionale Filesystem-Schicht, in der zusammengehörige Änderungen an Objekten, Metadaten, Relations, Projections und Namespace-Strukturen konsistent durchgeführt werden können. Unvollständige Operationen werden nicht als gültiger Zustand sichtbar, während Konflikte, Abstürze und fehlgeschlagene Änderungen kontrolliert behandelt werden.