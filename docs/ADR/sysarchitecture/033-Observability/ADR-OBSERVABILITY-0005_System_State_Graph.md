# ADR-OBSERVABILITY-0005 – System State Graph

## Status

Angenommen

## Kategorie

Observability Architecture / System State

## Kontext

Logs, Metrics, Traces und Decision Traces zeigen einzelne Ereignisse und Ausführungsabläufe.

Für Diagnose, Introspection und Self-Healing benötigt NovaOS zusätzlich eine zusammenhängende Sicht auf den **aktuellen Systemzustand und dessen Beziehungen**.

Beispiele:

```text
Process uses Capability
Capability uses Provider
Provider runs on Device
Execution uses Object
Service depends on Service
Object stored on Volume
Resource reserved by Execution
```

Isolierte Listen reichen dafür nicht aus.

## Entscheidung

NovaOS führt einen **System State Graph** als beobachtbare, verknüpfte Darstellung des relevanten Systemzustands ein.

```text
System Objects
     +
Relationships
     +
Observed State
     ↓
System State Graph
```

Der Graph ist eine beobachtbare Projektion des Systems und nicht die alleinige Quelle der Wahrheit für alle Subsysteme.

## Graphmodell

Der System State Graph besteht aus:

```text
Nodes
Edges
State
Metadata
Evidence
```

Nodes repräsentieren relevante Systemobjekte.

Edges beschreiben deren Beziehungen.

## Nodes

Typische Nodes sind:

```text
Device
Node
Process
Service
Capability
Algorithm
Compute Provider
Execution
Object
Volume
Network Endpoint
Resource
Transaction
```

Nodes sollen soweit möglich bestehende stabile Systemidentitäten verwenden.

## Relationships

Beziehungen werden explizit typisiert.

Beispiele:

```text
Process ──uses────────→ Capability
Capability ─provides──→ Service
Execution ──uses──────→ Algorithm
Algorithm ──runs_on───→ Provider
Provider ──located_on─→ Device
Execution ──reads─────→ Object
Object ─────stored_on─→ Volume
Service ───depends_on─→ Service
Execution ─reserves───→ Resource
```

Eine Beziehung besitzt eine definierte Semantik und darf nicht allein aus räumlicher oder zeitlicher Nähe abgeleitet werden.

## Observed State

Nodes und Beziehungen können beobachtbare Zustände besitzen.

Beispiele:

```text
Healthy
Degraded
Unavailable
Failed
Unknown
Starting
Stopping
```

Dabei gilt:

```text
Unknown ≠ Healthy
Unknown ≠ Failed
```

## Desired und Actual State

Der Graph kann Desired und Actual State miteinander korrelieren.

```text
Desired State
      ↓
System State Graph
      ↑
Observed Actual State
```

Damit können Abweichungen sichtbar gemacht werden.

Der Graph ersetzt jedoch nicht das deklarative Desired-State-Modell.

## Herkunft der Informationen

Der Graph wird aus bestehenden Systeminformationen aufgebaut.

Beispiele:

```text
Introspection
Observability
Resource Tracing
Execution Planning
Capability Registry
Algorithm Registry
Provider State
Storage State
Network State
Health State
```

Jede relevante Information soll ihrer Quelle oder Evidence zugeordnet werden können.

## Aktualität

Systemzustand verändert sich kontinuierlich.

Graphinformationen können deshalb Zustände besitzen wie:

```text
Current
Stale
Unknown
```

Veraltete Informationen dürfen nicht ohne Kennzeichnung als aktueller Zustand interpretiert werden.

## Execution Integration

Ausführungen werden als Teil des Graphen sichtbar.

```text
Execution
 ├── uses → Capability
 ├── uses → Algorithm
 ├── runs_on → Provider
 ├── reads → Input Object
 ├── produces → Output Object
 └── reserves → Resource
```

Dadurch lassen sich Execution Planning, Resource Usage und Datenabhängigkeiten gemeinsam betrachten.

## Distributed State

Der System State Graph kann Informationen mehrerer Nodes zusammenführen.

```text
Node A ──network── Node B
  │                  │
Provider A         Provider B
  │                  │
Execution ────────────┘
```

Location Transparency darf die physische Verteilung nicht aus dem Graphen entfernen.

Standort bleibt eine relevante Eigenschaft.

## Kausalität

Der Graph beschreibt primär Zustand und Beziehungen.

Zeitliche oder kausale Abläufe bleiben Aufgabe von Tracing und Event-Historien.

```text
System State Graph
    → Was ist miteinander verbunden?

Tracing
    → Was geschah in welcher Ausführung?
```

Beide Modelle müssen korrelierbar sein.

## Self-Healing

`Nova.Resilience` kann den Graphen verwenden, um Auswirkungen eines Fehlers zu bestimmen.

```text
Failed Provider
      ↓
Affected Executions
      ↓
Dependent Capabilities
      ↓
Affected Services
      ↓
Recovery Scope
```

Damit kann der kleinste sichere Recovery Scope besser bestimmt werden.

Der Graph selbst führt keine Recovery Policy aus.

## Security und Privacy

Nicht jeder Consumer darf den vollständigen System State Graph sehen.

Graphabfragen unterliegen:

```text
Authorization
Data Classification
Privacy
Data Sovereignty
Security Policy
```

Sensible Beziehungen oder Metadata können gefiltert oder verborgen werden.

## Introspection

Der Graph bildet eine zentrale Grundlage für systemweite Introspection.

NovaOS soll beispielsweise beantworten können:

```text
Welche Services hängen von diesem Provider ab?
Welche Execution verwendet dieses Objekt?
Welche Ressourcen gehören zu dieser Execution?
Wo befindet sich dieses Objekt?
Welche Komponenten wären von einem Ausfall betroffen?
```

## Normative Anforderungen

1. NovaOS MUSS einen systemweiten logischen System State Graph bereitstellen können.
2. Der Graph MUSS Nodes und typisierte Relationships unterscheiden.
3. Nodes SOLLEN bestehende stabile Systemidentitäten verwenden.
4. Beziehungen MÜSSEN eine definierte Semantik besitzen.
5. `Unknown` MUSS von bekannten Zuständen unterschieden werden.
6. Veraltete Zustandsinformationen MÜSSEN als solche erkennbar sein.
7. Desired und Actual State MÜSSEN korrelierbar sein.
8. Der Graph DARF das deklarative Desired-State-Modell nicht ersetzen.
9. Execution-, Capability-, Algorithmus-, Provider-, Objekt- und Ressourcenbeziehungen MÜSSEN darstellbar sein.
10. Verteilte Systemzustände MÜSSEN nodeübergreifend darstellbar sein.
11. Physische Location DARF durch Location Transparency nicht verborgen werden.
12. System State Graph und Distributed Tracing MÜSSEN korrelierbar sein.
13. Zustandsinformationen SOLLEN mit ihrer Quelle oder Evidence verknüpfbar sein.
14. `Nova.Resilience` MUSS den Graphen für Impact- und Recovery-Analyse verwenden können.
15. Der Graph DARF Recovery Policy nicht selbst bestimmen.
16. Graphzugriffe MÜSSEN Authorization-, Privacy-, Security- und Data-Sovereignty-Regeln einhalten.

## Konsequenzen

### Positive Konsequenzen

- systemweite Abhängigkeiten werden sichtbar,
- Fehlerauswirkungen können gezielt analysiert werden,
- Execution, Ressourcen und Daten lassen sich gemeinsam betrachten,
- Introspection erhält eine einheitliche Systemansicht,
- Self-Healing kann Recovery Scopes präziser bestimmen.

### Negative Konsequenzen

- der Graph muss kontinuierlich aktualisiert werden,
- verteilte Zustände können zeitweise inkonsistent oder veraltet sein,
- Zugriff auf den Graphen benötigt klare Sicherheitsregeln.

## Abhängigkeiten

- `ADR-OBSERVABILITY-0001_Unified_Logging_Metrics_Tracing`
- `ADR-OBSERVABILITY-0002_Distributed_Tracing`
- `ADR-OBSERVABILITY-0003_Resource_Tracing`
- `ADR-OBSERVABILITY-0004_Decision_Tracing`
- `ADR-ARCH-0013_Introspection`
- `ADR-DECLARATIVE-0001_Desired_State_und_Actual_State`
- `ADR-OBJECT-0001_Unified_System_Object_Model`
- `ADR-OBJECT-0002_Stabile_Object_Identity`
- `ADR-EXECUTION-0010_Automatische_Ausführungsplanung`
- `ADR-DISTRIBUTED-0009_Location_Transparent_Execution`
- `ADR-RESILIENCE-0003_Hierarchical_Failure_Domains`
- `ADR-RESILIENCE-0004_Kleinster_sicherer_Recovery_Scope`
- `ADR-RESILIENCE-0008_Health_State_Model`

## Zugehörige NPSPECs

- `NPSPEC-OBSERVABILITY-SYSTEM-STATE-GRAPH-0001`
- `NPSPEC-OBSERVABILITY-STATE-NODE-0001`
- `NPSPEC-OBSERVABILITY-STATE-RELATIONSHIP-0001`
- `NPSPEC-OBSERVABILITY-STATE-EVIDENCE-0001`
- `NPSPEC-OBSERVABILITY-STATE-QUERY-0001`

## Ergebnis

NovaOS erhält eine verknüpfte Sicht auf seinen aktuellen Systemzustand:

```text
Objects + Components + Executions
              ↓
       Typed Relationships
              ↓
        Observed State
              ↓
      System State Graph
              ↓
 Introspection / Diagnosis
              ↓
 Impact Analysis / Resilience
```

Der zentrale Grundsatz lautet:

```text
NovaOS soll nicht nur wissen,
welche Komponenten existieren.

Es soll verstehen können,
wie sie zusammenhängen,
welchen Zustand sie besitzen
und welche Teile des Systems
von einer Änderung betroffen sind.
```