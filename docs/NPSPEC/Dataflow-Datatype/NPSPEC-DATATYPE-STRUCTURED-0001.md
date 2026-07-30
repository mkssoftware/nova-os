# NPSPEC-DATATYPE-STRUCTURED-0001: Tabellen-, Datenbank- und strukturierte Datentypen

- **Dokumenttyp:** Nova Platform Specification (NPSPEC)
- **Version:** 1.0
- **Status:** Verbindlich
- **Kategorie:** Core Architecture
- **Erstellt:** 2026-07-28
- **Gültig für:** Gesamte Nova Platform
- **Referenzen:**
  - NPSPEC-DATATYPE-0001
  - NPSPEC-DATATYPE-PRIMITIVE-0001
  - NPSPEC-DATATYPE-SEMANTIC-0001
  - NPSPEC-DATATYPE-COMPOSITE-0001
  - NPSPEC-DATAFLOW-0001
  - NPSPEC-CAPABILITY-CONTRACT-0001
  - NPSPEC-CAPABILITY-DISCOVERY-0001
  - NPSPEC-CAPABILITY-REGISTRY-0001
  - ADR-DATAFLOW-0002
  - ADR-DATAFLOW-0003
  - ADR-DATAFLOW-0005
  - ADR-DATAFLOW-0006

---

# 1. Zweck

Diese Spezifikation definiert die strukturierten Datentypen der Nova Platform.

Sie beschreibt ein universelles Datenmodell für:

- Tabellen
- relationale Daten
- Dokumentdatenbanken
- Key-Value-Daten
- Graphdaten
- Zeitreihen
- DataFrames
- analytische Daten
- multidimensionale Daten
- Datenbankobjekte
- Schemata
- strukturierte Datensätze

Das Modell dient als gemeinsame Grundlage für sämtliche Speicher-, Analyse- und Datenverarbeitungssysteme innerhalb der Nova Platform.

---

# 2. Ziele

Das Modell verfolgt folgende Ziele:

- universelles Datenbankschema
- datenbankunabhängige Verarbeitung
- starke Typisierung
- semantische Datenmodelle
- Schemaevolution
- effiziente Analysen
- Streaming-Unterstützung
- KI-Integration
- verteilte Speicherung
- vollständige Reflection
- plattformunabhängige Serialisierung

Strukturierte Datentypen sollen sowohl relationale als auch dokumentenorientierte, Graph- und andere Datenmodelle unter einer gemeinsamen Metamodellierung abbilden können.

---

# 3. Grundprinzipien

Die Nova Structured Data Architecture basiert auf folgenden Prinzipien:

- Daten besitzen immer einen Typ.
- Jedes Schema ist versionierbar.
- Daten und Schema sind getrennt.
- Struktur besitzt eine eindeutige Identität.
- Änderungen erzeugen neue Versionen.
- Beziehungen sind explizit modelliert.
- Constraints sind deklarativ.
- Daten sind semantisch beschrieben.
- Referenzen ersetzen Kopien.
- Strukturen sind maschinenlesbar.

---

# 4. Architekturübersicht

```text
Structured Data
├── Schema
├── Entity Types
├── Records
├── Relations
├── Constraints
├── Indexes
├── Views
├── Metadata
├── Provenance
└── Version
```

---

# 5. Strukturierte Datentypen

Nova definiert folgende Haupttypen:

```text
Table

Record

Entity

Row

Column

DataFrame

Dataset

Relation

View

Schema

Index

Collection

Graph

TimeSeries

KeyValueStore
```

Alle basieren auf dem universellen Nova-Datentypmodell.

---

# 6. Schema

Ein Schema beschreibt den logischen Aufbau strukturierter Daten.

Es enthält:

```text
Schema ID

Version

Entities

Fields

Relations

Indexes

Constraints

Metadata
```

Schema und Daten sind voneinander getrennt.

---

# 7. Entity

Eine Entity beschreibt einen fachlichen Datentyp.

Beispiele:

```text
Person

Order

Invoice

Building

Robot

Sensor

Measurement
```

Entities besitzen:

- Entity ID
- Name
- Version
- Felder
- Beziehungen

---

# 8. Record

Ein Record repräsentiert genau eine Entity-Instanz.

Beispiel:

```text
Person

↓

Max Mustermann
```

Ein Record besitzt:

```text
Primary Identifier

Field Values

Version

Metadata
```

---

# 9. Tabellen

Eine Tabelle besteht aus:

```text
Columns

Rows

Constraints

Indexes

Metadata
```

Jede Zeile besitzt denselben logischen Schematyp.

---

# 10. Spalten

Eine Spalte besitzt:

```text
Column ID

Name

Datatype

Semantic Type

Nullable

Constraints

Default Value
```

Spalten sind eindeutig innerhalb einer Tabelle.

---

# 11. Zeilen

Eine Zeile repräsentiert genau einen Datensatz.

Eigenschaften:

- Row ID
- Werte
- Version
- Provenance
- Änderungszeit

---

# 12. Primärschlüssel

Jede Entity kann einen Primärschlüssel besitzen.

Beispiele:

```text
UUID

Integer

Composite Key
```

Der Primärschlüssel identifiziert eine Entity eindeutig.

---

# 13. Fremdschlüssel

Beziehungen werden über typisierte Referenzen beschrieben.

Beispiel:

```text
Order
↓
CustomerID
↓
Customer
```

Referenzen besitzen Typinformationen.

---

# 14. Beziehungen

Nova unterstützt:

```text
One-to-One

One-to-Many

Many-to-One

Many-to-Many

Composition

Aggregation

Reference
```

Alle Beziehungen besitzen definierte Semantik.

---

# 15. Collections

Collections enthalten mehrere strukturierte Objekte.

Beispiele:

```text
List<Record>

Set<Entity>

Collection<Document>
```

Collections besitzen:

- Elementtyp
- Constraints
- Version

---

# 16. DataFrames

Ein DataFrame repräsentiert tabellarische Analyseobjekte.

Eigenschaften:

```text
Columns

Rows

Column Types

Metadata

Statistics
```

DataFrames unterstützen analytische Verarbeitung.

---

# 17. Datasets

Ein Dataset beschreibt eine logisch zusammengehörige Datenmenge.

Beispiele:

```text
Training Dataset

Measurement Dataset

Financial Dataset

GIS Dataset
```

Datasets besitzen:

- Dataset ID
- Schema
- Provenance
- Version

---

# 18. Zeitreihen

Nova definiert spezialisierte TimeSeries-Typen.

Eigenschaften:

```text
Timestamp

Measurement

Unit

Source

Quality
```

Zeitreihen unterstützen:

- Windowing
- Aggregation
- Streaming
- Interpolation

---

# 19. Graphdaten

Graphen bestehen aus:

```text
Nodes

Edges

Properties

Labels
```

Eigenschaften:

- gerichtet
- ungerichtet
- gewichtet
- versioniert

---

# 20. Dokumentdaten

Dokumentorientierte Datenbanken werden als strukturierte Collections modelliert.

Ein Dokument besitzt:

```text
Object ID

Schema

Fields

Nested Objects

Arrays
```

Schemafreie Dokumente sind zulässig, müssen aber strukturell beschreibbar bleiben.

---

# 21. Key-Value-Daten

Key-Value-Strukturen besitzen:

```text
Key

Value

Version

Metadata
```

Keys besitzen:

- eindeutigen Typ
- Vergleichsregel
- Hashfunktion

---

# 22. Hierarchische Daten

Hierarchische Daten können modelliert werden als:

```text
Tree

Folder

XML

JSON

YAML
```

Hierarchien besitzen:

- Parent
- Children
- Level
- Path

---

# 23. Mehrdimensionale Daten

Nova unterstützt:

```text
Cube

Tensor

Matrix

ND Array
```

Dimensionen besitzen:

```text
Name

Size

Semantic Meaning
```

---

# 24. Constraints

Unterstützte Constraints:

```text
Primary Key

Foreign Key

Unique

Nullable

Range

Pattern

Check

Computed
```

Constraints werden deklarativ beschrieben.

---

# 25. Berechnete Felder

Ein Feld kann berechnet werden.

Beispiel:

```text
Price
×
Quantity
↓
Total
```

Berechnungen sind versioniert.

---

# 26. Views

Views beschreiben virtuelle Daten.

Beispiele:

```text
SQL View

Filtered View

Projection

Aggregated View
```

Views besitzen kein eigenes physisches Speicherlayout.

---

# 27. Materialisierte Views

Materialisierte Views speichern Ergebnisse dauerhaft.

Eigenschaften:

- Aktualisierungsstrategie
- Synchronisation
- Version

---

# 28. Indexe

Nova unterstützt:

```text
Hash

B-Tree

B+Tree

Bitmap

Spatial

Vector

Fulltext
```

Indexe besitzen eigene Metadaten.

---

# 29. Volltextindex

Textfelder können Volltextindizes besitzen.

Unterstützt werden:

```text
Token

Phrase

Language

Stemming

Ranking
```

---

# 30. Vektorindex

KI-Anwendungen können Vektorindizes verwenden.

Beispiele:

```text
Embedding Index

Similarity Search

ANN Index
```

Eigenschaften:

```text
Dimension

Metric

Normalization
```

---

# 31. Geodaten

Geodaten unterstützen:

```text
Point

Line

Polygon

Raster

Coordinate System
```

Alle Koordinaten besitzen ein Referenzsystem.

---

# 32. Statistik

Strukturierte Daten können Statistiken enthalten.

Beispiele:

```text
Minimum

Maximum

Average

Median

Variance

Histogram
```

Statistiken sind Metadaten.

---

# 33. Partitionierung

Datasets können partitioniert werden.

Strategien:

```text
Hash

Range

Time

Region

Custom
```

---

# 34. Replikation

Unterstützt werden:

```text
Full Replica

Partial Replica

Read Replica

Geo Replica
```

---

# 35. Schemaevolution

Erlaubte Änderungen:

```text
Add Field

Deprecate Field

Rename Field

Migration

Version Adapter
```

Inkompatible Änderungen benötigen Migrationen. Eine kontrollierte Schemaevolution ist insbesondere bei strukturierten Datenmodellen essenziell, um unterschiedliche Versionen sicher verwalten zu können.

---

# 36. Serialisierung

Unterstützt werden:

```text
Nova Binary

JSON

CBOR

Apache Arrow

Parquet

CSV

XML
```

Die logische Struktur bleibt unabhängig vom Format erhalten.

---

# 37. Reflection

Reflection liefert:

```text
Schema

Fields

Relations

Constraints

Indexes

Statistics

Metadata
```

---

# 38. Provenance

Jede strukturierte Datenmenge besitzt Provenance.

Erfasst werden:

```text
Origin

Importer

Transformations

Capabilities

Version

Timestamp
```

---

# 39. Sicherheit

Strukturierte Daten unterstützen:

```text
Access Rights

Classification

Encryption

Integrity

Audit
```

Sicherheitsinformationen sind Bestandteil des Datenmodells.

---

# 40. Datenschutz

Felder können klassifiziert werden.

Beispiele:

```text
PersonalData

SensitiveData

Public

Secret
```

Die Klassifikation beeinflusst:

- Speicherung
- Export
- Suche
- KI-Verarbeitung

---

# 41. Streaming

Strukturierte Daten unterstützen Streaming.

Beispiele:

```text
Append Stream

Change Stream

Snapshot Stream

Live Query
```

---

# 42. Validierung

Die Runtime prüft:

```text
Schema

Constraints

Relations

Types

Semantic Types

Version
```

Ungültige Daten dürfen nicht persistiert werden.

---

# 43. YAML-Beispiel

```yaml
table:
  id: nova.measurements
  version: 1.0
  columns:
    - id
    - timestamp
    - value
    - unit
  primaryKey:
    - id
  indexes:
    - timestamp
  constraints:
    value:
      minimum: 0
```

---

# 44. Validierungsregeln

Ein strukturierter Datentyp ist gültig, wenn:

- ein Schema existiert,
- alle Felder typisiert sind,
- Beziehungen gültig sind,
- Constraints erfüllt werden,
- Versionen eindeutig sind,
- Reflection unterstützt wird,
- Provenance vorhanden ist.

---

# 45. Qualitätsanforderungen

Das Modell muss:

- stark typisiert sein,
- semantisch beschreibbar sein,
- schemaevolutionsfähig sein,
- verteilte Speicherung unterstützen,
- Streaming unterstützen,
- KI-fähig sein,
- Zero-Copy ermöglichen,
- vollständig versionierbar bleiben.

---

# 46. Konformitätskriterien

Eine Nova-Implementierung erfüllt diese Spezifikation, wenn:

- Tabellen, Records und Entities unterstützt werden,
- Schema und Daten getrennt verwaltet werden,
- Beziehungen typisiert sind,
- Constraints validiert werden,
- Reflection verfügbar ist,
- Versionierung unterstützt wird,
- strukturierte Serialisierung bereitgestellt wird,
- Provenance erhalten bleibt,
- Sicherheits- und Datenschutzinformationen verarbeitet werden.

---

# 47. Zusammenfassung

Die **Tabellen-, Datenbank- und strukturierten Datentypen** definieren ein universelles Datenmodell für sämtliche strukturierten Informationen der Nova Platform.

Durch die Vereinheitlichung relationaler Tabellen, Dokumentdatenbanken, Graphen, Zeitreihen, DataFrames, Key-Value-Strukturen und analytischer Datenmodelle entsteht eine gemeinsame Architektur für Speicherung, Analyse und Datenverarbeitung.

Die Kombination aus starken Datentypen, deklarativen Constraints, semantischen Typen, Schemaevolution, Versionierung, Provenance, Reflection und plattformunabhängiger Serialisierung bildet die Grundlage für hochperformante, verteilte und KI-fähige Datenverarbeitung innerhalb der Nova Platform.
```
````
