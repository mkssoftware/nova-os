# NPSPEC-OBSERVABILITY-STATEGRAPH-0001 – Nova Observability State Graph

## Status

Angenommen

## Kategorie

Observability / State Graph / System State

## Zweck

NovaOS definiert einen systemweiten State Graph zur strukturierten Beobachtung von Zuständen und Zustandsbeziehungen zwischen Systemobjekten.

Der State Graph ermöglicht die Frage:

```text
Was ist gerade in welchem Zustand,
wovon hängt es ab
und wie kam es zu diesem Zustand?
```

## Grundprinzipien

```text
State Graph ≠ System Model
State Graph ≠ Object Graph
State Graph ≠ Relationship Graph
State Graph ≠ Log
Observed State ≠ Desired State
Observed State ≠ Authoritative State
Unknown ≠ Healthy
```

Der State Graph beobachtet das System, steuert es aber nicht selbst.

## State Graph Model

```text
StateGraph
├── GraphID
├── Timestamp
├── Nodes
├── Edges
└── Completeness
```

## State Node

Ein beobachteter Zustand wird dargestellt durch:

```text
StateNode
├── EntityID
├── EntityType
├── State
└── ObservationTime
```

Optional:

```text
ObjectID
ResourceID
ExecutionID
TaskID
ProcessID
ProviderID
NodeID
TransactionID
LocationID
VersionID
Health
Quality
SecurityLabel
```

## State Edges

Beziehungen zwischen Zuständen werden explizit dargestellt.

Beispiele:

```text
DependsOn
Uses
HostedOn
ExecutedBy
LocatedAt
BlockedBy
WaitingFor
Provides
Consumes
ReplicatedAt
DerivedFrom
```

## Beispiel

```text
Execution
   │
   ├── ExecutedBy → Provider
   │                  │
   │                  └── Uses → GPU
   │
   ├── Reads → Object
   │
   └── WaitingFor → IPC Channel
                       │
                       └── ConnectedTo → Remote Node
```

Dadurch werden systemweite Abhängigkeiten sichtbar.

## State Transitions

Zustandsänderungen können als Transition dargestellt werden:

```text
Previous State
      ↓
Transition
      ↓
Current State
```

Optional:

```text
Cause
DecisionID
ExecutionID
TransactionID
TraceID
Timestamp
```

## Current vs Historical State

NovaOS unterscheidet:

```text
Current State
Historical State
```

Der aktuelle Graph beschreibt die derzeit bekannte Sicht.

Historische Zustände können zur Rekonstruktion verwendet werden.

```text
State T1
   ↓
State T2
   ↓
State T3
```

## Desired und Actual State

Der State Graph kann beide Zustände darstellen:

```text
Desired State
     ↕
Observed Actual State
```

Abweichungen können sichtbar gemacht werden:

```text
Desired: Running
Actual:  Failed
```

Die Reconciliation selbst bleibt Aufgabe des Systemmodells.

## State Quality

Jeder beobachtete Zustand kann eine Qualität besitzen:

```text
Confirmed
Observed
Estimated
Predicted
Stale
Incomplete
Unknown
```

```text
Predicted ≠ Observed
Stale ≠ Current
Unknown ≠ Failed
```

## Dependency Analysis

Der State Graph ermöglicht die Analyse von Abhängigkeiten.

Beispiel:

```text
Storage Failure
      ↓
Object Unavailable
      ↓
Provider Blocked
      ↓
Execution Waiting
```

Damit können Auswirkungen eines Fehlers systemweit nachvollzogen werden.

## Decision Correlation

Entscheidungen können mit Zustandsänderungen verbunden werden:

```text
State
  ↓
DecisionID
  ↓
Action
  ↓
New State
```

Damit kann nachvollzogen werden, warum NovaOS beispielsweise:

```text
Provider gewechselt
Task migriert
Replica aktiviert
Ressourcen zurückgewonnen
Rollback durchgeführt
```

hat.

## Trace Correlation

State Nodes und Transitions können referenzieren:

```text
TraceID
SpanID
ExecutionID
```

Dadurch können Zustandsänderungen mit konkreten Ausführungsabläufen verbunden werden.

## Distributed State Graph

Jeder Node kann einen lokalen Teilgraphen besitzen.

```text
Node A Graph ─┐
Node B Graph ─┼→ Distributed View
Node C Graph ─┘
```

Eine globale Sicht ist eine zusammengesetzte Beobachtung und keine automatisch atomare Wahrheit.

```text
Distributed View ≠ Global Atomic Snapshot
```

## Partitions

Bei Netzwerkpartitionen können unterschiedliche Teilgraphen entstehen.

```text
Node A View
     ≠
Node B View
```

NovaOS muss diese Unsicherheit sichtbar machen.

## State Freshness

Zustände müssen zeitlich bewertet werden können:

```text
Current
PossiblyStale
Stale
Unavailable
Unknown
```

Alte Zustände dürfen nicht stillschweigend als aktuell gelten.

## Failure Analysis

Der State Graph soll Fragen unterstützen wie:

```text
Welche Komponente ist ausgefallen?
Was hängt davon ab?
Welche Executions sind betroffen?
Welche Ressourcen fehlen?
Welche Nodes sind betroffen?
Welche Recovery-Aktion wurde ausgelöst?
```

## Self-Healing Integration

Self-Healing kann den State Graph als Beobachtungsquelle verwenden:

```text
Observe
   ↓
State Graph
   ↓
Detect Deviation
   ↓
Decision
   ↓
Recovery
   ↓
Verify
```

Der State Graph selbst führt keine Recovery-Aktion aus.

## Security

State Graphs können sensible Informationen über Systemstruktur und Abhängigkeiten offenlegen.

Zugriff wird capabilitybasiert kontrolliert:

```text
StateGraphRead
StateGraphInspect
StateGraphHistory
StateGraphExport
```

Sichtbare Nodes und Edges müssen auf die Authority des Beobachters begrenzt werden.

## Privacy

Der Graph darf keine unberechtigten Beziehungen zwischen:

```text
Users
Objects
Processes
Devices
Locations
Services
```

offenlegen.

Es gelten:

```text
Data Minimization
Security Labels
Selective Visibility
Retention
```

## Skalierung

Der vollständige Systemgraph kann sehr groß werden.

NovaOS muss daher unterstützen:

```text
Filtering
Scoped Views
Incremental Updates
Lazy Expansion
Aggregation
Bounded History
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Entity State
Dependencies
Dependents
State Transitions
Desired State
Observed State
State Quality
Freshness
Decision Correlation
Trace Correlation
Affected Components
Distributed Views
Graph Completeness
```

## Normative Anforderungen

1. NovaOS MUSS Zustände und Zustandsbeziehungen strukturiert beobachtbar machen können.
2. State Nodes MÜSSEN über stabile Systemidentitäten referenzierbar sein.
3. Beziehungen zwischen beobachteten Zuständen MÜSSEN explizit darstellbar sein.
4. Desired State und Observed State MÜSSEN unterscheidbar bleiben.
5. State Quality und Freshness MÜSSEN darstellbar sein.
6. `Unknown` DARF NICHT als gesunder oder fehlerhafter Zustand interpretiert werden.
7. Zustandsübergänge SOLLEN mit Ursachen korrelierbar sein.
8. DecisionIDs, TraceIDs und ExecutionIDs SOLLEN referenzierbar sein.
9. Abhängigkeiten und Auswirkungen von Fehlern SOLLEN graphbasiert analysierbar sein.
10. Distributed Views DÜRFEN NICHT automatisch als atomarer globaler Zustand behandelt werden.
11. Netzwerkpartitionen und unvollständige Sichtweisen MÜSSEN sichtbar bleiben.
12. Historische Zustände SOLLEN rekonstruierbar sein.
13. Der State Graph DARF keine Authority erzeugen.
14. Der State Graph DARF Self-Healing-Entscheidungen unterstützen, aber NICHT selbst Policy Enforcement übernehmen.
15. Zugriff auf State Graphs MUSS capabilitybasiert kontrolliert werden.
16. Nicht autorisierte Beziehungen DÜRFEN NICHT über Graphabfragen offengelegt werden.
17. Speicher- und Verarbeitungskosten des Graphen MÜSSEN begrenzbar sein.
18. Graph State und Completeness MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-DISTRIBUTEDTRACE-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-OBJECT-RELATIONSHIP-0001`
- `NPSPEC-OBJECT-REACTIVE-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-DISTRIBUTED-CONSISTENCY-0001`
- `NPSPEC-DISTRIBUTED-LOCATION-0001`
- `ADR-ARCH-0077`

## Ergebnis

```text
System Entities
      ↓
Observed States
      ↓
State Relationships
      ↓
System State Graph
      ↓
Dependency + Causality Analysis
      ↓
Diagnosis / Introspection / Self-Healing
```

NovaOS erhält damit eine systemweite graphbasierte Sicht auf aktuelle und historische Systemzustände, Abhängigkeiten und Zustandsübergänge. Dadurch können Fehlerketten, Auswirkungen, automatische Entscheidungen und Self-Healing-Abläufe nachvollzogen werden, ohne den beobachteten State Graph mit dem autoritativen Systemmodell oder einer global atomaren Systemwahrheit gleichzusetzen.