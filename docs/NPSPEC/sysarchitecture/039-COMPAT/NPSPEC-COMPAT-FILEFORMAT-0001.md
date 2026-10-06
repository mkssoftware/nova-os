# NPSPEC-COMPAT-FILEFORMAT-0001 – Nova File Format Compatibility

## Status

Angenommen

## Kategorie

Compatibility / File Format

## Zweck

NovaOS definiert eine Kompatibilitätsschicht für fremde und ältere Dateiformate.

Dateiformat-Kompatibilität ermöglicht das Erkennen, Lesen, Schreiben, Konvertieren und Anzeigen von Daten, ohne das jeweilige Format zum nativen Datenmodell von NovaOS zu machen.

## Grundprinzipien

```text
File Format ≠ Semantic Type
File Extension ≠ Format Identity
Format Compatibility ≠ Application Dependency
Codec ≠ Authority
Conversion ≠ Object Identity
Unknown Format ≠ Invalid File
```

## Modell

```text
File / Object
     ↓
Format Detection
     ↓
FormatID
     ↓
Compatibility Provider
     ↓
Decode / Encode / Convert
     ↓
Semantic Data
```

## Formatidentität

Ein Dateiformat wird durch eine stabile `FormatID` beschrieben.

```text
FileFormat
├── FormatID
├── Version
├── MediaType
├── Signatures
├── Extensions
├── SemanticTypes
└── Capabilities
```

Dateiendungen dienen nur als Hinweis und dürfen nicht allein die Formatidentität bestimmen.

## Erkennung

NovaOS darf Formate anhand mehrerer Merkmale erkennen:

```text
Magic / Signature
Container Structure
Metadata
Declared Media Type
Extension
Semantic Analysis
```

Bei widersprüchlichen Informationen haben validierte Inhaltsmerkmale Vorrang vor der Dateiendung.

## Provider

Ein Format darf durch mehrere Provider unterstützt werden:

```text
FormatID
├── Decoder A
├── Decoder B
├── Encoder
├── Converter
└── Preview Provider
```

Die konkrete Implementierung wird anhand von Compatibility, Trust, Policy und Execution Contract ausgewählt.

## Operationen

Format Provider dürfen insbesondere folgende Operationen bereitstellen:

```text
Detect
Validate
Decode
Encode
ReadMetadata
WriteMetadata
Preview
Convert
Repair
```

Nicht jeder Provider muss alle Operationen unterstützen.

## Semantische Daten

Dekodierte Inhalte sollen auf NovaOS-Semantic Types abgebildet werden.

```text
JPEG
  ↓
Decode
  ↓
de.nova.semantic.image
```

Das konkrete Speicherformat bleibt dadurch von der semantischen Bedeutung getrennt.

## NovaFile

Ein NovaFile darf fremde Formate als Payload enthalten:

```text
NovaFile (.nf)
├── Payload
│   └── Foreign Format
├── Metadata
└── Relations
```

Das Einbetten verändert weder die Formatidentität des Payloads noch dessen semantischen Typ.

## Konvertierung

Konvertierungen erzeugen grundsätzlich neuen Inhalt.

```text
Source Object
     ↓
Conversion
     ↓
New Content
```

Ob daraus ein neues ObjectID entsteht, richtet sich nach der jeweiligen Objektoperation und den NovaOS-Identitätsregeln.

Verlustbehaftete Konvertierungen müssen als solche erkennbar sein.

## Sicherheit

Parser und Decoder verarbeiten potenziell nicht vertrauenswürdige Daten.

Riskante Format Provider sollen daher isoliert ausgeführt werden.

Dateiinhalte dürfen durch Parsing oder Decoding keine zusätzliche Authority erhalten.

## Normative Anforderungen

1. Dateiformat und Semantic Type MÜSSEN getrennt behandelt werden.
2. Dateiendungen DÜRFEN nicht allein die Formatidentität bestimmen.
3. Formate MÜSSEN über stabile `FormatID`s identifizierbar sein.
4. Mehrere Provider MÜSSEN dasselbe Format unterstützen können.
5. Formatversionen MÜSSEN unterscheidbar sein.
6. Decoder und Parser MÜSSEN Eingaben validieren.
7. Nicht vertrauenswürdige Format Provider SOLLEN isoliert ausführbar sein.
8. Formatverarbeitung DARF keine zusätzliche Authority erzeugen.
9. Konvertierungen MÜSSEN Format- und Semantikänderungen nachvollziehbar machen.
10. Verlustbehaftete Konvertierungen MÜSSEN erkennbar sein.
11. Unbekannte Formate DÜRFEN nicht automatisch als beschädigt gelten.
12. FormatID, Version, Provider und Unterstützungsgrad MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-CAPABILITY-SEMANTICTYPE-0001`
- `NPSPEC-CAPABILITY-DISCOVERY-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-FILESYSTEM-SEMANTIC-0001`
- `NPSPEC-FILESYSTEM-OBJECTID-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`

## Ergebnis

NovaOS kann fremde und ältere Dateiformate unabhängig von bestimmten Programmen erkennen und verarbeiten. Formatidentität, semantische Bedeutung und Objektidentität bleiben getrennt, während Decoder, Encoder und Konverter austauschbare Compatibility Provider bilden.