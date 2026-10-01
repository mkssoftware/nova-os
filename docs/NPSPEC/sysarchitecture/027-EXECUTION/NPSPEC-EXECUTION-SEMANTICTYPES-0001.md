# NPSPEC-EXECUTION-SEMANTICTYPES-0001 – Nova Execution Semantic Types

## Status

Angenommen

## Kategorie

Execution / Semantic Types / Execution Model

## Zweck

NovaOS definiert ein semantisches Typsystem für die Ausführung von Operationen.

Operationen beschreiben ihre Ein- und Ausgaben nicht primär über Dateiformate, Speicheradressen oder konkrete Implementierungstypen, sondern über deren systemweit verstandene Bedeutung.

```text
Semantic Input
     ↓
Operation
     ↓
Semantic Output
```

Dadurch kann NovaOS unterschiedliche Provider, Repräsentationen und Hardware verwenden, solange die semantischen Anforderungen des Execution Contracts erhalten bleiben.

## Grundprinzipien

```text
Semantic Type ≠ File Extension
Semantic Type ≠ MIME Type
Semantic Type ≠ Representation
Semantic Type ≠ Object Type
Semantic Type ≠ Memory Layout

Meaning ≠ Representation
Operation ≠ Implementation
Compatibility ≠ Identity
Convertible ≠ Compatible
```

## Execution Type Model

Eine ausführbare Operation beschreibt mindestens:

```text
ExecutionTypeSignature
├── OperationID
├── Input Semantic Types
└── Output Semantic Types
```

Optional:

```text
Type Constraints
Compatibility Requirements
Conversion Policy
Precision Requirements
Validation Requirements
Representation Requirements
```

Beispiel:

```text
Operation:
Image.Resize

Input:
Image.Raster

Output:
Image.Raster
```

Die konkrete Repräsentation kann beispielsweise sein:

```text
PNG
JPEG
WebP
NovaFile Payload
Shared Memory Surface
GPU Texture
```

## Semantische Signatur

Operationen besitzen eine providerunabhängige semantische Signatur.

```text
Image.Resize(
    Image.Raster,
    Dimensions
) → Image.Raster
```

Provider können diese Operation unterschiedlich implementieren.

```text
CPU Provider
GPU Provider
Application Provider
System Provider
Remote Provider
```

Alle müssen jedoch die definierte semantische Signatur erfüllen.

## ExecutionContract Integration

Der Execution Contract referenziert semantische Ein- und Ausgabetypen.

```text
ExecutionContract
├── OperationID
├── Input Semantic Types
├── Output Semantic Types
└── Constraints
```

Damit kann NovaOS zuerst bestimmen:

```text
Was soll verarbeitet werden?
```

und anschließend:

```text
Welche Implementierung kann dies ausführen?
```

## Type Resolution

Vor der Ausführung erfolgt:

```text
Input Object
    ↓
SemanticTypeID
    ↓
Operation Requirements
    ↓
Compatibility Check
```

Mögliche Ergebnisse:

```text
Compatible
CompatibleWithConstraints
Convertible
Incompatible
Unknown
```

`Unknown` darf nicht automatisch als kompatibel behandelt werden.

## Representation Independence

Semantische Typen bleiben unabhängig von ihrer physischen Darstellung.

```text
Image.Raster
├── PNG
├── JPEG
├── WebP
├── Raw Buffer
└── GPU Surface
```

Dadurch muss eine Operation nicht für jedes Dateiformat als eigenständige semantische Operation modelliert werden.

## Conversion

Falls der Eingangstyp nicht direkt kompatibel ist:

```text
Input Type
    ↓
Compatibility Check
    ↓
Conversion Required
    ↓
Semantic Conversion
    ↓
Required Type
```

Eine Konvertierung darf nur erfolgen, wenn:

```text
Conversion Semantics Known
Capability Available
ExecutionContract Allows Conversion
Required Authority Available
Hard Constraints Preserved
```

Lossy Conversion darf nicht stillschweigend erfolgen.

## Type Constraints

Semantic Types können zusätzliche Constraints besitzen.

Beispiele:

```text
Image.Raster
├── Width
├── Height
├── Color Space
├── Bit Depth
└── Alpha Support
```

oder:

```text
Tensor
├── Element Type
├── Shape
├── Layout
└── Precision
```

Dadurch kann ein Typ grundsätzlich kompatibel sein, aber bestimmte Execution Requirements nicht erfüllen.

## Precision

Numerische Semantic Types können Präzisionsanforderungen besitzen.

```text
Tensor<float32>
```

Ein Provider darf daraus nicht ohne Erlaubnis:

```text
Tensor<int8>
```

erzeugen.

Precision Reduction muss durch den Execution Contract erlaubt sein.

## Validation

Vor der Ausführung kann NovaOS prüfen:

```text
Semantic Type
      ↓
Type Constraints
      ↓
Representation
      ↓
Semantic Validation
```

Dabei gilt:

```text
Correct Type ≠ Valid Content
Valid Content ≠ Trusted Content
Trusted Content ≠ Authorized Access
```

## Capability Discovery

Semantic Types ermöglichen providerunabhängige Capability Discovery.

```text
Required Operation
        +
Input Semantic Types
        +
Output Semantic Types
        ↓
Capability Discovery
        ↓
Compatible Providers
```

## Pipeline Integration

Semantic Types verbinden mehrere Operationen.

```text
Object
  ↓
Operation A
  ↓
Semantic Type X
  ↓
Operation B
  ↓
Semantic Type Y
```

Dadurch kann NovaOS Datenpipelines zusammensetzen, ohne dass die beteiligten Provider direkt voneinander wissen müssen.

## Zero-Copy

Semantic Type und physische Datenübertragung bleiben getrennt.

```text
Semantic Object
      ↓
Representation
      ↓
Shared Buffer / Mapping / Copy
```

Ein semantisch identisches Objekt kann daher über Zero-Copy übertragen werden, wenn Representation, Security und Lifetime dies erlauben.

## IPC

Semantic Types können über IPC erhalten bleiben.

```text
Process A
   ↓
Semantic IPC
   ↓
Process B
```

Die empfangende Komponente erhält die semantische Typinformation unabhängig von der verwendeten Serialisierung.

## Remote Execution

Dasselbe Modell gilt für entfernte Provider.

```text
Local Object
     ↓
Semantic Type
     ↓
Remote Execution
     ↓
Semantic Result
```

Location darf die Bedeutung des Typs nicht verändern.

## Versionierung

Semantic Types müssen versionierbar sein.

```text
SemanticTypeID
+
SemanticTypeVersion
```

Änderungen müssen auf Kompatibilität geprüft werden.

NovaOS darf inkompatible Typversionen nicht stillschweigend gleichsetzen.

## Security

Semantic Type Information gewährt keine Zugriffsrechte.

```text
Known SemanticTypeID ≠ Read Capability
Known Object Type ≠ Object Authority
Compatible Type ≠ Permission
```

Für Zugriff und Verarbeitung bleiben entsprechende Capabilities erforderlich.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
SemanticTypeID
SemanticTypeVersion
Type Constraints
Representation
Compatible Operations
Compatible Providers
Conversion Paths
Validation State
```

## Normative Anforderungen

1. NovaOS MUSS semantische Ein- und Ausgabetypen für Execution Operations unterstützen.
2. Semantic Types MÜSSEN von Representation und Speicherlayout getrennt bleiben.
3. Execution Contracts MÜSSEN Semantic Types direkt referenzieren können.
4. Provider MÜSSEN anhand semantischer Signaturen vergleichbar sein.
5. Inkompatible Typen DÜRFEN nicht ohne explizite Conversion verarbeitet werden.
6. Lossy Conversion DARF nicht stillschweigend erfolgen.
7. Type Constraints und Precision Requirements MÜSSEN bei der Provider-Auswahl berücksichtigt werden.
8. Semantic Types MÜSSEN über IPC und Remote Execution erhalten bleiben können.
9. Semantic Type Versionen MÜSSEN auf Kompatibilität geprüft werden.
10. Semantic Type Information DARF keine Capability oder Zugriffsautorität erzeugen.
11. Semantic Types MÜSSEN mit Execution Pipelines und Zero-Copy kompatibel sein.
12. Type Resolution und Compatibility State MÜSSEN autorisiert introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-EXECUTION-CONTRACT-0001`
- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-COMPATIBILITY-0001`
- `NPSPEC-SEMANTIC-CONVERSION-0001`
- `NPSPEC-SEMANTIC-VALIDATION-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-SEMANTIC-IPC-0001`
- `NPSPEC-OBJECT-PIPELINE-0001`
- `NPSPEC-OBJECT-ZEROCOPY-0001`
- `NPSPEC-ARCH-LOCATIONTRANSPARENCY-0001`
- `ADR-ARCH-0049`

## Ergebnis

```text
Semantic Input
      ↓
Type Resolution
      ↓
Compatibility / Conversion
      ↓
ExecutionContract
      ↓
Capability + Provider Discovery
      ↓
Execution
      ↓
Semantic Output
```

NovaOS erhält damit eine semantische Typisierung seiner gesamten Ausführungskette. Operationen werden nicht mehr an konkrete Dateiformate, Speicherlayouts oder Implementierungen gekoppelt, sondern an die Bedeutung ihrer Daten, wodurch Provider, Hardware und Ausführungsorte flexibel austauschbar bleiben.