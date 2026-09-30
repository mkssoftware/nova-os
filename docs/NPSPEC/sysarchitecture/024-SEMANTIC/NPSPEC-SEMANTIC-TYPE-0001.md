# NPSPEC-SEMANTIC-TYPE-0001 – Nova Semantic Type System

## Status

Angenommen

## Kategorie

Architecture / Semantic Types / Data Model

## Zweck

NovaOS definiert ein systemweites semantisches Typsystem, das Daten nach ihrer Bedeutung statt ausschließlich nach Dateiformat, Speicherort oder binärer Darstellung beschreibt.

```text
Raw Data
   ↓
Representation
   ↓
Semantic Type
   ↓
Compatible Capability
   ↓
Operation
```

Dadurch können Komponenten miteinander arbeiten, ohne konkrete Anwendungen oder Datenformate kennen zu müssen.

## Grundprinzipien

```text
Semantic Type ≠ File Extension
Semantic Type ≠ MIME Type
Semantic Type ≠ Storage Format
Semantic Type ≠ Application
Semantic Type ≠ Object Identity
Semantic Compatibility ≠ Binary Compatibility
```

Ein semantischer Typ beschreibt, **was Daten bedeuten**, nicht ausschließlich, wie sie gespeichert sind.

## SemanticTypeID

Jeder definierte Typ besitzt eine stabile Identität.

```text
SemanticType
├── SemanticTypeID
├── Name
├── Version
├── Properties
└── Constraints
```

Beispiele:

```text
Nova.Document.Text
Nova.Image.Raster
Nova.Image.Vector
Nova.Audio.Stream
Nova.Video.Stream
Nova.Model.3D
Nova.Math.Expression
Nova.Geo.Location
Nova.Contact.Person
Nova.Code.Source
```

`SemanticTypeID` bleibt unabhängig von:

```text
Filename
Extension
Application
Storage Location
Provider
Encoding
```

## Repräsentationen

Ein semantischer Typ kann mehrere physische Repräsentationen besitzen.

```text
Nova.Image.Raster
├── PNG
├── JPEG
├── WebP
└── NovaFile Payload
```

Damit kann NovaOS erkennen:

```text
Different Representation
        ↓
Same Semantic Meaning
```

Die konkrete Repräsentation bleibt dennoch explizit verfügbar, wenn sie für eine Operation relevant ist.

## Typbeziehungen

Semantic Types können Beziehungen besitzen:

```text
IsA
Contains
DerivedFrom
CompatibleWith
ConvertibleTo
```

Beispiel:

```text
Nova.Document
    ↑
Nova.Document.Text
    ↑
Nova.Document.Markdown
```

Typbeziehungen dürfen nicht automatisch Sicherheits- oder Autoritätsbeziehungen erzeugen.

## Capability Integration

Capabilities deklarieren die Semantic Types, die sie akzeptieren oder erzeugen.

```text
Capability:
Image.Resize

Input:
Nova.Image.Raster

Output:
Nova.Image.Raster
```

NovaOS kann dadurch passende Provider über Capability Discovery bestimmen.

```text
Semantic Input
      ↓
Required Operation
      ↓
Capability Discovery
      ↓
Compatible Provider
```

## ExecutionContract Integration

`Nova.ExecutionContract` verwendet Semantic Types zur Beschreibung von Ein- und Ausgaben.

```text
ExecutionContract
├── Operation
├── Input Semantic Types
├── Output Semantic Types
├── Constraints
└── Required Capabilities
```

Beispiel:

```text
Input:
Nova.Document.Text

Operation:
Render

Output:
Nova.Image.Raster
```

Der konkrete Provider kann anschließend anhand von Capability, Trust, Ressourcen und Policy gewählt werden.

## Konvertierung

Sind Typen nicht direkt kompatibel, kann NovaOS eine explizite Transformation suchen.

```text
Type A
  ↓
Conversion Capability
  ↓
Type B
```

Konvertierung ist eine normale Operation und darf nicht implizit Autoritäts-, Qualitäts- oder Sicherheitsanforderungen umgehen.

## Versionierung

Semantic Types müssen evolvierbar sein.

```text
SemanticTypeID
├── Version 1
├── Version 2
└── Version 3
```

Kompatible Erweiterungen sollen bestehende Verbraucher nicht unnötig brechen.

Inkompatible Änderungen müssen explizit erkennbar sein.

## NovaFile Integration

NovaFile kann den semantischen Typ eines Objekts direkt in seinen Metadaten speichern.

```text
NovaFile
├── ObjectID
├── Payload
├── SemanticTypeID
├── Representation
└── Metadata
```

Dadurch ist die Bedeutung eines Objekts nicht ausschließlich von Dateiname oder Endung abhängig.

## Sicherheit

Semantic Types stellen keine Autorität dar.

```text
Known Type ≠ Permission
Compatible Type ≠ Access
Convertible Type ≠ Authorization
```

Jede Operation benötigt weiterhin die erforderlichen Capabilities.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
SemanticTypeID
Name
Version
Parent Types
Properties
Representations
Compatibility
Available Conversions
Associated Capabilities
```

## Normative Anforderungen

1. NovaOS MUSS Daten systemweit durch stabile Semantic Types beschreiben können.
2. Semantic Types MÜSSEN unabhängig von Dateiendung, Anwendung und Speicherort sein.
3. Ein Semantic Type DARF mehrere physische Repräsentationen besitzen.
4. Typbeziehungen MÜSSEN explizit definiert werden.
5. Capabilities SOLLEN akzeptierte und erzeugte Semantic Types deklarieren.
6. ExecutionContracts SOLLEN Semantic Types für Ein- und Ausgaben verwenden.
7. Typkonvertierungen MÜSSEN als explizite Operationen behandelbar sein.
8. Semantic Types DÜRFEN keine Autorität erzeugen.
9. Semantic Types MÜSSEN versionierbar und evolvierbar sein.
10. Semantic-Type-Informationen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-ARCH-SYSTEMMODEL-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-REGISTRY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-STORAGE-SEMANTIC-0001`
- `NPSPEC-STORAGE-NOVAFILE-METADATA-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`
- `ADR-ARCH-0008`

## Ergebnis

```text
Data
 ↓
Semantic Meaning
 ↓
SemanticTypeID
 ↓
Required Operation
 ↓
Compatible Capability
 ↓
Provider Selection
 ↓
Execution
```

NovaOS erhält damit eine gemeinsame semantische Sprache für Daten und Fähigkeiten. Komponenten müssen nicht mehr primär wissen, welche Anwendung oder welches Dateiformat verwendet wird, sondern können anhand der tatsächlichen Bedeutung der Daten passende Capabilities und Provider bestimmen.