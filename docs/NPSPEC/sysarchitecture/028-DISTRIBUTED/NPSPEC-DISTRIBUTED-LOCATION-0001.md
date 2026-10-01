# NPSPEC-DISTRIBUTED-LOCATION-0001 – Nova Distributed Location

## Status

Angenommen

## Kategorie

Distributed / Location / Location Transparency

## Zweck

NovaOS definiert ein einheitliches Modell zur Beschreibung, Auflösung und Änderung physischer sowie logischer Standorte verteilter Objekte, Ressourcen, Provider und Ausführungen.

Identität und Standort bleiben grundsätzlich getrennt.

```text
Stable Identity
      ↓
Location Resolution
      ↓
Current Location
      ↓
Access / Execution
```

## Grundprinzipien

```text
Identity ≠ Location
Location ≠ Address
Location ≠ Authority
Location ≠ Trust
Location ≠ Sovereignty
Location ≠ Placement
Location ≠ Ownership

Location Transparency ≠ Authority Transparency
Location Transparency ≠ Policy Transparency
```

## Location Model

Ein Standort wird logisch beschrieben durch:

```text
DistributedLocation
├── LocationID
├── LocationType
├── Domain
└── State
```

Optional:

```text
NodeID
ClusterID
DeviceID
Network Endpoint
Storage Location
Physical Region
Administrative Domain
Sovereignty Domain
Trust Domain
Failure Domain
Topology Information
```

## Location Types

NovaOS kann unterschiedliche Standortebenen modellieren:

```text
Memory
Device
NUMA Domain
Machine
Node
Cluster
Network
Site
Region
Provider
Administrative Domain
```

Diese Ebenen können hierarchisch oder über Beziehungen miteinander verbunden werden.

## Identity Separation

Objekte und Ressourcen behalten ihre stabile Identität unabhängig vom Standort.

```text
ObjectID
   ↓
Location A
   ↓ migration
Location B
```

Dabei bleibt:

```text
ObjectID = unverändert
```

Dasselbe gilt für:

```text
ResourceID
ProviderID
ServiceID
WorkloadID
```

sofern die jeweilige Identität logisch erhalten bleibt.

## Location Resolution

NovaOS löst stabile Identitäten auf aktuelle Standorte auf.

```text
ObjectID
      ↓
Location Resolver
      ↓
LocationID
      ↓
Endpoint / Node / Storage
```

Location Resolution darf keine Authority erzeugen.

## Multiple Locations

Ein logisches Objekt kann gleichzeitig mehrere Standorte besitzen.

Beispiel:

```text
ObjectID
├── Replica → Node A
├── Replica → Node B
└── Cache   → Node C
```

Der Resolver muss zwischen:

```text
Authoritative Location
Replica Location
Cache Location
Temporary Location
```

unterscheiden können.

## Location Transparency

Aufrufer sollen Operationen möglichst über stabile Identitäten ausführen.

```text
Operation(ObjectID)
```

statt:

```text
Operation(Node42, Disk3, Path7)
```

Das System übernimmt, soweit erlaubt:

```text
Resolution
Placement
Routing
Migration
Provider Selection
```

Hard Constraints bleiben sichtbar und verbindlich.

## Location Constraints

Execution Contracts und Policies können Standorte einschränken.

Beispiele:

```text
LocalOnly
DeviceOnly
ClusterOnly
AllowedNodes
AllowedRegions
ForbiddenRegions
AllowedProviders
RequiredFailureDomain
```

Diese Constraints sind Bestandteil der Ausführungsplanung.

## Location und Placement

Placement entscheidet, **wo** etwas platziert werden soll.

Location beschreibt, **wo** es sich befindet.

```text
Placement Decision
      ↓
Migration / Creation
      ↓
Location State
```

```text
Placement ≠ Location
```

## Location und Migration

Migration verändert den Standort eines bestehenden logischen Objekts oder Workloads.

```text
Location A
      ↓
Prepare
      ↓
Transfer
      ↓
Verify
      ↓
Switch
      ↓
Location B
```

Die stabile Identität bleibt erhalten.

## Location und Replication

Replication kann mehrere gültige Locations erzeugen.

```text
ObjectID
├── Location A
├── Location B
└── Location C
```

Location State muss deshalb Mehrfachplatzierung unterstützen.

## Location und Distributed IPC

Distributed IPC verwendet Location Resolution zur Ermittlung aktueller Kommunikationsendpunkte.

```text
Target Identity
      ↓
Location Resolution
      ↓
Endpoint
      ↓
Distributed IPC
```

Der Endpoint darf nicht als Identität oder Authority behandelt werden.

## Location und Capabilities

Eine gültige Location gewährt keinen Zugriff.

```text
Known Location ≠ Capability
```

Nach erfolgreicher Resolution müssen weiterhin alle erforderlichen Capabilities geprüft werden.

## Trust

Trust kann standortabhängig sein.

Beispiel:

```text
Provider A / Region A → Allowed
Provider A / Region B → Restricted
```

Location selbst erzeugt jedoch keinen Trust.

## Sovereignty

Sovereignty Policies können zulässige Locations begrenzen.

```text
Candidate Location
      ↓
Sovereignty Check
      ↓
Allowed / Rejected
```

Dies gilt für:

```text
Execution
Storage
Replication
Caching
Temporary Data
Migration
Recovery
```

## Failure Domains

Locations können Failure Domains zugeordnet werden.

```text
Location
├── Device Domain
├── Power Domain
├── Network Domain
└── Site Domain
```

Diese Informationen können Placement und Replication verwenden.

## Dynamic Location

Standorte können sich während des Betriebs ändern.

Auslöser:

```text
Migration
Re-Placement
Node Failure
Load Balancing
Storage Reorganization
Network Change
Recovery
Energy Optimization
```

Location Resolver und Caches müssen solche Änderungen erkennen können.

## Location Cache

Aufgelöste Locations dürfen gecacht werden.

```text
Identity
   ↓
Cached Location
```

Caches benötigen jedoch:

```text
Version
Validity
Expiration
Invalidation
```

Ein veralteter Location Cache darf nicht dauerhaft als aktuelle Wahrheit behandelt werden.

## Unknown Location

NovaOS muss einen unbekannten Standort explizit darstellen können.

```text
Known
Changing
Multiple
Unreachable
Unknown
```

Dabei gilt:

```text
Unknown Location ≠ Missing Object
Unreachable Location ≠ Failed Object
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
LocationID
LocationType
Current Location
Previous Location
NodeID
ClusterID
Provider
Topology
Failure Domain
Trust Domain
Sovereignty Domain
Resolution State
Migration State
```

Sensible Standortinformationen müssen capability- und policygeschützt bleiben.

## Normative Anforderungen

1. NovaOS MUSS Identität und Location getrennt modellieren.
2. Location DÜRFEN NICHT als Authority oder Capability behandelt werden.
3. Stabile Objekt-, Ressourcen- und Provider-Identitäten SOLLEN Standortänderungen überleben.
4. NovaOS MUSS Location Resolution über stabile Identitäten unterstützen.
5. Location Resolution DARF keine implizite Authority erzeugen.
6. Mehrere gleichzeitige Locations MÜSSEN darstellbar sein.
7. Replica-, Cache- und authoritative Locations MÜSSEN unterscheidbar sein.
8. Placement und Location MÜSSEN getrennte Konzepte bleiben.
9. Migration DARF die logische Identität nicht unnötig verändern.
10. Distributed IPC SOLL aktuelle Endpoints über Location Resolution bestimmen.
11. Trust- und Sovereignty-Constraints MÜSSEN bei Location-Auswahl berücksichtigt werden.
12. Failure Domains SOLLEN über das Location Model beschreibbar sein.
13. Location Caches MÜSSEN invalidierbar und versionierbar sein.
14. `Unknown Location` DARF NICHT als nicht vorhandenes Objekt interpretiert werden.
15. `Unreachable` DARF NICHT automatisch als `Failed` interpretiert werden.
16. Location State MUSS autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-STORAGE-0001`
- `NPSPEC-DISTRIBUTED-SCHEDULER-0001`
- `NPSPEC-DISTRIBUTED-IPC-0001`
- `NPSPEC-DISTRIBUTED-REMOTECAPABILITY-0001`
- `NPSPEC-DISTRIBUTED-CLUSTER-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-DISTRIBUTED-REPLICATION-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-DISTRIBUTED-TRANSACTION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `ADR-ARCH-0069`

## Ergebnis

```text
Stable Identity
      ↓
Location Resolution
      ↓
Policy + Constraint Validation
      ↓
Current Location(s)
      ↓
IPC / Storage / Execution
      ↓
Migration / Replication / Re-Placement
      ↓
Updated Location State
```

NovaOS erhält damit ein einheitliches Distributed-Location-Modell, bei dem Objekte, Ressourcen und Ausführungen ihren Standort dynamisch ändern können, ohne ihre logische Identität zu verlieren oder Location mit Authority, Trust oder Sovereignty gleichzusetzen.