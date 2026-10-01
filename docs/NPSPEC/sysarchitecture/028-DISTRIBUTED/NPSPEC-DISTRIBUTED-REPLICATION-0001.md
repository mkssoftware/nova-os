# NPSPEC-DISTRIBUTED-REPLICATION-0001 – Nova Distributed Replication

## Status

Angenommen

## Kategorie

Distributed / Replication / Availability & Resilience

## Zweck

NovaOS definiert ein einheitliches Modell zur kontrollierten Replikation von Daten, Zuständen und geeigneten Diensten über mehrere Nodes und Failure Domains hinweg.

```text
Logical Object / State
        ↓
Replication Policy
        ↓
Replica Placement
        ↓
Synchronization
        ↓
Replica Set
```

Replikation erhöht Verfügbarkeit und Ausfallsicherheit, erzeugt jedoch keine neue logische Identität.

## Grundprinzipien

```text
Replica ≠ New Object
Replication ≠ Backup
Replication ≠ Cache
Replication ≠ Migration
Replication ≠ Global Transaction
Replication ≠ Automatic Consistency
Replica Location ≠ Authority
More Replicas ≠ Automatically More Resilience
```

## Replication Model

Eine Replikationsgruppe wird beschrieben durch:

```text
ReplicationGroup
├── ReplicationID
├── ObjectID / StateID
├── Replica Set
├── Replication Policy
└── State
```

Optional:

```text
VersionID
Primary Replica
Consistency Policy
Replication Factor
Placement Constraints
Failure Domains
Trust Requirements
Sovereignty Constraints
Durability Requirements
Recovery Policy
```

## Replica Identity

Alle Replicas gehören zum selben logischen Objekt.

```text
ObjectID
├── Replica A
├── Replica B
└── Replica C
```

Die physische Replica benötigt dennoch eine eigene technische Identifikation.

```text
ObjectID ≠ ReplicaID
```

## Replication Targets

Replikation kann für geeignete Systemobjekte verwendet werden:

```text
Storage Objects
Metadata
Configuration State
Service State
Indexes
Cluster State
Selected Runtime State
```

Nicht jeder Zustand ist automatisch replizierbar.

## Replication Policy

Policies können definieren:

```text
Replication Factor
Consistency Model
Placement
Failure Domains
Synchronization Mode
Durability
Recovery
```

Beispiel:

```text
ReplicationFactor = 3
FailureDomains = DistinctNodes
Consistency = Strong
```

## Replica Placement

Replicas werden über das Distributed Placement Model verteilt.

```text
Replication Policy
       ↓
Placement Constraints
       ↓
Eligible Nodes
       ↓
Replica Placement
```

Dabei können berücksichtigt werden:

```text
Node
Device
Power Domain
Network Segment
Site
Provider
Region
```

Mehrere Replicas innerhalb derselben Failure Domain dürfen nicht automatisch als unabhängige Redundanz betrachtet werden.

## Synchronization

NovaOS muss unterschiedliche Synchronisationsmodelle unterstützen können.

```text
Synchronous
Asynchronous
PolicyDefined
```

Die gewählte Semantik muss explizit bleiben.

## Consistency

Replication und Consistency sind getrennte Konzepte.

```text
Replicated ≠ Consistent
```

Mögliche Modelle:

```text
Strong
Snapshot
Eventual
ApplicationDefined
```

Clients müssen erkennen können, welchen Konsistenzzustand sie erhalten.

## Versioning

Replicas müssen Versionen eindeutig unterscheiden können.

```text
ObjectID
├── Replica A → Version 7
├── Replica B → Version 7
└── Replica C → Version 6
```

Veraltete Replicas müssen als solche erkennbar sein.

## Conflict Detection

Bei konkurrierenden Änderungen können divergierende Versionen entstehen.

```text
Version A
   ↘
    Conflict
   ↗
Version B
```

Mögliche Behandlung:

```text
Reject
Merge
Keep Both
Resolve by Policy
Application Resolution
Transactional Resolution
```

Konflikte dürfen nicht durch stilles Überschreiben verborgen werden.

## Primary und Multi-Writer

Eine Policy kann beispielsweise festlegen:

```text
Single Writer
Primary Replica
Multi Writer
Immutable Replicas
```

Multi-Writer darf nur verwendet werden, wenn Konflikt- und Konsistenzsemantik definiert sind.

## Quorum

Geeignete Replication Policies können Quorum-Verfahren verwenden.

```text
N = Replicas
R = Read Participants
W = Write Participants
```

Die konkrete Quorum-Semantik hängt vom verwendeten Konsistenzmodell ab.

Quorum bedeutet nicht automatisch globale Strong Consistency.

## Integrity

Jede Replica muss auf Integrität überprüfbar sein.

```text
Replica
   ↓
ContentID / Checksum
   ↓
Verification
```

Beschädigte Replicas dürfen nicht als gültige Quelle verwendet werden.

## Repair

Fehlende oder beschädigte Replicas können rekonstruiert werden.

```text
Healthy Replica
      ↓
Verified Copy
      ↓
Replacement Replica
      ↓
Integrity Verification
```

Repair darf nur aus einer ausreichend vertrauenswürdigen und gültigen Quelle erfolgen.

## Trust

Replica Placement muss Trust Requirements berücksichtigen.

```text
Candidate Node
      ↓
Trust Evaluation
      ↓
Eligible / Rejected
```

```text
Unknown Trust ≠ Trusted
```

## Sovereignty

Alle Replicas müssen relevante Sovereignty Constraints einhalten.

```text
Object Policy
      ↓
Allowed Domains
      ↓
Replica Placement
```

Dies gilt auch für:

```text
Temporary Replicas
Repair Copies
Synchronization Data
Recovery Replicas
```

## Security und Capabilities

Replikation erzeugt keine neue Authority.

```text
ReplicaID ≠ Capability
```

Operationen wie:

```text
Create Replica
Read Replica
Write Replica
Synchronize
Repair
Remove Replica
```

benötigen explizite Capabilities.

## Encryption

Replicas können verschlüsselt gespeichert und übertragen werden.

Schlüsselzugriff muss unabhängig von Replica Placement kontrolliert bleiben.

```text
Replica Access ≠ Key Access
```

## Failure Handling

Replikation muss folgende Fehler berücksichtigen:

```text
Node Failure
Storage Failure
Network Partition
Corrupt Replica
Stale Replica
Trust Revocation
Sovereignty Change
```

Mögliche Reaktionen:

```text
Use Healthy Replica
Repair
Re-Replicate
Re-Place
Degrade
Reject
```

## Network Partition

Bei einer Partition:

```text
Replica Set
├── Partition A
└── Partition B
```

darf NovaOS keine Konsistenzgarantie behaupten, die unter den aktuellen Bedingungen nicht eingehalten werden kann.

```text
Unreachable Replica ≠ Failed Replica
```

## Re-Replication

Sinkt die Zahl gültiger Replicas unter die Policy:

```text
Required = 3
Available = 2
      ↓
Re-Replication
```

NovaOS kann automatisch einen neuen zulässigen Placement-Kandidaten bestimmen.

## Replication und Backup

Replikation schützt primär gegen Infrastruktur- und Verfügbarkeitsfehler.

Ein logisch gelöschtes oder beschädigtes Objekt kann auf alle Replicas übertragen werden.

Daher gilt:

```text
Replication ≠ Backup
```

Versioning, Snapshots und Backup bleiben eigenständige Schutzmechanismen.

## Distributed Execution

Distributed Execution kann bevorzugt Nodes verwenden, auf denen bereits geeignete Replicas vorhanden sind.

```text
Task
 +
Replica Locality
      ↓
Placement Optimization
```

Hard Constraints besitzen weiterhin Vorrang.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ReplicationID
ObjectID
ReplicaIDs
Replica Locations
Version State
Consistency State
Integrity State
Replication Factor
Failure Domains
Synchronization State
Trust State
Sovereignty State
Repair State
```

## Normative Anforderungen

1. NovaOS MUSS logische Objektidentität von Replica-Identität trennen.
2. Replicas DÜRFEN keine neuen logischen ObjectIDs erzeugen.
3. Replication MUSS von Backup, Caching und Migration getrennt bleiben.
4. Replication Factor und Consistency Policy MÜSSEN explizit definierbar sein.
5. Replica Placement MUSS Failure Domains berücksichtigen können.
6. Replication DARF nicht automatisch Strong Consistency voraussetzen.
7. Divergierende Versionen MÜSSEN erkennbar sein.
8. Konflikte DÜRFEN NICHT durch stilles Überschreiben verborgen werden.
9. Replica-Integrität MUSS überprüfbar sein.
10. Repair MUSS Quelle und Ziel validieren.
11. Trust-, Security- und Sovereignty-Constraints MÜSSEN für alle Replicas gelten.
12. Replikation DARF keine implizite Authority erzeugen.
13. Netzwerkpartitionen DÜRFEN NICHT automatisch als Replica Failure interpretiert werden.
14. NovaOS SOLL fehlende Replicas entsprechend der Policy automatisch wiederherstellen können.
15. Distributed Execution SOLL Replica Locality für Placement berücksichtigen können.
16. Replica-, Consistency-, Integrity- und Repair-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-STORAGE-0001`
- `NPSPEC-DISTRIBUTED-SCHEDULER-0001`
- `NPSPEC-DISTRIBUTED-IPC-0001`
- `NPSPEC-DISTRIBUTED-CLUSTER-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-SNAPSHOT-0001`
- `NPSPEC-STORAGE-CHECKSUM-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0066`

## Ergebnis

```text
Logical Object
      ↓
Replication Policy
      ↓
Placement + Failure Domains
      ↓
Replica Set
      ↓
Synchronization + Consistency
      ↓
Integrity Verification
      ↓
Failure Detection + Repair
```

NovaOS erhält damit ein kontrolliertes Distributed-Replication-Modell, das Daten und Zustände über mehrere Nodes und Failure Domains verteilen kann, ohne Replikation mit Backup, Konsistenz, Identität oder Authority gleichzusetzen.