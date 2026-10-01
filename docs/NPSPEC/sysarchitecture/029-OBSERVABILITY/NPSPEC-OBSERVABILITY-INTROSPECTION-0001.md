# NPSPEC-OBSERVABILITY-INTROSPECTION-0001 – Nova Observability Introspection

## Status

Angenommen

## Kategorie

Observability / Introspection / System Inspection

## Zweck

NovaOS definiert eine einheitliche Observability-Schnittstelle zur strukturierten Untersuchung des laufenden Systems.

Sie verbindet die verschiedenen Observability-Quellen zu einer kontrollierten Sicht auf:

```text
Systemzustand
Ressourcen
Executions
Tasks
Prozesse
Provider
Objekte
IPC
Nodes
Entscheidungen
Fehler
Abhängigkeiten
```

## Grundprinzipien

```text
Introspection ≠ Authority
Introspection ≠ Debug Privilege
Introspection ≠ Logging
Introspection ≠ Monitoring
Visible ≠ Accessible
Known ≠ Controllable
ObjectID ≠ Permission
```

Das Beobachten eines Systemobjekts darf keine zusätzlichen Rechte auf dieses Objekt erzeugen.

## Introspection Model

Eine Abfrage wird beschrieben durch:

```text
IntrospectionRequest
├── Target
├── Scope
├── Requested Information
└── Security Context
```

Das Ergebnis:

```text
IntrospectionResult
├── TargetID
├── Observed State
├── Data
├── Timestamp
├── Freshness
└── Completeness
```

Optional:

```text
ExecutionID
TraceID
DecisionID
ResourceID
ObjectID
ProviderID
NodeID
LocationID
VersionID
Quality
Security Labels
```

## Introspection Targets

Beobachtbar können sein:

```text
Kernel
Process
Task
Execution
Object
Resource
Provider
Capability Type
IPC Channel
Driver
Device
Storage
Network
Node
Cluster
Transaction
```

Neue Subsysteme sollen ihre relevanten Zustände über dasselbe Introspection-Modell bereitstellen können.

## Observability Integration

Die Introspection-Schicht verbindet:

```text
Logging
Metrics
Tracing
Distributed Tracing
Profiling
Resource Observability
Decision Observability
State Graph
```

Sie ersetzt diese Systeme nicht.

```text
Observability Sources
        ↓
Introspection
        ↓
Unified View
```

## Scoped Views

Abfragen müssen begrenzte Sichten unterstützen.

Beispiele:

```text
This Execution
This Process
This Object
This Resource
This Node
This Security Domain
System
Cluster
```

Ein vollständiger globaler Snapshot ist nicht grundsätzlich erforderlich.

## Live Introspection

NovaOS soll aktuelle Zustände untersuchen können.

Beispiel:

```text
Execution 42
├── State: Running
├── Provider: GPU0
├── Memory: 128 MiB
├── Deadline: 18 ms
└── WaitingFor: Object 81
```

Live-Daten können sich während der Abfrage verändern.

```text
Observed State ≠ Permanent State
```

## Historical Introspection

Soweit entsprechende Observability-Daten vorhanden sind, können historische Zustände rekonstruiert werden.

```text
Current
   ↓
Previous State
   ↓
Decision
   ↓
Earlier State
```

Historische Rekonstruktion darf nicht als vollständig dargestellt werden, wenn Daten fehlen.

## Relationship Inspection

Introspection kann Beziehungen zwischen Systemelementen darstellen:

```text
Execution
├── Uses → Provider
├── Reads → Object
├── Consumes → Resource
└── RunsOn → Node
```

Hierfür kann der Observability State Graph verwendet werden.

## Decision Inspection

Automatische Entscheidungen können untersucht werden:

```text
DecisionID
├── Selected Option
├── Candidates
├── Hard Constraints
├── Preferences
├── Reason Codes
└── Observation Inputs
```

Damit können adaptive Entscheidungen nachvollzogen werden.

## Execution Inspection

Für Executions sollen unter anderem sichtbar sein:

```text
ExecutionID
Operation
State
Provider
Algorithm
Resources
Budget
Latency
Deadline
Determinism
Trust
Sovereignty
Trace
```

Nur Informationen innerhalb der jeweiligen Authority dürfen sichtbar sein.

## Distributed Introspection

Introspection kann mehrere Nodes umfassen:

```text
Local Introspection
       ↓
Authorized Remote Query
       ↓
Remote Introspection
       ↓
Combined View
```

Dabei gilt:

```text
Combined View ≠ Global Atomic Snapshot
```

Jede Teilantwort benötigt eigene Freshness- und Completeness-Informationen.

## Freshness

Ergebnisse müssen ihren zeitlichen Zustand darstellen können:

```text
Current
PossiblyStale
Stale
Unavailable
Unknown
```

```text
Stale ≠ Current
Unknown ≠ Healthy
```

## Completeness

Eine Antwort kann sein:

```text
Complete
Filtered
Partial
Incomplete
Unknown
```

Security Filtering darf nicht durch Fehlermeldungen oder Metadaten die Existenz geschützter Objekte verraten.

## Security

Introspection ist capabilitybasiert.

Beispiele:

```text
IntrospectSelf
IntrospectProcess
IntrospectObject
IntrospectResource
IntrospectSystem
IntrospectCluster
```

Berechtigungen können zusätzlich begrenzen:

```text
Scope
Information Type
Object
Resource
Node
Time Range
```

## Sensitive Information

Introspection darf insbesondere keine:

```text
Passwords
Private Keys
Capability Tokens
Session Secrets
Encryption Keys
Protected Memory
```

ohne explizit dafür vorgesehene Authority offenlegen.

## Privacy

Introspection folgt:

```text
Data Minimization
Security Labels
Selective Visibility
Retention
Sovereignty
```

Eine technisch mögliche Beobachtung bedeutet nicht automatisch, dass sie sichtbar sein darf.

## Resource Limits

Introspection selbst muss ressourcenbegrenzt sein.

Begrenzbar sind:

```text
Query Rate
Result Size
Graph Depth
History Range
Remote Queries
Sampling Detail
Aggregation Cost
```

## Machine-Readable Interface

Introspection muss primär strukturierte Daten liefern.

```text
Kernel / Services
       ↓
Structured Introspection API
       ↓
CLI / GUI / Diagnostics / Developer Tools
```

Darstellungsoberflächen sind Projektionen des gleichen Datenmodells.

## Self-Healing Integration

Self-Healing kann Introspection zur Diagnose verwenden:

```text
Observe
   ↓
Introspect
   ↓
Diagnose
   ↓
Decision
   ↓
Recovery
   ↓
Verify
```

Introspection selbst darf keine Recovery-Policy ausführen.

## Normative Anforderungen

1. NovaOS MUSS eine einheitliche strukturierte Introspection-Schnittstelle bereitstellen.
2. Introspection MUSS von Authority und Policy Enforcement getrennt bleiben.
3. Observability-Subsysteme SOLLEN über ein gemeinsames Introspection-Modell zugänglich sein.
4. Introspection MUSS Scoped Views unterstützen.
5. Ergebnisse MÜSSEN Freshness darstellen können.
6. Ergebnisse MÜSSEN Completeness darstellen können.
7. `Unknown` DARF NICHT als bekannter Zustand interpretiert werden.
8. Live-Zustände DÜRFEN NICHT als unveränderliche Zustände dargestellt werden.
9. Historische Rekonstruktionen MÜSSEN fehlende Daten kenntlich machen.
10. Beziehungen zwischen Systemelementen SOLLEN introspektierbar sein.
11. Decision-, Trace- und Execution-Daten SOLLEN korrelierbar sein.
12. Distributed Introspection DARF eine zusammengesetzte Sicht NICHT als global atomaren Snapshot darstellen.
13. Introspection MUSS capabilitybasiert kontrolliert werden.
14. Nicht autorisierte Informationen DÜRFEN NICHT über Ergebnisse, Metadaten oder Fehlermeldungen offengelegt werden.
15. Capability Secrets und Credentials DÜRFEN NICHT durch allgemeine Introspection offengelegt werden.
16. Introspection-Abfragen MÜSSEN ressourcenbegrenzt werden können.
17. Privacy-, Security-, Retention- und Sovereignty-Regeln MÜSSEN erhalten bleiben.
18. Introspection MUSS maschinenlesbare Schnittstellen für Systemwerkzeuge bereitstellen.

## Abhängigkeiten

- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-DISTRIBUTEDTRACE-0001`
- `NPSPEC-OBSERVABILITY-PROFILING-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-OBJECT-IDENTITY-0001`
- `NPSPEC-SEMANTIC-QUERY-0001`
- `NPSPEC-CAPABILITY-IDENTITY-0001`
- `NPSPEC-DISTRIBUTED-LOCATION-0001`
- `NPSPEC-PRIVACY-MINIMIZATION-0001`
- `NPSPEC-EXECUTION-SOVEREIGNTY-0001`
- `ADR-ARCH-0078`

## Ergebnis

```text
Logs + Metrics + Traces
Resources + Decisions + State
            ↓
Unified Introspection
            ↓
Capability-Controlled Views
            ↓
System Understanding
            ↓
Diagnostics / Tools / Self-Healing
```

NovaOS erhält damit eine einheitliche, strukturierte und capabilitykontrollierte Introspection-Schicht, über die Menschen und Systemkomponenten den Zustand und die Zusammenhänge des laufenden Systems verstehen können, ohne Beobachtbarkeit mit Kontrolle oder Berechtigung gleichzusetzen.