# NPSPEC-RESILIENCE-FAILUREDOMAIN-0001 – Nova Resilience Failure Domain

## Status

Angenommen

## Kategorie

Resilience / Failure Domain / Fault Isolation

## Zweck

NovaOS definiert Failure Domains als explizite Grenzen, innerhalb derer Fehler entstehen, erkannt, isoliert und eingedämmt werden können.

```text
System
├── Failure Domain A
├── Failure Domain B
└── Failure Domain C
```

Ein Fehler innerhalb einer Domain soll andere Domains nicht unkontrolliert beeinträchtigen.

## Grundprinzipien

```text
Failure Domain ≠ Process
Failure Domain ≠ Security Domain
Failure Domain ≠ Resource Domain
Failure Domain ≠ Physical Device
Failure Domain ≠ Recovery Unit
Shared Resource ≠ Shared Failure Fate
```

Eine Failure Domain beschreibt primär die mögliche Ausbreitung und Begrenzung von Fehlern.

## Failure Domain Model

```text
FailureDomain
├── DomainID
├── DomainType
├── Members
├── Dependencies
├── Boundary
├── Criticality
└── State
```

Optional:

```text
ParentDomainID
ChildDomains
SharedResources
RecoveryPolicy
ContainmentPolicy
HealthState
ReplicationGroup
ProviderIDs
NodeIDs
ProvenanceID
```

## Domain Types

Failure Domains können unter anderem sein:

```text
Task
Process
Service
Driver
Device
Provider
Storage
Network
Subsystem
Node
Cluster
Distributed Service
```

Domains dürfen hierarchisch aufgebaut sein.

## Hierarchie

```text
System Domain
     ↓
Subsystem Domain
     ↓
Service Domain
     ↓
Process Domain
     ↓
Task Domain
```

Ein lokaler Fehler soll zunächst innerhalb der kleinsten betroffenen Domain behandelt werden.

Erst wenn dies nicht ausreicht, wird auf eine größere Domain eskaliert.

## Domain Identity

Jede Failure Domain besitzt eine stabile `DomainID`.

```text
DomainID ≠ Name
DomainID ≠ Location
DomainID ≠ ProcessID
DomainID ≠ NodeID
```

Dadurch kann eine logische Failure Domain auch nach Migration oder Provider-Wechsel identifizierbar bleiben.

## Domain Membership

Komponenten können einer oder mehreren relevanten Fehlerbeziehungen zugeordnet sein.

```text
Failure Domain
├── Components
├── Resources
├── Providers
└── Dependencies
```

Membership muss explizit bestimmbar sein.

## Failure Boundary

Die Boundary beschreibt, welche Auswirkungen innerhalb der Domain verbleiben müssen.

Beispiele:

```text
Memory Corruption
Crash
Resource Exhaustion
Invalid State
Driver Failure
Device Failure
Communication Failure
```

Isolation und Containment setzen diese Boundary technisch durch.

## Shared Resources

Failure Domains können Ressourcen gemeinsam verwenden:

```text
Shared Memory
Shared Device
Shared Storage
Shared Network
Shared Provider
Shared Cache
```

Gemeinsame Nutzung erzeugt mögliche gemeinsame Fehlerpfade.

```text
Shared Resource
      ↓
Potential Correlated Failure
```

Diese Beziehungen müssen explizit modellierbar sein.

## Dependency Graph

Failure Domains können voneinander abhängen.

```text
Domain A
   ↓
Domain B
   ↓
Domain C
```

Der Ausfall von `Domain C` bedeutet nicht automatisch, dass `A` oder `B` selbst fehlerhaft sind.

Sie können stattdessen:

```text
Degraded
Blocked
Unavailable
Failover
```

werden.

## Correlated Failures

Mehrere Domains können durch dieselbe Ursache gleichzeitig betroffen sein.

Beispiele:

```text
Power Failure
Physical Device Failure
Shared Storage Failure
Network Failure
Firmware Failure
Node Failure
```

Redundanz innerhalb derselben zugrunde liegenden Failure Domain darf nicht als unabhängige Redundanz betrachtet werden.

## Replication

Für resiliente Replikation sollen Replicas möglichst unterschiedliche Failure Domains verwenden.

```text
Replica A → Node A
Replica B → Node B
Replica C → Node C
```

Befinden sich alle Replicas auf derselben physischen Failure Domain, schützt die Replikation nicht gegen deren Ausfall.

## Criticality

Domains können unterschiedliche Kritikalität besitzen:

```text
Low
Normal
High
Critical
```

Criticality beeinflusst:

```text
Monitoring
Detection Latency
Redundancy
Containment
Recovery Priority
Recovery Budget
```

Criticality erzeugt jedoch keine zusätzliche Authority.

## Dynamic Domains

Failure Domains dürfen sich während des Betriebs verändern.

Beispiele:

```text
Device Hotplug
Process Migration
Provider Replacement
Node Join / Leave
Storage Migration
Cluster Reconfiguration
```

Änderungen müssen konsistent im Systemmodell aktualisiert werden.

## Distributed Failure Domains

Verteilte Systeme besitzen mehrere unabhängige und überlappende Failure Domains.

```text
Datacenter
├── Node A
├── Node B
└── Node C
```

Zusätzlich können gemeinsame Abhängigkeiten bestehen:

```text
Network
Storage
Power
Trust Infrastructure
```

```text
Different Node ≠ Independent Failure Domain
```

## Isolation Integration

Failure Domains definieren die Zielgrenze für Isolation.

```text
Failure
   ↓
Determine Domain
   ↓
Isolation
   ↓
Containment
```

Isolation soll zunächst die kleinste ausreichende Domain verwenden.

## Containment Integration

Containment prüft, ob der Fehler innerhalb der vorgesehenen Failure Domain verbleibt.

```text
Failure Domain
      ↓
Containment Boundary
      ↓
Propagation Stopped?
```

Kann die Boundary nicht gehalten werden, muss eskaliert werden.

## Recovery Integration

Failure Domains können gleichzeitig Recovery Domains definieren, müssen dies jedoch nicht.

```text
Failure Domain ≠ Recovery Domain
```

Beispiel:

Ein einzelner Prozess kann die Failure Domain sein, während ein vollständiger Service aus mehreren Prozessen gemeinsam wiederhergestellt werden muss.

## Health State

Jede Domain kann einen aggregierten Zustand besitzen:

```text
Healthy
Degraded
Recovering
Failed
Contained
Unavailable
Unknown
```

Der aggregierte Zustand darf Unsicherheit einzelner kritischer Mitglieder nicht verdecken.

## Introspection

Autorisierte Komponenten sollen beobachten können:

```text
DomainID
Domain Type
Members
Parent Domain
Child Domains
Dependencies
Shared Resources
Criticality
Health State
Active Failures
Containment State
Recovery State
```

## Normative Anforderungen

1. NovaOS MUSS Failure Domains explizit modellieren können.
2. Jede Failure Domain MUSS eine stabile `DomainID` besitzen.
3. Domain Identity DARF NICHT von Name, Prozess-ID oder physischer Location abhängen.
4. Failure Domains MÜSSEN hierarchisch modellierbar sein.
5. Domain Membership MUSS explizit bestimmbar sein.
6. Failure Domains DÜRFEN NICHT automatisch mit Security Domains gleichgesetzt werden.
7. Gemeinsame Ressourcen MÜSSEN als mögliche gemeinsame Fehlerpfade modellierbar sein.
8. Abhängigkeiten zwischen Failure Domains MÜSSEN darstellbar sein.
9. Dependency Failure DARF NICHT automatisch als lokaler Fehler klassifiziert werden.
10. Korrelierte Fehler MÜSSEN berücksichtigt werden können.
11. Replikationsstrategien SOLLEN unterschiedliche Failure Domains berücksichtigen.
12. Unterschiedliche Nodes DÜRFEN NICHT automatisch als unabhängige Failure Domains betrachtet werden.
13. Criticality MUSS pro Domain definierbar sein.
14. Criticality DARF keine zusätzliche Authority erzeugen.
15. Failure Domains MÜSSEN dynamisch aktualisierbar sein.
16. Isolation SOLL die kleinste ausreichende Failure Domain verwenden.
17. Containment MUSS die Einhaltung der Failure Boundary verifizieren können.
18. Nicht haltbare Failure Boundaries MÜSSEN eskalierbar sein.
19. Failure Domain und Recovery Domain MÜSSEN getrennt modellierbar sein.
20. Domain-Struktur und Health State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-RESILIENCE-ARCH-0001`
- `NPSPEC-RESILIENCE-DETECTION-0001`
- `NPSPEC-RESILIENCE-CLASSIFICATION-0001`
- `NPSPEC-RESILIENCE-ISOLATION-0001`
- `NPSPEC-RESILIENCE-CONTAINMENT-0001`
- `NPSPEC-AUTONOMY-SELFHEALING-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-DISTRIBUTED-REPLICATION-0001`
- `NPSPEC-DISTRIBUTED-PLACEMENT-0001`
- `NPSPEC-PROCESS-ISOLATION-0001`
- `NPSPEC-PROCESS-MIGRATION-0001`
- `NPSPEC-DRIVER-ISOLATION-0001`
- `ADR-ARCH-0114`

## Ergebnis

```text
System
   ↓
Explicit Failure Domains
   ↓
Failure Detected
   ↓
Identify Affected Domain
   ↓
Isolate
   ↓
Contain
   ↓
Boundary Holds?
├── Yes → Diagnose / Recover
└── No  → Escalate Domain
```

NovaOS erhält damit ein explizites Failure-Domain-Modell, das Fehlergrenzen, gemeinsame Abhängigkeiten und korrelierte Ausfälle sichtbar macht und Isolation, Containment, Replikation sowie Recovery auf klar definierten Fehlerdomänen aufbauen lässt.