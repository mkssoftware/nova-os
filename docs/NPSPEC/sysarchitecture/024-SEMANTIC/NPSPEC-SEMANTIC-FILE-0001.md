# NPSPEC-SEMANTIC-FILE-0001 – Nova Semantic File Model

## Status

Angenommen

## Kategorie

Semantic / Storage / File Model

## Zweck

NovaOS behandelt Dateien nicht ausschließlich als Bytefolgen mit Dateinamen und Endungen, sondern als semantisch beschriebene Datenobjekte.

```text
File
 ↓
Object Identity
 ↓
Semantic Type
 ↓
Representation
 ↓
Capabilities / Operations
```

Dadurch kann NovaOS verstehen, **was eine Datei darstellt**, unabhängig davon, welche Anwendung sie erzeugt hat.

## Grundprinzipien

```text
File ≠ Filename
File ≠ Path
File ≠ Extension
File ≠ Application
File Identity ≠ File Location
Semantic Type ≠ Storage Format
File Visibility ≠ File Authority
```

## Semantic File

Eine semantische Datei besitzt mindestens:

```text
SemanticFile
├── ObjectID
├── SemanticTypeID
├── Representation
├── Payload
└── Metadata
```

Optional:

```text
ContentID
VersionID
Provenance
Relationships
Security Labels
Privacy Labels
Integrity Information
Capabilities
```

`ObjectID` identifiziert das logische Objekt.

`ContentID` kann eine konkrete Payload-Version identifizieren.

## Dateiname und Pfad

Dateiname und Pfad sind Projektionen auf das Objekt.

```text
ObjectID
├── Benutzer/Dokumente/Bericht.md
├── Suchergebnis
├── Projektansicht
└── Semantische Sammlung
```

Das Verschieben oder Umbenennen einer Datei verändert ihre logische Identität nicht.

```text
Identity ≠ Location
```

## Dateiendung

Die Dateiendung beschreibt primär die Repräsentation.

Beispiel:

```text
bericht.md
```

kann besitzen:

```text
SemanticType:
Nova.Document.Text.Markdown

Representation:
text/markdown
```

NovaOS darf sich nicht ausschließlich auf die Dateiendung verlassen.

## Mehrere Repräsentationen

Ein semantisches Objekt kann unterschiedliche Repräsentationen besitzen.

```text
Nova.Document.Text
├── Markdown
├── HTML
├── PDF
└── Plain Text
```

Eine Transformation zwischen Repräsentationen erfolgt über geeignete Capabilities.

## Capability Discovery

NovaOS kann anhand des Semantic Type passende Fähigkeiten suchen.

```text
Semantic File
      ↓
SemanticTypeID
      ↓
Requested Operation
      ↓
Capability Discovery
      ↓
Compatible Provider
```

Beispiel:

```text
Nova.Image.Raster
        +
Edit
        ↓
Image Editing Capability
```

Dadurch muss keine feste Dateizuordnung zu einer einzelnen Anwendung existieren.

## Operation statt Anwendung

Der Benutzer kann primär eine gewünschte Aktion auswählen.

```text
Document
├── Öffnen
├── Bearbeiten
├── Drucken
├── Konvertieren
├── Teilen
└── Analysieren
```

NovaOS bestimmt anschließend eine geeignete Capability und deren Provider.

## NovaFile Integration

`NovaFile (.nf)` ist die native Container-Repräsentation des Semantic File Models.

```text
bild.jpg.nf
├── ObjectID
├── Payload
├── SemanticTypeID
├── Representation: JPEG
├── Metadata
├── Provenance
└── Relationships
```

Das Semantic File Model ist jedoch die logische Ebene.

```text
Semantic File = Logical Object
NovaFile      = Native Container Representation
```

Damit bleibt das semantische Modell grundsätzlich unabhängig vom konkreten Speichercontainer.

## Beziehungen

Semantic Files können explizite Beziehungen besitzen.

```text
Document A
├── DerivedFrom → Document B
├── Contains → Image C
├── References → Dataset D
└── RelatedTo → Project E
```

Diese Beziehungen verwenden stabile Objektidentitäten statt ausschließlich Pfade.

## Sicherheit

Semantische Informationen erzeugen keine Autorität.

```text
Known ObjectID ≠ Access
Known SemanticTypeID ≠ Access
Relationship ≠ Permission
Metadata Visibility ≠ Payload Access
```

Zugriffe werden weiterhin über Capabilities kontrolliert.

## Versionierung

Änderungen können neue Versionen erzeugen.

```text
ObjectID
├── Version 1
├── Version 2
└── Version 3
```

Die logische Objektidentität kann dabei erhalten bleiben.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ObjectID
SemanticTypeID
Representation
VersionID
ContentID
Metadata
Relationships
Provenance
Available Operations
Compatible Capabilities
```

## Normative Anforderungen

1. NovaOS MUSS Dateien als semantische Objekte beschreiben können.
2. Dateiidentität MUSS von Pfad und Dateiname getrennt sein.
3. Semantic Type und physische Repräsentation MÜSSEN getrennt modelliert werden.
4. Dateiendungen DÜRFEN NICHT die einzige Quelle für Typinformationen sein.
5. Semantic Files SOLLEN über stabile `ObjectID`s referenziert werden.
6. Operationen SOLLEN anhand von Semantic Types passenden Capabilities zugeordnet werden können.
7. Semantic Files MÜSSEN Beziehungen zu anderen Objekten ausdrücken können.
8. NovaFile SOLL als native Container-Repräsentation des Modells dienen.
9. Semantische Metadaten DÜRFEN keine Zugriffsautorität erzeugen.
10. Semantic Files MÜSSEN introspektierbar und versionierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-SEMANTIC-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-STORAGE-NOVAFILE-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-PROJECTION-0001`
- `NPSPEC-STORAGE-NOVAFILE-SECURITY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0010`

## Ergebnis

```text
Semantic File
      ↓
Stable Object Identity
      ↓
Semantic Meaning
      ↓
Requested Operation
      ↓
Capability Discovery
      ↓
Provider
```

NovaOS erhält damit ein Dateimodell, bei dem Dateien nicht länger primär über Pfad, Endung oder zugeordnete Anwendung definiert werden, sondern als stabile semantische Objekte, auf denen passende Systemfähigkeiten dynamisch arbeiten können.