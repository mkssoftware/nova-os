# NPSPEC-DISTRIBUTED-CONSISTENCY-0001 – Nova Distributed Consistency

## Status

Angenommen

## Kategorie

Distributed / Consistency / State Coordination

## Zweck

NovaOS definiert ein einheitliches Modell für Konsistenz von replizierten und verteilten Zuständen.

Die Konsistenzanforderung legt fest, welche Sicht auf einen Zustand Teilnehmer eines verteilten Systems erhalten müssen und welche Abweichungen zwischen Replikaten zulässig sind.

```text
Distributed State
      ↓
Consistency Policy
      ↓
Replication + Coordination
      ↓
Observable State
```

## Grundprinzipien

```text
Consistency ≠ Replication
Consistency ≠ Availability
Consistency ≠ Durability
Consistency ≠ Transaction
Consistency ≠ Synchronization
Consistency ≠ Integrity

Replicated ≠ Consistent
Available ≠ Current
Reachable ≠ Up-to-Date
Latest Known ≠ Globally Latest
```

## Consistency Model

Ein verteilter Zustand wird beschrieben durch:

```text
DistributedConsistency
├── ObjectID / StateID
├── Consistency Policy
├── Version State
└── Consistency State
```

Optional:

```text
ReplicationID
Epoch
Revision
Read Policy
Write Policy
Conflict Policy
Quorum Policy
Staleness Bound
Synchronization State
```

## Consistency Classes

NovaOS unterstützt mindestens:

```text
Strong
Snapshot
Eventual
ApplicationDefined
```

Weitere spezialisierte Modelle können ergänzt werden, müssen jedoch explizit definiert und versioniert sein.

## Strong Consistency

Bei:

```text
Consistency = Strong
```

muss das System die für den jeweiligen Dienst definierte starke Konsistenzsemantik gewährleisten.

Kann diese aufgrund von:

```text
Network Partition
Insufficient Quorum
Node Failure
Coordination Failure
```

nicht eingehalten werden, darf NovaOS nicht stillschweigend auf schwächere Konsistenz wechseln.

```text
Strong unavailable
      ↓
Block / Defer / Reject / Fail
```

## Snapshot Consistency

Snapshot-Konsistenz liefert eine zusammengehörige Sicht eines definierten Versionszustands.

```text
SnapshotID
├── Object A → Version 5
├── Object B → Version 8
└── Object C → Version 3
```

Neuere Änderungen müssen nicht sichtbar sein, solange die Snapshot-Sicht konsistent bleibt.

## Eventual Consistency

Bei Eventual Consistency dürfen Replikate zeitweise unterschiedliche Zustände besitzen.

```text
Replica A → Version 8
Replica B → Version 7
Replica C → Version 8
```

Ohne weitere Änderungen sollen sie entsprechend der definierten Policy konvergieren.

Eventual Consistency darf nicht als aktuelle globale Sicht dargestellt werden.

## Consistency State

NovaOS muss Zustände unterscheiden können:

```text
Current
Snapshot
Converging
PossiblyStale
Stale
Conflicted
Unavailable
Unknown
```

Dabei gilt:

```text
Unknown ≠ Current
```

## Versioning

Consistency arbeitet mit expliziter Versionierung.

```text
ObjectID
├── VersionID
├── Epoch
└── Revision
```

Versionen müssen ausreichend sein, um relevante Zustandsunterschiede und Konflikte erkennen zu können.

## Reads

Read Policies können beispielsweise verlangen:

```text
Current Read
Snapshot Read
Replica Read
Stale-Tolerant Read
```

Der Aufrufer muss über die tatsächlich gewährleistete Konsistenz informiert werden können.

## Writes

Write Policies können definieren:

```text
Single Writer
Primary Writer
Quorum Write
Multi Writer
Transactional Write
```

Die gewählte Write Policy muss mit dem Consistency Model kompatibel sein.

## Conflicts

Bei konkurrierenden Änderungen:

```text
Version A
   ↘
   Conflict
   ↗
Version B
```

muss NovaOS den Konflikt erkennen können.

Mögliche Strategien:

```text
Reject
Keep Both
Merge
Policy Resolution
Application Resolution
Transactional Resolution
```

Automatische Zusammenführung darf nur erfolgen, wenn die Semantik des Zustands dies erlaubt.

## Quorum

Geeignete Consistency Policies können Quorum-Mechanismen verwenden.

```text
N = Replica Count
R = Read Quorum
W = Write Quorum
```

Quorum ist ein Mechanismus und keine eigenständige Garantie für eine bestimmte Konsistenzklasse.

```text
Quorum ≠ Automatically Strong Consistency
```

## Network Partition

Bei einer Netzwerkpartition:

```text
Replica Set
├── Partition A
└── Partition B
```

muss NovaOS entsprechend der definierten Policy entscheiden, welche Operationen weiterhin zulässig sind.

Das System darf keine Garantie behaupten, die unter den aktuellen Bedingungen nicht eingehalten werden kann.

```text
Unreachable ≠ Failed
```

## Availability Trade-off

Ein Consistency Model kann Auswirkungen auf Availability besitzen.

Bei bestimmten Fehlerzuständen kann eine starke Konsistenzanforderung bedeuten:

```text
Consistency preserved
      ↓
Operation unavailable
```

NovaOS darf diese Entscheidung nicht implizit durch einen Wechsel auf schwächere Konsistenz umgehen.

## Replication

Replication stellt physische oder logische Kopien bereit.

Consistency bestimmt, welche Zustandsbeziehungen zwischen diesen Replicas gelten.

```text
Replication
    +
Consistency Policy
    ↓
Defined Replica Semantics
```

## Distributed Storage

Distributed Storage muss für jedes relevante Objekt erkennen können:

```text
Requested Consistency
Actual Consistency
Replica Version
Conflict State
```

Caches dürfen ihre möglicherweise veraltete Sicht nicht als aktuelle starke Sicht darstellen.

## Transactions

Transactions und Consistency ergänzen sich, bleiben aber getrennte Konzepte.

```text
Transaction ≠ Distributed Consistency
```

Eine lokal atomare Transaktion garantiert nicht automatisch eine global konsistente Sicht aller Replicas.

## Distributed Execution

Execution Contracts können erforderliche Konsistenz definieren.

```text
ExecutionContract
      ↓
Required Data Consistency
      ↓
Eligible Data Sources
```

Ein Task darf keine stale Replica verwenden, wenn eine aktuelle Sicht erforderlich ist.

## Recovery

Nach Ausfällen oder Partitionen können Zustände divergieren.

```text
Reconnect
   ↓
Version Comparison
   ↓
Conflict Detection
   ↓
Synchronization / Resolution
   ↓
Convergence
```

Konflikte dürfen während Recovery nicht stillschweigend verloren gehen.

## Trust und Sovereignty

Consistency Coordination darf Trust- und Sovereignty-Grenzen nicht umgehen.

Ein Node darf nicht nur deshalb für Quorum oder Synchronisation verwendet werden, weil er technisch erreichbar ist.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Consistency Policy
Actual Consistency State
VersionID
Epoch
Replica Versions
Read Policy
Write Policy
Quorum State
Conflict State
Synchronization State
Staleness
```

## Normative Anforderungen

1. NovaOS MUSS Consistency unabhängig von Replication, Availability und Transactions modellieren.
2. Die verwendete Consistency Policy MUSS explizit definiert sein.
3. Strong Consistency DARF NICHT stillschweigend auf ein schwächeres Modell reduziert werden.
4. `Unknown` DARF NICHT als `Current` behandelt werden.
5. Replica-Versionen MÜSSEN ausreichend unterscheidbar sein.
6. Read- und Write-Policies MÜSSEN mit der gewählten Consistency Policy vereinbar sein.
7. Konflikte MÜSSEN explizit erkannt werden können.
8. Konflikte DÜRFEN NICHT durch stilles Überschreiben verborgen werden.
9. Automatische Konfliktauflösung DARF nur bei definierter Semantik erfolgen.
10. Quorum DARF NICHT automatisch mit Strong Consistency gleichgesetzt werden.
11. Netzwerkpartitionen DÜRFEN NICHT automatisch als Node Failure interpretiert werden.
12. NovaOS DARF keine Konsistenzgarantie melden, die aktuell nicht eingehalten werden kann.
13. Execution Contracts MÜSSEN erforderliche Datenkonsistenz deklarieren können.
14. Recovery MUSS Versionen und Konflikte berücksichtigen.
15. Trust- und Sovereignty-Constraints DÜRFEN durch Consistency Coordination NICHT umgangen werden.
16. Consistency-, Version-, Quorum- und Conflict-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-STORAGE-0001`
- `NPSPEC-DISTRIBUTED-CLUSTER-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-DISTRIBUTED-REPLICATION-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `ADR-ARCH-0067`

## Ergebnis

```text
Distributed State
      ↓
Consistency Requirement
      ↓
Read + Write Policy
      ↓
Replication Coordination
      ↓
Version + Conflict Tracking
      ↓
Observable Consistency State
      ↓
Recovery + Convergence
```

NovaOS erhält damit ein explizites Distributed-Consistency-Modell, das starke, Snapshot-, eventual und anwendungsspezifische Konsistenz sauber voneinander trennt und verhindert, dass veraltete, unbekannte oder konfliktbehaftete Zustände fälschlich als aktuelle konsistente Daten behandelt werden.