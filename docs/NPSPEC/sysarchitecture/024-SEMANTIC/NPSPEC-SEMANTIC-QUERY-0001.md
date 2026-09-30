# NPSPEC-SEMANTIC-QUERY-0001 – Nova Semantic Query

## Status

Angenommen

## Kategorie

Semantic / Query / Discovery

## Zweck

NovaOS definiert eine systemweite semantische Abfrageschicht, mit der Objekte, Ressourcen, Capabilities und Beziehungen anhand ihrer Bedeutung und Eigenschaften gefunden werden können.

```text
Semantic Query
      ↓
Query Engine
      ↓
Semantic Index / Graph
      ↓
Authorization Filter
      ↓
Results
```

Abfragen sollen nicht auf Dateipfade, konkrete Anwendungen oder physische Speicherorte beschränkt sein.

## Grundprinzipien

```text
Query ≠ Authority
Search Result ≠ Access
Known ObjectID ≠ Permission
Metadata Match ≠ Payload Access
Relationship Visibility ≠ Target Access
Query Language ≠ Storage Layout
Query Result ≠ Trusted Result
```

## Query-Modell

Eine semantische Anfrage kann enthalten:

```text
SemanticQuery
├── Target Type
├── Semantic Type
├── Conditions
├── Relationships
└── Result Projection
```

Optional:

```text
Metadata Conditions
Resource Requirements
Capability Requirements
Time Range
Provenance Conditions
Trust Requirements
Location Constraints
Sovereignty Constraints
Ordering
Result Limit
```

## Typabfragen

Objekte können anhand ihrer semantischen Typen gesucht werden.

```text
FIND
    Type = Nova.Document.Text
```

Subtypen können berücksichtigt werden, wenn dies ausdrücklich angefordert wird.

## Metadatenabfragen

Semantic Metadata kann als Suchkriterium verwendet werden.

```text
FIND
    Type = Nova.Image.Raster
    Metadata.CaptureTime > 2026-01-01
```

Die Query Engine muss Datentypen und Einheiten korrekt berücksichtigen.

## Relationship Queries

Der semantische Relationship Graph kann abgefragt werden.

```text
FIND Documents
WHERE
    PartOf → Project X
```

oder:

```text
Project X
    ↓ Contains
    ?
```

Graph-Traversierung muss die definierten Eigenschaften der Relationship Types beachten.

## Resource Queries

Semantische Ressourcen können anhand benötigter Eigenschaften gesucht werden.

```text
FIND Resource
WHERE
    Type = Compute.GPU
    Memory >= 2 GiB
    Location = Local
```

Resource Discovery und Resource Query können dadurch dieselbe semantische Grundlage verwenden.

## Capability Queries

Capabilities können anhand ihrer Semantik gesucht werden.

```text
FIND Capability
WHERE
    Input = Nova.Document.Text
    Operation = Render
    Output = Nova.Image.Raster
```

Das Ergebnis beschreibt mögliche Capability-Typen oder Provider.

Es erzeugt keine Autorität.

## Query Composition

Abfragen können mehrere Bedingungen kombinieren.

```text
Type
+
Metadata
+
Relationship
+
Resource
+
Trust
+
Sovereignty
```

Beispiel:

```text
FIND Nova.Document.Text

WHERE
    CreatedAt > 2026-01-01
    AND PartOf → Project X
    AND TrustState = Trusted
```

## Autorisierungsfilter

Security- und Privacy-Prüfungen sind Teil der Abfrageausführung.

```text
Raw Matches
     ↓
Capability Check
     ↓
Privacy Policy
     ↓
Security Policy
     ↓
Visible Results
```

Nicht autorisierte Objekte dürfen nicht durch:

```text
Result Count
Metadata
Relationships
Timing
Error Messages
```

unnötig offengelegt werden.

## Query Planning

Die logische Query bleibt unabhängig von der physischen Speicherung.

```text
Semantic Query
      ↓
Query Planner
      ↓
Metadata Index
Relationship Graph
Storage Index
Resource Registry
Capability Registry
```

NovaOS kann geeignete Datenquellen und Indizes automatisch auswählen.

## Location Transparency

Abfragen können lokale und entfernte Quellen umfassen.

```text
Semantic Query
├── Local Objects
├── Local Resources
├── Remote Resources
└── Distributed Providers
```

Dabei gilt:

```text
Transparent Query ≠ Transparent Authority
```

Trust-, Sovereignty- und Security-Anforderungen bleiben verbindlich.

## Konsistenz

Query-Ergebnisse müssen ihren Konsistenzzustand beschreiben können.

Beispiele:

```text
Current
Snapshot
PossiblyStale
Incomplete
```

Ein unvollständiges Ergebnis darf nicht still als vollständiges Ergebnis dargestellt werden.

## ExecutionContract

Komplexe Queries können über einen ExecutionContract gesteuert werden.

```text
Query
 +
ExecutionContract
 ↓
Resource Budget
Deadline
Determinism
Trust
Sovereignty
```

Dadurch können Query-Ausführung und Ressourcenverbrauch kontrolliert werden.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
QueryID
Query Plan
Semantic Conditions
Used Indexes
Result Count
Consistency State
Execution Time
Resource Usage
Security Filters
```

Sensible Query-Inhalte oder nicht autorisierte Treffer dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaOS MUSS systemweite semantische Queries unterstützen können.
2. Queries MÜSSEN unabhängig von konkreten Speicherpfaden formulierbar sein.
3. Semantic Types, Metadata und Relationships MÜSSEN als Query-Kriterien nutzbar sein.
4. Semantic Resources und Capabilities SOLLEN semantisch abfragbar sein.
5. Query-Ergebnisse DÜRFEN keine Autorität erzeugen.
6. Security- und Privacy-Regeln MÜSSEN während der Query-Ausführung berücksichtigt werden.
7. Nicht autorisierte Objekte DÜRFEN nicht unnötig über Seiteneffekte offengelegt werden.
8. Query Planning MUSS von der logischen Query getrennt bleiben.
9. Unvollständige oder veraltete Ergebnisse MÜSSEN erkennbar sein.
10. Query-Ausführung MUSS introspektierbar und ressourcenkontrollierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-FILE-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-CAPABILITY-REGISTRY-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0015`

## Ergebnis

```text
Semantic Intent
      ↓
Semantic Query
      ↓
Types + Metadata + Relationships
      ↓
Query Planning
      ↓
Authorized Semantic Sources
      ↓
Filtered Results
```

NovaOS erhält damit eine einheitliche semantische Abfrageschicht, über die Daten, Ressourcen und Fähigkeiten nach ihrer tatsächlichen Bedeutung gefunden werden können, ohne Anwendungen an konkrete Dateipfade, Speicherstrukturen oder Provider koppeln zu müssen.