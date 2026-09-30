# NPSPEC-SEMANTIC-METADATA-0001 – Nova Semantic Metadata

## Status

Angenommen

## Kategorie

Semantic / Metadata / Data Model

## Zweck

NovaOS definiert ein systemweites Modell für semantische Metadaten, mit dem Eigenschaften, Bedeutung, Beziehungen und Kontext von Objekten maschinenlesbar beschrieben werden.

```text
Object
  ↓
Semantic Metadata
  ↓
Meaning + Properties + Relations
  ↓
Discovery / Policy / Processing
```

Metadaten ergänzen ein Objekt, ohne dessen Identität, Inhalt oder Autorität zu ersetzen.

## Grundprinzipien

```text
Metadata ≠ Payload
Metadata ≠ Object Identity
Metadata ≠ Semantic Type
Metadata ≠ Authority
Metadata Visibility ≠ Payload Access
Metadata Claim ≠ Verified Fact
Metadata Relationship ≠ Permission
```

## Metadatenmodell

Ein semantischer Metadateneintrag besitzt mindestens:

```text
SemanticMetadata
├── MetadataTypeID
├── Value
└── ValueType
```

Optional:

```text
Schema Version
Source
Timestamp
Provenance
Confidence
Trust State
Privacy Label
Security Label
Language
Unit
Scope
```

## MetadataTypeID

Metadatenfelder werden über stabile semantische Identitäten beschrieben.

Beispiele:

```text
Nova.Metadata.Title
Nova.Metadata.Author
Nova.Metadata.CreatedAt
Nova.Metadata.Language
Nova.Metadata.Duration
Nova.Metadata.Resolution
Nova.Metadata.Location
Nova.Metadata.Relationship
```

Die Bedeutung darf nicht ausschließlich von einem frei gewählten Feldnamen abhängen.

## Typisierte Werte

Metadatenwerte sollen explizit typisiert sein.

```text
Duration    → Time.Duration
Width       → Length.Pixel
CreatedAt   → Time.Timestamp
Author      → Identity.Reference
Location    → Geo.Location
```

Dadurch können Komponenten Werte korrekt interpretieren, vergleichen und verarbeiten.

## Semantic-Type-Integration

Semantic Types können definieren, welche Metadaten unterstützt oder benötigt werden.

```text
Nova.Image.Raster
├── Width
├── Height
├── ColorSpace
└── CaptureTime
```

Unbekannte optionale Metadaten sollen erhalten werden können.

Unbekannte erforderliche Metadaten dürfen nicht still ignoriert werden.

## Beziehungen

Metadaten können semantische Beziehungen zwischen Objekten beschreiben.

```text
Object A
├── DerivedFrom → Object B
├── References → Object C
├── Contains → Object D
└── RelatedTo → Object E
```

Beziehungen sollen stabile `ObjectID`s verwenden.

## Provenance

Metadaten können Herkunftsinformationen besitzen.

```text
Metadata Value
     ↓
Source
     ↓
Transformation
     ↓
Current Value
```

Dadurch kann NovaOS zwischen ursprünglichen, importierten, berechneten und vom Benutzer gesetzten Metadaten unterscheiden.

## Vertrauenswürdigkeit

Ein vorhandener Metadatenwert gilt nicht automatisch als wahr.

```text
Metadata
   +
Provenance
   +
Signature / Trust Evidence
   ↓
Trust Evaluation
```

NovaOS muss Metadatenwert und Vertrauensbewertung getrennt behandeln.

## Veränderung

Änderungen an Metadaten sollen kontrolliert erfolgen.

```text
Read
 ↓
Modify
 ↓
Validate
 ↓
Transaction
 ↓
Commit
```

Bei versionierten Objekten kann eine Metadatenänderung eine neue Objektversion erzeugen.

## NovaFile Integration

NovaFile kann Semantic Metadata direkt im Container speichern.

```text
NovaFile
├── ObjectID
├── Payload
├── SemanticTypeID
├── Metadata
├── Provenance
└── Relationships
```

Metadaten können abhängig von ihrer Sensitivität getrennt geschützt werden.

## Datenschutz

Metadaten können genauso sensibel sein wie Payload-Daten.

Beispiele:

```text
Location
Author
Device Identity
Creation Time
Relationships
Usage Information
```

Deshalb müssen Metadaten Privacy Labels, Retention und Selective Disclosure unterstützen können.

## Sicherheit

Metadatenzugriff wird über Capabilities kontrolliert.

```text
MetadataRead
MetadataWrite
```

Dabei gilt:

```text
PayloadRead ≠ MetadataRead
MetadataRead ≠ MetadataWrite
```

NovaOS kann diese Rechte getrennt vergeben.

## Discovery und Suche

Autorisierte semantische Metadaten können für Suche und Discovery verwendet werden.

```text
Query
  ↓
Semantic Metadata Index
  ↓
Matching ObjectIDs
```

Ein Suchindex darf keine Metadaten oder Objekte offenlegen, auf die der Requester keine entsprechende Autorität besitzt.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
MetadataTypeID
ValueType
Schema Version
Source
Provenance
Trust State
Privacy Label
Security Label
Relationships
```

## Normative Anforderungen

1. NovaOS MUSS semantisch typisierte Metadaten unterstützen.
2. Metadatentypen SOLLEN stabile `MetadataTypeID`s besitzen.
3. Metadatenwerte SOLLEN explizit typisiert sein.
4. Semantic Types SOLLEN unterstützte Metadaten definieren können.
5. Metadaten und Payload MÜSSEN getrennt behandelbar sein.
6. Metadaten DÜRFEN keine Autorität erzeugen.
7. Herkunft und Vertrauenswürdigkeit von Metadaten SOLLEN nachvollziehbar sein.
8. Sensible Metadaten MÜSSEN Privacy- und Security-Regeln unterliegen.
9. MetadataRead und MetadataWrite MÜSSEN getrennt autorisierbar sein.
10. Semantische Metadaten MÜSSEN versionierbar und introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-FILE-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-STORAGE-NOVAFILE-SECURITY-0001`
- `NPSPEC-PRIVACY-LABEL-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0013`

## Ergebnis

```text
Object
  ↓
Typed Semantic Metadata
  ↓
Meaning + Context + Relationships
  ↓
Protected Discovery
  ↓
Semantic Processing
```

NovaOS erhält damit ein einheitliches Metadatenmodell, in dem Informationen über Objekte systemweit verständlich, typisiert, nachvollziehbar und geschützt genutzt werden können, ohne Metadaten mit Inhalt, Identität oder Autorität gleichzusetzen.