# ADR-STATE-0001 – Globaler System State Graph

## Status

Angenommen

## Kategorie

State Management / System Model / Introspection / Declarative Architecture

## Kontext

NovaOS besteht aus vielen miteinander verbundenen Objekten, Ressourcen und Diensten.

Beispiele:

```text
Processes
Services
Devices
Volumes
Files
Objects
Capabilities
Users
Sessions
Network Connections
Compute Providers
AI Models
Realtime Domains
Execution Contracts
```

Klassische Betriebssysteme verteilen diese Zustände über zahlreiche voneinander getrennte Subsysteme.

Dadurch entstehen unterschiedliche und teilweise widersprüchliche Sichtweisen auf das System.

NovaOS benötigt eine gemeinsame semantische Darstellung des relevanten Systemzustands.

Diese Darstellung soll insbesondere ermöglichen:

```text
Introspection
Dependency Analysis
Declarative Reconciliation
Observability
Scheduling
Self-Healing
Security Analysis
Resource Planning
Failure Analysis
```

## Entscheidung

NovaOS führt einen **Global System State Graph** als systemweite logische Repräsentation des relevanten Systemzustands ein.

```text
                 System State Graph

                    System
                       │
        ┌──────────────┼──────────────┐
        ↓              ↓              ↓
     Objects        Services       Devices
        │              │              │
        ↓              ↓              ↓
   Executions ───→ Capabilities ←── Drivers
        │
        ↓
    Resources
```

Der Graph bildet Systementitäten als Nodes und ihre semantischen Beziehungen als Edges ab.

Er ist keine einzelne globale Datenbank und kein monolithischer Speicher aller Systemdaten.

## Grundprinzip

Der Graph beschreibt:

```text
What exists?
What state is it in?
How is it related?
Who owns it?
What depends on it?
What is using it?
Where is it located?
What version is active?
```

Damit entsteht eine gemeinsame logische Sicht auf NovaOS.

## Nodes

Systementitäten werden als Nodes repräsentiert.

Beispiele:

```text
Object
Process
Service
Capability
Device
Driver
Volume
Network Endpoint
Execution
Execution Contract
Realtime Domain
AI Model
Compute Provider
User
Session
Policy
```

Jeder Node besitzt mindestens:

```text
Stable ID
Semantic Type
State
Version / Generation
Authority Source
```

## Stable Identity

Nodes werden über stabile IDs identifiziert.

```text
Node ID
    ≠
Path
    ≠
Name
    ≠
Handle
```

Eine Änderung von:

```text
Name
Location
Path
Runtime Handle
```

erzeugt nicht automatisch eine neue Identität.

## Edges

Beziehungen zwischen Nodes werden als typisierte Edges dargestellt.

Beispiele:

```text
USES
DEPENDS_ON
PROVIDES
OWNS
CONTAINS
RUNS_ON
MOUNTED_ON
CONNECTED_TO
AUTHORIZED_BY
CONTROLLED_BY
DERIVED_FROM
REPLICA_OF
EXECUTES
REQUIRES
```

Beziehungen besitzen damit eine explizite Semantik.

## Typisierte Beziehungen

Eine Edge ist nicht lediglich:

```text
A → B
```

sondern beispielsweise:

```text
Process A
    ──USES──>
Capability B
```

oder:

```text
Service A
    ──DEPENDS_ON──>
Device B
```

Dadurch können Systemkomponenten Beziehungen maschinell interpretieren.

## Graph ist eine logische Sicht

Der Global System State Graph wird nicht als eine einzige zentrale mutable Datenstruktur implementiert.

Stattdessen:

```text
Subsystem State
      ↓
State Providers
      ↓
Normalized State Model
      ↓
Global Logical Graph
```

Die jeweiligen Subsysteme bleiben Authority für ihren eigenen Zustand.

Beispiele:

```text
Scheduler
→ Execution State

VFS / Object System
→ Object State

Driver Framework
→ Device State

Network Stack
→ Network State
```

Der Graph aggregiert diese Zustände zu einer gemeinsamen Sicht.

## Keine zweite Wahrheit

Der Graph darf keine konkurrierende zweite Source of Truth erzeugen.

```text
Authoritative Subsystem
        ↓
Published State
        ↓
State Graph
```

Änderungen erfolgen grundsätzlich über die zuständige Capability oder das zuständige Subsystem.

Nicht:

```text
Modify Graph
     ↓
Hope System Changes
```

## Desired und Actual State

Der Graph kann sowohl gewünschten als auch tatsächlichen Zustand darstellen.

```text
Desired State
      │
      ↓
Reconciliation
      │
      ↓
Actual State
```

Beispiel:

```text
Service X

Desired:
Running

Actual:
Stopped
```

Diese Differenz kann durch das deklarative Systemmodell verarbeitet werden.

## State Versioning

Graph-Zustände müssen versionierbar sein.

```text
State Generation 1042
        ↓
State Generation 1043
        ↓
State Generation 1044
```

Damit können Leser erkennen, ob sich relevante Informationen während einer Operation geändert haben.

## Snapshots

NovaOS kann konsistente oder zweckgebunden konsistente Graph-Snapshots erzeugen.

```text
Live State Graph
      ↓
Snapshot
      ↓
Analysis
```

Snapshots können verwendet werden für:

```text
Diagnostics
Planning
Debugging
Record/Replay
Failure Analysis
Security Analysis
```

Ein Snapshot ist nicht automatisch ein vollständiger Snapshot sämtlicher Systemdaten.

## State Freshness

Graphinformationen besitzen eine erkennbare Aktualität.

Beispielsweise:

```text
Current
Stale
Unknown
Unavailable
```

Es gilt:

```text
Unknown
≠
False

Unknown
≠
Absent
```

NovaOS darf fehlende Informationen nicht automatisch als negativen Zustand interpretieren.

## Event Updates

State Provider können Änderungen ereignisbasiert veröffentlichen.

```text
Device Added
      ↓
State Event
      ↓
Graph Update
```

Dadurch muss der Graph nicht permanent vollständig neu aufgebaut werden.

## Queries

Subsysteme können semantische Graph-Abfragen durchführen.

Beispiele:

```text
Welche Services hängen von Device X ab?

Welche Executions verwenden Object Y?

Welche Capabilities besitzt Process Z?

Welche Ressourcen gehören zu Realtime Domain A?

Welche Objekte liegen auf einem ausfallenden Volume?
```

Dadurch werden systemweite Analysen möglich, ohne dass jedes Subsystem alle anderen intern kennen muss.

## Dependency Analysis

Der Graph bildet Abhängigkeiten explizit ab.

```text
Application
    ↓
Capability
    ↓
Service
    ↓
Driver
    ↓
Device
```

Fällt das Device aus, kann NovaOS bestimmen, welche höheren Komponenten potenziell betroffen sind.

## Self-Healing

Self-Healing verwendet den Graph zur Bestimmung von Auswirkungen und Recovery-Pfaden.

```text
Failure
   ↓
Affected Node
   ↓
Dependency Graph
   ↓
Impact Analysis
   ↓
Recovery Plan
```

Der Graph entscheidet jedoch nicht selbst über Recovery.

Er liefert den Zustand und die Beziehungen für die zuständigen Policy- und Autonomy-Systeme.

## Scheduling

Execution Planning kann den Graph verwenden, um verfügbare Ressourcen und Abhängigkeiten zu bestimmen.

Beispiel:

```text
Execution
    ↓
Requires GPU
    ↓
Available Providers
    ↓
Resource State
    ↓
Placement
```

Der Graph ersetzt dabei nicht den Scheduler.

## Security

Security-relevante Beziehungen können ebenfalls dargestellt werden.

Beispiel:

```text
Process
   ↓
Security Context
   ↓
Capability
   ↓
Object
```

Der Graph selbst erzeugt keine Berechtigung.

```text
Graph Visibility
    ≠
Authorization
```

Die Kenntnis eines Nodes oder einer Edge verleiht keinerlei Zugriff auf die repräsentierte Ressource.

## Sichtbarkeit

Nicht jeder Prozess darf den vollständigen globalen Graph sehen.

Graph Queries werden entsprechend:

```text
Identity
Capabilities
Security Context
Privacy
Sovereignty
```

gefiltert.

Ein Prozess erhält eine autorisierte Graph View.

```text
Global State Graph
        ↓
Authorization Filter
        ↓
Process State View
```

## Privacy

Sensible Informationen dürfen nicht allein deshalb global sichtbar werden, weil sie Teil des Systemzustands sind.

Nodes und Edges können daher unterschiedliche Sichtbarkeitsregeln besitzen.

Metadaten werden ebenfalls als potenziell schützenswerte Informationen behandelt.

## Distributed State

Der State Graph kann entfernte Ressourcen darstellen.

```text
Local Node
Remote Node
Cluster Node
Remote Capability
Remote Object
```

Location Transparency bleibt erhalten, aber der tatsächliche Ort kann als semantische Eigenschaft sichtbar sein.

Remote State muss seine Freshness und Vertrauenswürdigkeit erkennen lassen.

## Partial Knowledge

In verteilten Systemen kann NovaOS keinen jederzeit vollständig aktuellen globalen Zustand garantieren.

Deshalb unterstützt das Modell explizit:

```text
Known
Stale
Unknown
Unavailable
Conflicting
```

Der Begriff „global“ bezeichnet die gemeinsame logische Sicht, nicht die Behauptung einer jederzeit synchronen globalen Wahrheit.

## Realtime

Realtime-Subsysteme dürfen nicht von langsamen globalen Graph-Abfragen auf ihrem kritischen Pfad abhängig sein.

Benötigter State kann:

```text
Precomputed
Cached
Pinned
Snapshot
Locally Published
```

werden.

Der Global State Graph darf keine unbounded Latenz in Hard-Realtime-Pfade einführen.

## AI und Agenten

AI- und Agent-Systeme können den Graph über autorisierte Context Services nutzen.

```text
System State Graph
       ↓
Context Service
       ↓
Authorization
       ↓
Filtered Context View
       ↓
AI / Agent
```

Ein AI-Modell erhält keinen direkten unbegrenzten Zugriff auf den globalen Systemzustand.

## Observability

Der State Graph integriert Observability-Daten, ohne Logs, Metrics und Traces zu ersetzen.

```text
State Graph
→ What exists and how it relates

Logs
→ What happened

Metrics
→ How much / how often

Traces
→ How execution flowed
```

Diese Informationen können über gemeinsame IDs miteinander korreliert werden.

## Record/Replay

Record/Replay kann relevante Graph-Versionen referenzieren.

```text
Execution Record
      ↓
State Generation
Object Versions
Configuration Versions
      ↓
Replay
```

Es ist nicht notwendig, bei jeder Execution den vollständigen globalen Graph zu kopieren.

## Graph Evolution

Neue Node- und Edge-Typen müssen ergänzt werden können, ohne bestehende Verbraucher unnötig zu brechen.

Dafür verwendet NovaOS:

```text
Semantic Types
Versioned Schemas
Stable IDs
Extensible Metadata
```

Unbekannte Typen müssen als solche behandelbar bleiben.

## Failure Verhalten

Fällt ein State Provider aus, wird sein Zustand nicht automatisch gelöscht.

Stattdessen kann der betroffene Teil des Graphen markiert werden als:

```text
Stale
Unknown
Unavailable
```

Dadurch bleibt der Unterschied zwischen:

```text
Resource does not exist
```

und:

```text
Resource state cannot currently be determined
```

erhalten.

## Normative Anforderungen

1. NovaOS MUSS einen globalen logischen System State Graph bereitstellen.
2. Systementitäten MÜSSEN über stabile IDs referenzierbar sein.
3. Nodes MÜSSEN semantisch typisiert sein.
4. Beziehungen zwischen Nodes MÜSSEN typisierbar sein.
5. Pfad, Name und Runtime Handle DÜRFEN nicht als globale Objektidentität verwendet werden.
6. Zuständige Subsysteme MÜSSEN Authority über ihren jeweiligen Zustand behalten.
7. Der State Graph DARF keine konkurrierende zweite Source of Truth erzeugen.
8. Desired und Actual State MÜSSEN unterscheidbar sein.
9. Graphzustände MÜSSEN versionierbar sein.
10. Zweckgebundene State Snapshots MÜSSEN möglich sein.
11. State Freshness MUSS ausdrückbar sein.
12. `Unknown` DARF nicht automatisch als `False` oder `Absent` interpretiert werden.
13. State Provider SOLLEN Änderungen ereignisbasiert veröffentlichen können.
14. Der Graph MUSS semantische Queries über Nodes und Beziehungen ermöglichen.
15. Abhängigkeiten MÜSSEN explizit modellierbar sein.
16. Graph Visibility DARF keine Authorization erzeugen.
17. Graph Queries MÜSSEN Security- und Privacy-Grenzen respektieren.
18. Prozesse SOLLEN nur autorisierte Graph Views erhalten.
19. Distributed State MUSS Location und Freshness ausdrücken können.
20. Der globale Graph DARF keine vollständig synchrone globale Wahrheit voraussetzen.
21. Hard-Realtime-Pfade DÜRFEN nicht von unbounded globalen Graph-Abfragen abhängig sein.
22. AI und Agenten DÜRFEN keinen impliziten vollständigen Zugriff auf den State Graph erhalten.
23. Observability-Daten SOLLEN über gemeinsame IDs mit Graph Nodes korrelierbar sein.
24. Record/Replay MUSS relevante State-Generationen referenzieren können.
25. Node- und Edge-Schemas MÜSSEN evolvierbar sein.
26. Ausfall eines State Providers DARF dessen Ressourcen nicht automatisch als nicht existent darstellen.
27. Der State Graph MUSS partielles und unbekanntes Wissen explizit darstellen können.
28. Die Korrektheit des Kernsystems DARF nicht von einer zentralen Graph-Datenbank abhängen.

## Konsequenzen

### Positive Konsequenzen

- NovaOS erhält eine einheitliche Sicht auf Systemzustand und Beziehungen,
- Abhängigkeiten können systemweit analysiert werden,
- Self-Healing kann Auswirkungen von Fehlern gezielter bestimmen,
- Scheduling und Execution Planning erhalten eine gemeinsame Zustandsbasis,
- Observability, Security und Introspection lassen sich über stabile IDs korrelieren,
- verteilte Ressourcen können in dasselbe semantische Modell integriert werden,
- AI und Agenten können kontrollierte Systemkontexte erhalten.

### Negative Konsequenzen

- State Provider und Graph-Schemas erhöhen die Architekturkomplexität,
- verteilte Zustände können stale oder unvollständig sein,
- Graph Queries benötigen konsequente Security- und Privacy-Filter,
- große Systeme benötigen effiziente Indizes, Caches und inkrementelle Updates.

## Abhängigkeiten

- `ADR-ARCH-0003_Deklaratives_Systemmodell`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0013_Introspection`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0019_Evolvierbar`
- `ADR-OBJECT-0001`
- `ADR-OBJECT-0002`
- `ADR-SEMANTIC-0001`
- `ADR-DECLARATIVE-0001`
- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_und_Tracing`
- `ADR-OBSERVABILITY-0005_System_State_Graph`
- `ADR-AUTONOMY-0004_Self_Diagnosis`
- `ADR-AUTONOMY-0005_Self_Healing`
- `ADR-AI-0008_Context_Services`
- `ADR-REALTIME-0005_Deterministic_Execution`

## Zugehörige NPSPECs

- `NPSPEC-STATE-GRAPH-0001`
- `NPSPEC-STATE-NODE-0001`
- `NPSPEC-STATE-EDGE-0001`
- `NPSPEC-STATE-PROVIDER-0001`
- `NPSPEC-STATE-QUERY-0001`
- `NPSPEC-STATE-SNAPSHOT-0001`
- `NPSPEC-STATE-FRESHNESS-0001`
- `NPSPEC-STATE-VIEW-0001`

## Ergebnis

NovaOS erhält eine gemeinsame semantische Sicht auf seinen Zustand:

```text
Authoritative Subsystems
          ↓
     State Providers
          ↓
┌──────────────────────────┐
│ Global System State Graph│
│                          │
│ Nodes + Typed Relations  │
└──────────────────────────┘
          ↓
 ┌────────┼────────┬─────────┐
 ↓        ↓        ↓         ↓
Query  Diagnose  Planning  Recovery
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll nicht nur wissen,
welche Komponenten existieren.

Es soll auch wissen,
wie diese Komponenten
miteinander zusammenhängen.

Der globale State Graph
ist dabei keine zweite Wahrheit,
sondern die gemeinsame
semantische Sicht auf die
Wahrheiten der einzelnen
Subsysteme.
```