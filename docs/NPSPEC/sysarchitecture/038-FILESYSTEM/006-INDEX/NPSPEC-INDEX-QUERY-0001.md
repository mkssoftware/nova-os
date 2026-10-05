# NPSPEC-INDEX-QUERY-0001 – Nova Index Query

## Status

Angenommen

## Kategorie

Index / Query

## Zweck

NovaOS definiert ein einheitliches Abfragemodell für systemweite Indizes.

Index Queries ermöglichen kombinierte Abfragen über Namen, Metadaten, semantische Typen, Beziehungen, Namespaces und Workspaces, ohne Anwendungen an die interne Struktur einzelner Indizes zu koppeln.

## Grundprinzipien

```text
Query ≠ Authority
Query Result ≠ Object Access
Query ≠ Physical Index Layout
Result ObjectID ≠ Object Copy
Index ≠ Source of Truth
```

## Query-Modell

Eine Abfrage kann enthalten:

```text
IndexQuery
├── Scope
├── Conditions
├── Sort
├── Limit
├── Offset / Cursor
└── Projection
```

Bedingungen dürfen logisch kombiniert werden:

```text
AND
OR
NOT
```

## Abfragefelder

Queries können unter anderem verwenden:

```text
Name
ObjectID
SemanticTypeID
Metadata
RelationTypeID
Namespace
WorkspaceID
ProgramID
SolutionID
Content
```

Beispiel:

```text
SemanticType = Image
AND
Metadata.Year = 2026
AND
WorkspaceID = <Workspace>
```

## Ausführung

```text
Query
  ↓
Parse / Validate
  ↓
Query Planner
  ↓
Relevant Indexes
  ↓
Candidate ObjectIDs
  ↓
Permission Filter
  ↓
Result
```

Der Query Planner darf mehrere Indizes kombinieren.

## Ergebnisse

Ergebnisse sollen primär stabile Identitäten liefern:

```text
QueryResult
├── ObjectID
├── Match Information
├── Requested Metadata
└── Result State
```

Pfade oder Anzeigenamen können ergänzend bereitgestellt werden, sind jedoch nicht die Objektidentität.

## Pagination und Streaming

Große Ergebnismengen müssen schrittweise verarbeitet werden können.

NovaOS darf dafür:

```text
Cursor
Pagination
Streaming
Incremental Results
```

verwenden.

Eine Abfrage darf nicht voraussetzen, dass sämtliche Ergebnisse gleichzeitig im Speicher gehalten werden.

## Konsistenz

Queries können auf einer definierten Indexgeneration ausgeführt werden.

```text
Query Start
    ↓
Generation N
    ↓
Query Execution
    ↓
Result
```

Ändert sich der Index während einer laufenden Abfrage, muss die verwendete Konsistenzstrategie eindeutig sein.

## Sicherheit

Der Query Layer muss Ergebnisse anhand des aktuellen Sicherheitskontexts filtern.

```text
Candidate
   ↓
Visibility / Permission Check
   ↓
Result
```

Eine Query darf weder geschützte Objekte noch sensible Metadaten allein aufgrund ihrer Indexierung offenlegen.

## Resource Economy

Abfragen müssen begrenzbar sein durch:

```text
Result Limit
Execution Time
Memory Budget
Traversal Depth
CPU Budget
```

Unbegrenzte oder übermäßig teure Queries müssen kontrolliert eingeschränkt oder abgebrochen werden können.

## Normative Anforderungen

1. NovaOS MUSS ein einheitliches Index-Query-Modell bereitstellen.
2. Queries MÜSSEN mehrere Bedingungen kombinieren können.
3. Mehrere Indexarten MÜSSEN innerhalb einer Query kombinierbar sein.
4. Ergebnisse SOLLEN stabile `ObjectID`s verwenden.
5. Queries DÜRFEN nicht von der physischen Indexstruktur abhängig sein.
6. Große Ergebnismengen MÜSSEN inkrementell verarbeitet werden können.
7. Die verwendete Indexgeneration MUSS bestimmbar sein.
8. Query-Ergebnisse MÜSSEN den aktuellen Sicherheitskontext berücksichtigen.
9. Query-Ausführung DARF keine zusätzliche Authority erzeugen.
10. Ressourcenverbrauch von Queries MUSS begrenzbar sein.
11. Graph-Traversierungen MÜSSEN gegen unbegrenzte Tiefe geschützt sein.
12. Fehler und Abbruchgründe MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-INDEX-SEARCH-0001`
- `NPSPEC-INDEX-METADATA-0001`
- `NPSPEC-INDEX-NAMESPACE-0001`
- `NPSPEC-INDEX-RELATION-0001`
- `NPSPEC-INDEX-WORKSPACE-0001`
- `NPSPEC-INDEX-INTEGRITY-0001`
- `NPSPEC-INDEX-TRANSACTION-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`

## Ergebnis

NovaOS erhält eine einheitliche Query-Schicht über seine unterschiedlichen Indizes. Anwendungen, Solutions und Systemdienste können komplexe Such-, Metadaten-, Namespace-, Relations- und Workspace-Abfragen ausführen, ohne die interne Indexstruktur kennen zu müssen oder dadurch zusätzliche Authority zu erhalten.