# NPSPEC-DISTRIBUTED-STORAGE-0001 – Nova Distributed Storage

## Status

Angenommen

## Kategorie

Distributed / Storage / Data Placement

## Zweck

NovaOS definiert ein einheitliches Modell für die Speicherung und Bereitstellung von Objekten über mehrere lokale oder entfernte Speichersysteme hinweg.

Für Anwendungen und Systemkomponenten bleibt die logische Objektidentität unabhängig davon, auf welchem Gerät, Volume oder Knoten die Daten physisch gespeichert sind.

```text
ObjectID
   ↓
Distributed Storage Resolution
   ↓
Placement / Replica / Cache
   ↓
Physical Storage
```

## Grundprinzipien

```text
Object Identity ≠ Storage Location
ObjectID ≠ Path
ObjectID ≠ Node
Replica ≠ New Object
Cache ≠ Authoritative Copy
Replication ≠ Backup
Encryption ≠ Authorization
Availability ≠ Consistency
Location Transparency ≠ Sovereignty Bypass
```

## Distributed Storage Object

Ein verteilt gespeichertes Objekt wird logisch über seine bestehende NovaOS-Objektidentität adressiert.

```text
DistributedStorageObject
├── ObjectID
├── VersionID
├── SemanticTypeID
└── Placement State
```

Optional:

```text
ContentID
Primary Location
Replicas
Caches
Consistency Policy
Durability Policy
Availability Policy
Sovereignty Constraints
Trust Requirements
Encryption State
Integrity State
```

## Storage Nodes

Ein Storage Node kann sein:

```text
Local Volume
Other NovaOS Device
NAS
Server
Private Infrastructure
Remote Storage Provider
Distributed Storage Cluster
```

Jeder Node besitzt eigene:

```text
NodeID
Resources
Capabilities
Location
Trust State
Sovereignty Domain
Health
Capacity
Performance
```

## Location Transparency

Objekte werden über stabile Identitäten referenziert.

```text
ObjectID
   ↓
Resolution
   ↓
Current Storage Location
```

Eine Migration verändert nicht automatisch die Objektidentität.

```text
Node A → Node B

ObjectID bleibt identisch
```

## Placement

NovaOS kann Daten anhand von Constraints platzieren.

```text
Object
   ↓
Placement Policy
   ↓
Eligible Storage Nodes
   ↓
Selected Placement
```

Berücksichtigt werden können:

```text
Capacity
Latency
Availability
Durability
Trust
Sovereignty
Security
Energy
Network Cost
Locality
```

Hard Constraints besitzen Vorrang vor Optimierung.

## Replication

Ein Objekt kann mehrere physische Repräsentationen besitzen.

```text
ObjectID
├── Replica A
├── Replica B
└── Replica C
```

Replicas bleiben demselben logischen Objekt zugeordnet.

Replica-Zustände müssen Version und Integrität eindeutig erkennen lassen.

## Consistency

NovaOS muss unterschiedliche Konsistenzanforderungen unterstützen können.

Beispiele:

```text
Strong
Snapshot
Eventual
ApplicationDefined
```

Die verwendete Konsistenzsemantik muss explizit sein.

```text
Unknown Consistency ≠ Current Data
```

## Versioning

Verteilte Speicherung integriert das Nova Object Versioning.

```text
ObjectID
├── Version 1
├── Version 2
└── Version 3
```

Konflikte dürfen nicht durch stilles Überschreiben verborgen werden.

## Conflict Handling

Bei konkurrierenden Änderungen können entstehen:

```text
Version A
Version B
```

NovaOS muss Konflikte explizit erkennen.

Mögliche Strategien:

```text
Reject
Merge
Keep Both
Application Resolution
Transactional Resolution
```

Automatische Zusammenführung ist nur zulässig, wenn die Semantik dies unterstützt.

## Caching

Daten dürfen näher an der Ausführung zwischengespeichert werden.

```text
Authoritative Storage
       ↓
Cache
       ↓
Consumer
```

Caches müssen ihren Zustand kennen:

```text
Current
PossiblyStale
Stale
Invalid
Unknown
```

Ein Cache erzeugt keine neue Objektidentität.

## Data Locality

Execution Planning und Distributed Storage können gemeinsam optimieren.

```text
Move Compute to Data

oder

Move Data to Compute
```

Die Entscheidung kann berücksichtigen:

```text
Data Size
Network Cost
Latency
Energy
Sovereignty
Trust
Resource Availability
```

## Sovereignty

Placement und Replication müssen Sovereignty Constraints einhalten.

```text
Object Policy
     ↓
Allowed Domains
     ↓
Eligible Storage Nodes
```

Dies gilt ebenfalls für:

```text
Replicas
Caches
Temporary Copies
Migration
Recovery Copies
```

Temporäre Speicherung darf Sovereignty nicht umgehen.

## Trust

Storage Nodes können Trust Requirements unterliegen.

```text
Node Identity
Integrity
Attestation
Provider Trust
Software Trust
Revocation State
```

```text
Unknown Trust ≠ Trusted
```

## Capability Security

Objektidentität oder Kenntnis eines Speicherortes gewährt keinen Zugriff.

```text
ObjectID ≠ Capability
NodeID ≠ Capability
Location ≠ Capability
```

Lesen, Schreiben, Replizieren, Migrieren und Löschen benötigen entsprechende Authority.

## Encryption

Daten können abhängig von ihrer Policy geschützt werden:

```text
At Rest Encryption
Transport Encryption
Metadata Protection
Key Domain Separation
```

Storage Provider sollen keine unnötigen Schlüssel oder Capabilities erhalten.

## Integrity

Replikate und übertragene Daten müssen auf Integrität überprüfbar sein.

```text
Payload
   ↓
ContentID / Checksum
   ↓
Verification
```

Beschädigte Replicas dürfen nicht stillschweigend als gültig verwendet werden.

## Migration

Objekte können zwischen Storage Nodes verschoben werden.

```text
Source
  ↓
Copy
  ↓
Verify
  ↓
Switch Placement
  ↓
Retire Old Copy
```

Die logische ObjectID bleibt erhalten.

Migration muss Security-, Trust- und Sovereignty-Anforderungen erneut prüfen.

## Failure Handling

Das System muss Fehler einzelner Nodes tolerieren können.

```text
Node Failure
Network Failure
Corrupt Replica
Unavailable Provider
Capacity Exhaustion
Trust Revocation
```

Mögliche Reaktionen:

```text
Use Replica
Reconstruct
Replan
Restore
Degrade
Fail
```

## Transactions

Verteilte Speicherung bedeutet nicht automatisch globale ACID-Semantik.

```text
Distributed Storage ≠ Global Transaction
```

Benötigte atomare Änderungen müssen explizit über das NovaOS Transaction Model koordiniert werden.

## Distributed Execution

Distributed Execution kann Storage Placement berücksichtigen.

```text
Execution Task
      +
Object Placement
      ↓
Execution Placement
```

Dadurch kann NovaOS unnötige Datenbewegungen vermeiden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
VersionID
Storage Nodes
Replica State
Cache State
Consistency Policy
Placement Policy
Integrity State
Trust State
Sovereignty State
Migration State
Availability
```

## Normative Anforderungen

1. NovaOS MUSS logische Objektidentität von physischer Storage Location trennen.
2. Migration DARF die ObjectID NICHT verändern.
3. Replicas DÜRFEN keine neuen logischen Objektidentitäten erzeugen.
4. Konsistenzsemantik MUSS explizit definierbar sein.
5. Konflikte DÜRFEN NICHT durch stilles Überschreiben verborgen werden.
6. Caches MÜSSEN ihren Aktualitätszustand darstellen können.
7. Placement MUSS Security-, Trust- und Sovereignty-Constraints berücksichtigen.
8. Replicas, Caches und temporäre Kopien MÜSSEN dieselben relevanten Sovereignty Constraints einhalten.
9. ObjectID und Storage Location DÜRFEN keine Authority erzeugen.
10. Datenintegrität MUSS über Storage- und Netzwerkgrenzen hinweg überprüfbar sein.
11. Migration MUSS Ziel-Node und Daten nach der Übertragung validieren.
12. Distributed Storage DARF NICHT automatisch globale Transaktionssemantik voraussetzen.
13. Distributed Execution SOLL Data Locality für die Ausführungsplanung berücksichtigen können.
14. Placement-, Replica-, Version- und Consistency-State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-OBJECT-MODEL-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-VERSIONING-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-CONTENTADDRESS-0001`
- `NPSPEC-STORAGE-TRANSACTION-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-CHECKSUM-0001`
- `NPSPEC-STORAGE-ENCRYPTION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-ARCH-TRANSACTIONS-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `ADR-ARCH-0060`

## Ergebnis

```text
ObjectID
   ↓
Placement Policy
   ↓
Storage Resolution
   ↓
Local / Remote Storage Nodes
   ↓
Replication + Caching + Versioning
   ↓
Integrity + Consistency
   ↓
Location-Transparent Object Access
```

NovaOS erhält damit ein verteiltes Storage-Modell, bei dem Objekte unabhängig von ihrem physischen Speicherort adressiert, repliziert, gecacht und migriert werden können, während Identität, Versionierung, Security, Trust, Sovereignty und Konsistenz explizit erhalten bleiben.