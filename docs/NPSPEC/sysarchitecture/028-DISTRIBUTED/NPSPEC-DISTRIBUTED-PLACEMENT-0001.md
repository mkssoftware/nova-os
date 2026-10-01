# NPSPEC-DISTRIBUTED-PLACEMENT-0001 – Nova Distributed Placement

## Status

Angenommen

## Kategorie

Distributed / Placement / Resource & Data Locality

## Zweck

NovaOS definiert ein einheitliches Placement-Modell für die Entscheidung, **wo** Tasks, Daten, Provider und Ressourcen innerhalb eines lokalen oder verteilten Systems platziert werden.

```text
Execution Requirements
        ↓
Placement Constraints
        ↓
Eligible Locations
        ↓
Placement Optimization
        ↓
Execution Placement
```

Placement ist eine Planungsentscheidung und erzeugt weder Authority noch Ownership.

## Grundprinzipien

```text
Placement ≠ Scheduling
Placement ≠ Migration
Placement ≠ Allocation
Placement ≠ Authorization
Placement ≠ Ownership
Placement ≠ Provider Selection

Location ≠ Identity
Available Location ≠ Eligible Location
Nearest Location ≠ Best Location
Placement Decision ≠ Capability
```

## Placement Model

Eine Placement-Anforderung wird beschrieben durch:

```text
DistributedPlacement
├── PlacementID
├── Target
├── Constraints
├── Eligible Locations
└── State
```

Optional:

```text
Preferred Locations
Affinity
Anti-Affinity
Data Locality
Resource Requirements
Latency
Deadline
Trust
Sovereignty
Security
Energy
Thermal Constraints
Availability
Failure Domains
```

## Placement Targets

Placement kann angewendet werden auf:

```text
Task
Execution
Provider
Object
Replica
Cache
Service
Resource
```

Die Regeln können je nach Target unterschiedlich sein.

## Placement Pipeline

NovaOS verwendet:

```text
Candidate Locations
        ↓
Hard Constraint Filtering
        ↓
Eligible Locations
        ↓
Soft Preference Evaluation
        ↓
Cost Evaluation
        ↓
Selected Placement
```

Hard Constraints werden immer vor Optimierungszielen ausgewertet.

## Hard Constraints

Beispiele:

```text
Required Resource
Required Capability
Security Domain
Trust Requirement
Sovereignty Domain
Data Residency
Deadline Feasibility
Required Hardware
Isolation Requirement
```

Kann kein Standort alle Hard Constraints erfüllen:

```text
No Valid Placement
      ↓
Replan / Defer / Reject / Fail
```

NovaOS darf Constraints nicht stillschweigend abschwächen.

## Soft Preferences

Nach Hard Constraint Filtering können berücksichtigt werden:

```text
Low Latency
Low Energy
Low Network Cost
Data Locality
Current Load
Preferred Provider
Thermal Distribution
Cache Locality
User Preference
```

## Compute Placement

Tasks können auf unterschiedliche Compute Nodes verteilt werden.

```text
Task
├── CPU Requirement
├── Memory Requirement
├── GPU / NPU Requirement
└── Execution Constraints
        ↓
Compute Placement
```

Der Distributed Scheduler übernimmt anschließend die konkrete zeitliche Ausführung.

```text
Placement → Where
Scheduling → When
```

## Data Placement

Objekte können anhand ihrer Storage Policy platziert werden.

```text
ObjectID
      ↓
Placement Constraints
      ↓
Storage Node
```

Dabei gelten insbesondere:

```text
Sovereignty
Durability
Availability
Trust
Latency
Capacity
Security
```

## Data Locality

NovaOS soll Compute- und Data-Placement gemeinsam betrachten.

```text
Compute → Data

oder

Data → Compute
```

Die Entscheidung kann basieren auf:

```text
Data Size
Transfer Cost
Latency
Energy
Network Load
Sovereignty
Resource Availability
```

## Affinity

Placement kann Nähe verlangen oder bevorzugen.

```text
Task A
   ↓ affinity
Task B
```

Beispiele:

```text
Same Node
Same Cluster
Same NUMA Domain
Same Accelerator
Near Storage
Near Provider
```

## Anti-Affinity

Targets können getrennt platziert werden.

```text
Replica A → Node A
Replica B → Node B
```

Anti-Affinity kann verwendet werden für:

```text
Fault Isolation
Availability
Security Isolation
Performance Isolation
```

## Failure Domains

Placement muss physische und logische Failure Domains berücksichtigen können.

Beispiele:

```text
Device
Machine
Power Domain
Network Segment
Rack
Site
Provider
Region
```

Mehrere Replicas auf derselben Failure Domain gelten nicht automatisch als unabhängige Redundanz.

## Trust

Ein Placement-Ziel muss erforderliche Trust Constraints erfüllen.

```text
Location
   +
Provider
   +
Node
   ↓
Trust Evaluation
```

```text
Unknown Trust ≠ Trusted
```

## Sovereignty

Sovereignty besitzt Vorrang vor Placement-Optimierung.

```text
Candidate Location
      ↓
Sovereignty Check
      ↓
Eligible / Rejected
```

Dies gilt für:

```text
Compute
Storage
Replicas
Caches
Temporary Data
Migration Targets
```

## Security und Capabilities

Placement erzeugt keine Zugriffsrechte.

```text
Placed on Node A
      ≠
Authorized on Node A
```

Nach Placement müssen weiterhin alle benötigten Capabilities vorhanden oder kontrolliert delegiert werden.

## Resource Admission

Vor verbindlicher Platzierung müssen benötigte Ressourcen verfügbar sein.

```text
Placement Candidate
      ↓
Resource Admission
      ↓
Accepted
      ↓
Placement Commit
```

Eine erwartete Ressource darf nicht als garantiert verfügbar behandelt werden.

## Reservations

Placement kann mit Reservations kombiniert werden.

```text
Placement
   +
Resource Reservation
   ↓
Stable Execution Plan
```

Placement allein reserviert jedoch keine Ressourcen.

## Dynamic Placement

Placement ist nicht zwingend dauerhaft.

Änderungen können ausgelöst werden durch:

```text
Node Failure
Resource Pressure
Thermal Pressure
Energy State
Network Change
Provider Failure
Trust Change
Sovereignty Change
Load Change
```

## Re-Placement

NovaOS kann eine bestehende Platzierung neu bewerten.

```text
Current Placement
      ↓
Environment Change
      ↓
Constraint Revalidation
      ↓
New Placement
```

Eine neue Platzierung kann anschließend Migration oder Rescheduling auslösen.

## Migration

```text
Placement Decision
      ↓
Migration Plan
      ↓
State Transfer
      ↓
Verification
      ↓
New Placement
```

Placement entscheidet über das Ziel.

Migration führt den Ortswechsel durch.

## Determinismus

Bei:

```text
Determinism = Required
```

kann der Execution Contract Placement einschränken.

Beispiele:

```text
Fixed Node
Fixed Provider
Fixed Hardware Class
Fixed Data Placement
```

Adaptive Optimierung darf diese Anforderungen nicht verletzen.

## Adaptive Placement

NovaOS kann historische Daten verwenden:

```text
Placement
   ↓
Execution
   ↓
Measured:
├── Latency
├── Energy
├── Network Cost
├── Resource Usage
└── Reliability
   ↓
Placement Model Update
```

Vorhersagen dürfen Hard Constraints nicht überschreiben.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
PlacementID
Target
Current Location
Eligible Locations
Rejected Locations
Hard Constraints
Soft Preferences
Affinity
Anti-Affinity
Data Locality
Failure Domain
Placement Reason
Migration State
```

## Normative Anforderungen

1. NovaOS MUSS Placement unabhängig von Scheduling, Migration und Resource Allocation modellieren.
2. Placement MUSS für Compute-, Storage-, Provider- und Service-Targets verwendbar sein.
3. Hard Constraints MÜSSEN vor Soft Preferences ausgewertet werden.
4. Nicht erfüllbare Hard Constraints DÜRFEN NICHT stillschweigend abgeschwächt werden.
5. Placement DARF keine Authority oder Capability erzeugen.
6. Data Locality SOLL bei Compute- und Storage-Placement berücksichtigt werden.
7. Affinity und Anti-Affinity MÜSSEN unterstützt werden können.
8. Placement MUSS Failure Domains berücksichtigen können.
9. Trust-, Security- und Sovereignty-Anforderungen MÜSSEN vor der Auswahl eines Placement-Ziels geprüft werden.
10. Resource Admission MUSS vor verbindlicher Ressourcenbelegung berücksichtigt werden.
11. Placement DARF nicht automatisch als Resource Reservation behandelt werden.
12. Re-Placement MUSS bei relevanten Zustandsänderungen möglich sein.
13. Migration MUSS von der Placement-Entscheidung getrennt bleiben.
14. Required Determinism DARF durch dynamisches Placement NICHT verletzt werden.
15. Adaptive Placement-Optimierung DARF Hard Constraints NICHT überschreiben.
16. Placement State und Entscheidungsgrundlagen MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-DISTRIBUTED-EXECUTION-0001`
- `NPSPEC-DISTRIBUTED-STORAGE-0001`
- `NPSPEC-DISTRIBUTED-SCHEDULER-0001`
- `NPSPEC-DISTRIBUTED-IPC-0001`
- `NPSPEC-DISTRIBUTED-REMOTECAPABILITY-0001`
- `NPSPEC-DISTRIBUTED-CLUSTER-0001`
- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-EXECUTION-RESOURCEBUDGET-0001`
- `NPSPEC-EXECUTION-DEADLINE-0001`
- `NPSPEC-EXECUTION-DETERMINISM-0001`
- `NPSPEC-EXECUTION-TRUST-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `NPSPEC-EXECUTION-PROVIDER-0001`
- `NPSPEC-EXECUTION-ENERGY-0001`
- `NPSPEC-RESOURCE-ADMISSION-0001`
- `NPSPEC-RESOURCE-RESERVATION-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0065`

## Ergebnis

```text
Execution + Data Requirements
          ↓
Placement Constraints
          ↓
Hard Constraint Filtering
          ↓
Eligible Locations
          ↓
Locality + Cost Optimization
          ↓
Placement Decision
          ↓
Admission
          ↓
Scheduling / Storage / Migration
```

NovaOS erhält damit ein gemeinsames Placement-Modell für Compute, Daten, Provider und Dienste, das physische Platzierung von Identität und Authority trennt und gleichzeitig Locality, Ressourcen, Failure Domains, Trust, Sovereignty, Security und Execution Constraints systemweit berücksichtigt.