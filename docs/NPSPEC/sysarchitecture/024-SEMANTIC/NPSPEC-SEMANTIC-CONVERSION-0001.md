# NPSPEC-SEMANTIC-CONVERSION-0001 – Nova Semantic Conversion

## Status

Angenommen

## Kategorie

Semantic / Conversion / Transformation

## Zweck

NovaOS definiert ein systemweites Modell zur kontrollierten Umwandlung zwischen Semantic Types und deren Repräsentationen.

```text
Source
  ↓
Semantic Type A
  ↓
Conversion Capability
  ↓
Semantic Type B
  ↓
Result
```

Konvertierungen werden als explizite Operationen behandelt und nicht als versteckte Nebenwirkung.

## Grundprinzipien

```text
Conversion ≠ Rename
Conversion ≠ Serialization
Conversion ≠ Representation Change
Convertible ≠ Compatible
Conversion Path ≠ Authority
Available Converter ≠ Authorized Converter
Successful Conversion ≠ Lossless Conversion
```

## Conversion-Modell

Eine Conversion beschreibt mindestens:

```text
SemanticConversion
├── ConversionTypeID
├── Source SemanticTypeID
├── Target SemanticTypeID
├── Conversion Properties
└── State
```

Optional:

```text
Source Representation
Target Representation
Loss Properties
Quality Properties
Resource Requirements
Trust Requirements
Determinism
Metadata Policy
Provenance Policy
```

## Typkonvertierung

Eine echte semantische Konvertierung verändert den Datentyp oder dessen Bedeutung.

```text
Nova.Document.Markdown
        ↓
Document.Render
        ↓
Nova.Image.Raster
```

Dies unterscheidet sich von einer reinen Änderung der Repräsentation.

## Repräsentationskonvertierung

Die semantische Bedeutung kann erhalten bleiben.

```text
Nova.Image.Raster
    PNG
     ↓
Image.Encode
     ↓
Nova.Image.Raster
    WebP
```

Dabei gilt:

```text
Semantic Type = gleich
Representation = verändert
```

## Conversion Discovery

NovaOS kann geeignete Conversion Capabilities automatisch suchen.

```text
Source Type
    +
Target Type
    ↓
Semantic Query
    ↓
Capability Registry
    ↓
Conversion Candidates
```

Die Auffindbarkeit eines Konverters erzeugt keine Autorität.

## Conversion Graph

Mehrere Konvertierungen können zu einem Pfad kombiniert werden.

```text
Type A
  ↓
Type B
  ↓
Type C
  ↓
Type D
```

NovaOS kann einen geeigneten Conversion Path bestimmen.

Die Auswahl kann berücksichtigen:

```text
Quality
Loss
Latency
Resource Cost
Trust
Sovereignty
Determinism
Security
```

## Verlustbehaftete Konvertierung

Konvertierungen müssen relevante Informationsverluste beschreiben können.

```text
Lossless
Lossy
UnknownLoss
```

Beispiel:

```text
RAW Image
   ↓
JPEG
```

Eine verlustbehaftete Konvertierung darf nicht still als verlustfrei behandelt werden.

## Metadata

Konvertierungen müssen definieren, wie Metadaten behandelt werden.

```text
Preserve
Transform
Drop
Generate
```

Sicherheits-, Privacy- und Provenance-Metadaten dürfen nicht unkontrolliert entfernt werden.

## Beziehungen

Semantic Relationships können bei einer Konvertierung:

```text
Preserved
Transformed
Created
Invalidated
```

werden.

Die Behandlung muss durch die Conversion Semantics definiert sein.

## Provenance

Konvertierte Objekte sollen ihre Herkunft nachvollziehbar behalten.

```text
Source Object
      ↓
Conversion
      ↓
Result Object
      ↓
DerivedFrom → Source Object
```

Dabei können Provider, Algorithmus und Conversion-Version dokumentiert werden.

## Capability Security

Eine Konvertierung benötigt explizite Autorität.

```text
Read(Source)
     +
Conversion Capability
     +
Write(Target)
     ↓
Authorized Conversion
```

Ein Conversion Path darf keine Capability-Grenzen umgehen.

## ExecutionContract

Konvertierungen können über `Nova.ExecutionContract` gesteuert werden.

```text
Conversion Request
       ↓
ExecutionContract
       ↓
Quality / Loss
Resources
Deadline
Determinism
Trust
Sovereignty
       ↓
Provider Selection
```

## Automatische Konvertierung

NovaOS darf automatische Konvertierungen durchführen, wenn:

```text
Semantics bekannt
+
Policy erlaubt
+
Authority vorhanden
+
Hard Requirements erfüllt
```

Informationsverlust oder sicherheitsrelevante Änderungen dürfen nicht still erfolgen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
ConversionTypeID
Source SemanticTypeID
Target SemanticTypeID
Provider
Loss Properties
Quality Properties
Resource Requirements
Metadata Policy
Provenance Policy
```

## Normative Anforderungen

1. NovaOS MUSS Semantic Conversions explizit modellieren können.
2. Semantic Conversion und Representation Conversion MÜSSEN unterscheidbar sein.
3. Conversion Discovery DARF keine Autorität erzeugen.
4. Mehrstufige Conversion Paths MÜSSEN darstellbar sein.
5. Verlustbehaftete Konvertierungen MÜSSEN als solche erkennbar sein.
6. Metadata- und Relationship-Behandlung MUSS definierbar sein.
7. Provenance SOLL über Konvertierungen erhalten bleiben.
8. Jede Konvertierung MUSS die erforderlichen Capabilities respektieren.
9. ExecutionContracts SOLLEN Conversion-Auswahl und Provider Selection steuern können.
10. Automatische Konvertierungen DÜRFEN Hard Requirements und Security Policy NICHT umgehen.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-FILE-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-QUERY-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-PROVIDER-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0016`

## Ergebnis

```text
Source Semantic Type
        ↓
Conversion Requirement
        ↓
Conversion Discovery
        ↓
Authorized Conversion Path
        ↓
Provider Execution
        ↓
Target Semantic Type
        ↓
Preserved Provenance
```

NovaOS erhält damit eine einheitliche semantische Konvertierungsschicht, über die Daten kontrolliert zwischen Typen und Repräsentationen transformiert werden können, während Qualität, Informationsverlust, Metadaten, Provenance, Security und Autorität explizit erhalten und überprüfbar bleiben.