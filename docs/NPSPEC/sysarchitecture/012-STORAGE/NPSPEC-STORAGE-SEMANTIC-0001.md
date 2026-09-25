# NPSPEC-STORAGE-SEMANTIC-0001 – Nova Semantic Storage

## Status

Angenommen

## Kategorie

Storage / Semantic Types / Metadata / Object Model

## Zweck

NovaOS definiert Semantic Storage als Erweiterung des Storage-Modells, bei der gespeicherte Objekte neben ihrem technischen Format eine maschinenlesbare semantische Bedeutung besitzen können.

```text
Storage Object
├── ObjectID
├── Semantic Type
├── Payload
├── Metadata
└── Relationships
```

Dadurch können System und Capabilities Daten anhand ihrer Bedeutung statt ausschließlich anhand von Dateiendungen oder Pfaden verarbeiten.

## Grundprinzip

```text
File Extension ≠ Semantic Type
Format ≠ Meaning
Path ≠ Classification
```

Ein Objekt kann beispielsweise als:

```text
Image
Document
Contact
Calendar Event
Source Code
Simulation Model
Circuit
Dataset
```

klassifiziert werden, unabhängig davon, wie sein Payload technisch gespeichert ist.

## Semantic Type

Ein Storage-Objekt darf einen oder mehrere definierte semantische Typen besitzen.

```text
ObjectID
   ↓
Semantic Type
   ↓
Compatible Capabilities
```

Semantische Typen müssen stabil identifizierbar und versionierbar sein.

## Format und Semantik

Technisches Datenformat und semantische Bedeutung werden getrennt modelliert.

```text
Semantic Type: Image
Format: PNG

Semantic Type: Image
Format: JPEG
```

Mehrere Formate können dieselbe semantische Bedeutung repräsentieren.

## Metadaten

Semantische Metadaten dürfen direkt mit dem Objekt verbunden werden.

Beispiele:

```text
Title
Author
Dimensions
Duration
Language
Tags
Relationships
Domain-specific Properties
```

Metadaten sollen nach Möglichkeit unabhängig vom sichtbaren Dateinamen verfügbar sein.

## Capability Discovery

Semantic Types können zur Auswahl geeigneter Systemfähigkeiten verwendet werden.

```text
Semantic Type
      ↓
Capability Registry
      ↓
Compatible Providers
```

Beispiel:

```text
Image
├→ View
├→ Edit
├→ Convert
└→ Analyze
```

Die Auswahl einer Capability bleibt von konkreten Anwendungen oder Providern getrennt.

## Beziehungen

Semantische Beziehungen dürfen zwischen Storage-Objekten bestehen.

```text
Document
   ├→ contains → Image
   ├→ references → Dataset
   └→ created-by → Project
```

Beziehungen sollen stabile `ObjectID`s verwenden.

## Erkennung

Der semantische Typ darf aus verschiedenen Quellen stammen:

```text
Explicit Metadata
Trusted Import
Format Inspection
User Selection
Capability Output
```

Eine Dateiendung allein darf nicht als vertrauenswürdiger Nachweis des tatsächlichen Inhalts gelten.

## Konvertierung

Unterschiedliche Formate desselben Semantic Types dürfen über Capabilities konvertiert werden.

```text
Image / PNG
     ↓
Convert
     ↓
Image / JPEG
```

Die semantische Bedeutung soll dabei erhalten bleiben, sofern die Konvertierung dies ermöglicht.

## Suche und Discovery

Semantic Storage soll Abfragen nach Bedeutung ermöglichen.

```text
Find:
Type = Image
Tag = NovaOS
Created > Date
```

Dadurch müssen Daten nicht ausschließlich über bekannte Pfade oder Dateinamen gefunden werden.

## Legacy-Dateien

Normale Dateien bleiben vollständig unterstützt.

```text
Legacy File
    ↓
VFS
    ↓
Optional Semantic Classification
```

Fehlende semantische Metadaten dürfen die grundlegende Dateinutzung nicht verhindern.

## Sicherheit

Semantische Klassifikation erzeugt keine Zugriffsberechtigung.

```text
Semantic Type
      ≠
Capability
```

Metadaten und Beziehungen müssen denselben Security- und Privacy-Regeln wie das zugehörige Objekt unterliegen.

## Introspection

NovaOS soll mindestens bereitstellen können:

```text
ObjectID
Semantic Type
Format
Metadata
Relationships
Compatible Capabilities
Classification Source
```

## Normative Anforderungen

1. NovaOS MUSS semantische Typen unabhängig von Dateiendungen darstellen können.
2. Semantic Type und technisches Datenformat MÜSSEN logisch getrennt sein.
3. Semantische Typen MÜSSEN stabil identifizierbar und versionierbar sein.
4. Ein Objekt DARF mehrere kompatible semantische Typinformationen besitzen.
5. Semantische Beziehungen SOLLEN stabile `ObjectID`s verwenden.
6. Semantic Types SOLLEN zur Capability Discovery verwendet werden können.
7. Eine Dateiendung DARF NICHT allein als vertrauenswürdige Inhaltsklassifikation gelten.
8. Legacy-Dateien MÜSSEN auch ohne semantische Metadaten verwendbar bleiben.
9. Semantische Klassifikation DARF KEINE zusätzlichen Zugriffsrechte erzeugen.
10. Semantische Metadaten MÜSSEN die Security- und Privacy-Regeln des Systems beachten.
11. Semantische Suche SOLL unabhängig von konkreten Speicherpfaden möglich sein.
12. Typ, Format, Beziehungen und Klassifikationsquelle SOLLEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-STORAGE-0006`

## Ergebnis

```text
Storage Object
      ↓
Semantic Type + Metadata
      ↓
Capability Discovery
      ↓
View / Edit / Convert / Process
```

NovaOS erhält damit eine semantische Storage-Schicht, bei der Daten nicht nur nach Speicherort und Format, sondern nach ihrer tatsächlichen Bedeutung organisiert und verarbeitet werden können.