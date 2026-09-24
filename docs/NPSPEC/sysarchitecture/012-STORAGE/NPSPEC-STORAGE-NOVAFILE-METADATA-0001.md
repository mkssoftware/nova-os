# NPSPEC-STORAGE-NOVAFILE-METADATA-0001 – NovaFile Metadata

## Status

Entwurf

## Kategorie

Storage / NovaFile / Metadata

## Zweck

NovaOS definiert das Metadatenmodell innerhalb eines NovaFile-Containers.

```text
NovaFile
├── Payload
└── Metadata
    ├── Core
    ├── Semantic
    ├── Provenance
    └── Relationships
```

Metadaten sollen gemeinsam mit dem Payload transportierbar bleiben, ohne Bestandteil des eigentlichen Nutzdatenformats zu werden.

## Grundprinzip

```text
Metadata ≠ Payload
Metadata ≠ ObjectID
Metadata ≠ Filename
```

Metadaten beschreiben ein Objekt und seinen Inhalt, während der ursprüngliche Payload unabhängig davon erhalten bleibt.

## Metadata-Bereich

NovaFile muss einen eindeutig identifizierbaren Metadata-Bereich unterstützen.

Ein Eintrag besteht logisch mindestens aus:

```text
FieldID
Type
Value
```

Optional:

```text
Schema
Version
Flags
Namespace
```

Die Struktur muss erweiterbar sein.

## Kernmetadaten

NovaFile darf grundlegende Objektinformationen enthalten:

```text
Name
Semantic Type
Content Type
Size
Timestamps
ContentID
Version Information
```

Die stabile `ObjectID` wird weiterhin durch das Storage-Objektmodell verwaltet und darf nicht ausschließlich aus diesen Metadaten erzeugt werden.

## Semantische Metadaten

NovaFile muss semantische Informationen aufnehmen können.

Beispiele:

```text
Semantic Type
Title
Tags
Language
Author
Domain Properties
```

Domänenspezifische Felder dürfen über eigene Metadata-Namespaces definiert werden.

## Beziehungen

Beziehungen zu anderen NovaOS-Objekten dürfen direkt als Metadaten gespeichert werden.

```text
Relationships
├── references → ObjectID
├── derived-from → ObjectID
└── related-to → ObjectID
```

Interne Beziehungen sollen stabile Objektidentitäten statt Pfade verwenden.

## Provenance

NovaFile darf Provenance-Informationen enthalten.

```text
CreatedBy
ImportedFrom
DerivedFrom
GeneratedBy
ModifiedBy
```

Versionsbezogene Provenance muss der entsprechenden Objektversion zugeordnet werden können.

## Erweiterbarkeit

Neue Metadata-Felder dürfen hinzugefügt werden, ohne ältere NovaFile-Implementierungen grundsätzlich inkompatibel zu machen.

```text
Known Field   → Process
Unknown Optional Field → Preserve / Skip
Unknown Required Field → Reject / Limited Access
```

Unbekannte Metadaten sollen beim unveränderten Speichern nach Möglichkeit erhalten bleiben.

## Typisierung

Metadata-Werte sollen eindeutig typisiert sein.

Beispiele:

```text
String
Integer
Boolean
Timestamp
ObjectID
ContentID
Binary
List
Map
```

Die Interpretation eines Feldes darf nicht ausschließlich von einer Anwendung abhängen.

## Versionierung

Metadaten müssen mit dem NovaOS-Versionierungsmodell zusammenarbeiten.

```text
Version 1
├── Payload A
└── Metadata A

Version 2
├── Payload A
└── Metadata B
```

Eine reine Metadatenänderung darf eine neue Objektversion erzeugen, ohne die `ContentID` eines unveränderten Payloads zu verändern.

## Integrität

Kritische Metadaten sollen gegen unbeabsichtigte Veränderungen geschützt werden können.

```text
Metadata
   ↓
Checksum / Hash
   ↓
Verification
```

Sicherheitsrelevante Metadaten dürfen zusätzlichen Authentizitäts- oder Signaturschutz verwenden.

## Datenschutz

Metadaten können sensible Informationen enthalten.

NovaFile muss daher ermöglichen, Metadaten:

```text
Visible
Protected
Encrypted
```

zu klassifizieren.

Die tatsächliche Sichtbarkeit muss durch die NovaOS-Security-Policy bestimmt werden.

## Suche und Indexierung

Freigegebene NovaFile-Metadaten sollen durch NovaOS indexierbar sein.

```text
NovaFile Metadata
       ↓
Storage Index
       ↓
Search / Discovery
```

Verschlüsselte oder nicht autorisierte Metadaten dürfen dabei nicht offengelegt werden.

## Normative Anforderungen

1. NovaFile MUSS einen strukturierten Metadata-Bereich unterstützen.
2. Metadaten und Payload MÜSSEN logisch getrennt bleiben.
3. Metadata-Felder SOLLEN eindeutig typisiert sein.
4. Das Metadata-Schema MUSS versionierbar und erweiterbar sein.
5. Unbekannte optionale Felder SOLLEN erhalten oder sicher übersprungen werden können.
6. Unbekannte verpflichtende Felder DÜRFEN NICHT stillschweigend ignoriert werden.
7. Semantische Metadaten MÜSSEN integrierbar sein.
8. Objektbeziehungen SOLLEN stabile `ObjectID`s verwenden.
9. Metadaten MÜSSEN gemeinsam mit Objektversionen versionierbar sein.
10. Metadatenänderungen DÜRFEN die `ContentID` eines unveränderten Payloads NICHT verändern.
11. Sensible Metadaten MÜSSEN durch Security- und Encryption-Policy geschützt werden können.
12. Autorisierte Metadaten SOLLEN für Suche und Discovery indexierbar sein.

## Abhängigkeiten

- `NPSPEC-STORAGE-OBJECT-0001`
- `NPSPEC-STORAGE-VERSIONING-0001`
- `NPSPEC-STORAGE-SEMANTIC-0001`
- `NPSPEC-STORAGE-METADATA-0001`
- `NPSPEC-STORAGE-PROVENANCE-0001`
- `NPSPEC-STORAGE-NOVAFILE-0001`
- `ADR-STORAGE-0018`

## Ergebnis

```text
NovaFile
├── Payload
└── Structured Metadata
    ├── Core
    ├── Semantic
    ├── Relationships
    └── Provenance
```

NovaFile erhält damit ein typisiertes, erweiterbares und versionierbares Metadatenmodell, das gemeinsam mit dem Payload transportiert werden kann, ohne dessen ursprüngliches Datenformat zu verändern.