# NPSPEC-OBSERVABILITY-PROVENANCE-0001 – Nova Observability Provenance

## Status

Angenommen

## Kategorie

Observability / Provenance / Causality / Evidence

## Zweck

NovaOS definiert ein systemweites Provenance-Modell für Observability-Daten.

Damit soll nachvollziehbar sein:

```text
Woher stammt eine Beobachtung?
Wer oder was hat sie erzeugt?
Wann wurde sie erzeugt?
Auf welchen Daten basiert sie?
Wie wurde sie verarbeitet?
Wie zuverlässig ist sie?
```

Provenance ermöglicht die Herkunfts- und Verarbeitungskette von Logs, Metrics, Traces, Profiling-Daten, Entscheidungen und beobachteten Systemzuständen.

## Grundprinzipien

```text
Provenance ≠ Trust
Provenance ≠ Identity
Provenance ≠ Authority
Provenance ≠ Integrity
Provenance ≠ Audit

Known Source ≠ Trusted Source
Signed Data ≠ Correct Data
Derived Data ≠ Original Data
Missing Provenance ≠ Invalid Data
```

Provenance beschreibt Herkunft und Transformation, bewertet diese aber nicht automatisch.

## Provenance Model

```text
ObservationProvenance
├── ProvenanceID
├── Source
├── CreationTime
├── ObservationType
└── Derivation
```

Optional:

```text
SourceID
NodeID
ProviderID
ExecutionID
TraceID
DecisionID
ObjectID
ResourceID
VersionID
CollectorID
TransformationID
Integrity Evidence
Trust Information
Quality
Security Label
```

## Provenance Chain

Beobachtungsdaten können mehrere Verarbeitungsschritte durchlaufen.

```text
Original Observation
        ↓
Aggregation
        ↓
Filtering
        ↓
Transformation
        ↓
Derived Observation
```

Jeder relevante Schritt soll nachvollziehbar bleiben.

## Source Types

Quellen können beispielsweise sein:

```text
Kernel
Driver
Service
Application
Device
Hardware Counter
Provider
Remote Node
Collector
Derived Analysis
Prediction Engine
```

Die Quelle soll über stabile Identitäten referenziert werden.

## Original und Derived Data

NovaOS unterscheidet:

```text
Original
Derived
Aggregated
Estimated
Predicted
Imported
```

Beispiel:

```text
CPU Hardware Counter
        ↓
Raw Metric
        ↓
Aggregation
        ↓
Average CPU Utilization
```

Der aggregierte Wert darf nicht als unverarbeiteter Hardwarewert dargestellt werden.

## Transformation Provenance

Transformationen sollen beschreibbar sein durch:

```text
TransformationID
Input Provenance
Operation
Provider
Version
Timestamp
Output Provenance
```

Dadurch können abgeleitete Observability-Daten bis zu ihren Eingaben zurückverfolgt werden.

## Decision Provenance

Decision Observability kann dokumentieren, welche Beobachtungen eine Entscheidung beeinflusst haben.

```text
Metrics ───────┐
Resource State ├→ Decision
State Graph ───┤
Policy ────────┘
```

Die verwendeten Eingaben können über ihre `ProvenanceID`s referenziert werden.

## State Provenance

Ein Zustand im State Graph kann angeben:

```text
State
├── ObservedBy
├── ObservedAt
├── Source
├── Quality
└── ProvenanceID
```

Dadurch bleibt erkennbar, ob ein Zustand direkt beobachtet, abgeleitet oder vorhergesagt wurde.

## Distributed Provenance

Bei verteilten Systemen muss Provenance über Node-Grenzen erhalten bleiben.

```text
Node A Observation
        ↓
Node B Aggregation
        ↓
Node C Analysis
```

Jede Stufe ergänzt die bestehende Provenance-Kette, ohne die vorherige Herkunft zu verlieren.

## Trust Integration

Trust kann Provenance als Eingabe verwenden:

```text
Provenance
    ↓
Trust Evaluation
```

Dabei gilt:

```text
Provenance beschreibt Herkunft.
Trust bewertet diese Herkunft im Kontext.
```

Beide Konzepte bleiben getrennt.

## Integrity

Kritische Provenance-Daten können mit Integritätsmechanismen geschützt werden.

Beispiele:

```text
Hash
Signature
Authenticated Storage
Integrity Chain
```

Integrität beweist jedoch nicht die inhaltliche Richtigkeit einer Beobachtung.

## Provenance Graph

Provenance kann als gerichteter Graph dargestellt werden:

```text
Observation A
     ↓
Aggregation B
     ↓
Analysis C
     ↓
Decision D
```

Dadurch können Herkunft und Ableitung rückwärts untersucht werden.

## Incomplete Provenance

Provenance kann unvollständig sein:

```text
Complete
Partial
Missing
Unknown
```

```text
Missing Provenance ≠ False Data
Complete Provenance ≠ Correct Data
```

Die Unsicherheit muss sichtbar bleiben.

## Security

Provenance kann interne Systemstrukturen offenlegen.

Zugriff muss capabilitybasiert kontrolliert werden.

Beispiele:

```text
ProvenanceRead
ProvenanceInspect
ProvenanceHistory
ProvenanceExport
```

Capability Tokens oder Credentials dürfen nicht Bestandteil der Provenance-Kette sein.

## Privacy

Provenance darf keine unnötige dauerhafte Historie sensibler Aktivitäten erzeugen.

Es gelten:

```text
Data Minimization
Security Labels
Retention
Selective Visibility
Controlled Export
```

Geschützte Quellen können durch autorisierte Referenzen dargestellt werden, ohne ihre internen Details offenzulegen.

## Retention

Provenance und zugehörige Observability-Daten können unterschiedliche Lebenszeiten besitzen.

Referenzen auf entfernte Daten müssen eindeutig als nicht mehr verfügbar erkennbar sein.

```text
Reference Exists
      +
Source Expired
      ↓
Historical Source Unavailable
```

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ProvenanceID
Original Source
Source Type
Creation Time
Derivation Chain
Transformations
Provider
Node
Input Observations
Output Observations
Quality
Integrity Evidence
Trust Context
Completeness
```

## Normative Anforderungen

1. NovaOS MUSS Observability-Daten mit Provenance-Informationen verknüpfen können.
2. Provenance MUSS von Trust, Authority, Audit und Integrity getrennt bleiben.
3. Provenance-Einträge SOLLEN über stabile `ProvenanceID`s identifizierbar sein.
4. Originale und abgeleitete Beobachtungen MÜSSEN unterscheidbar sein.
5. Aggregierte, geschätzte und vorhergesagte Daten MÜSSEN als solche erkennbar bleiben.
6. Relevante Transformationen SOLLEN ihre Eingaben referenzieren können.
7. Provenance-Ketten SOLLEN über System- und Node-Grenzen erhalten bleiben.
8. Decision Records SOLLEN verwendete Beobachtungen über Provenance referenzieren können.
9. State-Graph-Zustände SOLLEN ihre Beobachtungsherkunft angeben können.
10. Bekannte Provenance DARF NICHT automatisch als Trust interpretiert werden.
11. Integritätsnachweise DÜRFEN NICHT als Beweis für inhaltliche Richtigkeit interpretiert werden.
12. Unvollständige Provenance MUSS explizit darstellbar sein.
13. Fehlende Provenance DARF NICHT automatisch als ungültige Beobachtung behandelt werden.
14. Zugriff auf sensible Provenance MUSS capabilitybasiert kontrolliert werden.
15. Secrets und Capability Tokens DÜRFEN NICHT in Provenance-Daten gespeichert werden.
16. Privacy-, Retention-, Security- und Sovereignty-Regeln MÜSSEN erhalten bleiben.
17. Provenance-Historien MÜSSEN ressourcenbegrenzt werden können.
18. Provenance-Ketten MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-OBSERVABILITY-LOGGING-0001`
- `NPSPEC-OBSERVABILITY-METRICS-0001`
- `NPSPEC-OBSERVABILITY-TRACING-0001`
- `NPSPEC-OBSERVABILITY-DISTRIBUTEDTRACE-0001`
- `NPSPEC-OBSERVABILITY-PROFILING-0001`
- `NPSPEC-OBSERVABILITY-RESOURCE-0001`
- `NPSPEC-OBSERVABILITY-DECISION-0001`
- `NPSPEC-OBSERVABILITY-STATEGRAPH-0001`
- `NPSPEC-OBSERVABILITY-INTROSPECTION-0001`
- `NPSPEC-OBJECT-PROVENANCE-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-PRIVACY-RETENTION-0001`
- `ADR-ARCH-0079`

## Ergebnis

```text
Observation
     ↓
Source Identification
     ↓
Provenance Chain
     ↓
Transformation History
     ↓
Derived Observation
     ↓
Decision / State / Analysis
     ↓
Traceable Evidence Chain
```

NovaOS erhält damit eine systemweite Herkunfts- und Ableitungskette für Observability-Daten. Dadurch bleibt nachvollziehbar, wo Informationen entstanden sind, wie sie verarbeitet wurden und welche Beobachtungen auf welchen Quellen beruhen, ohne Provenance mit Trust, Authority oder inhaltlicher Wahrheit gleichzusetzen.