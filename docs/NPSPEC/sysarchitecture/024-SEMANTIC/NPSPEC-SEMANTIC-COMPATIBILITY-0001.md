# NPSPEC-SEMANTIC-COMPATIBILITY-0001 – Nova Semantic Compatibility

## Status

Angenommen

## Kategorie

Semantic / Compatibility / Type System

## Zweck

NovaOS definiert ein systemweites Modell zur Bestimmung, ob Semantic Types, Capabilities, Ressourcen und Daten ohne Bedeutungsverlust oder mit kontrollierter Anpassung miteinander verwendet werden können.

```text
Source Semantics
       ↓
Compatibility Evaluation
       ↓
Compatible / Convertible / Incompatible / Unknown
       ↓
Execution Decision
```

## Grundprinzipien

```text
Same Representation ≠ Semantic Compatibility
Same Semantic Type ≠ Representation Compatibility
Compatible ≠ Identical
Convertible ≠ Compatible
Newer Version ≠ Compatible
Structural Compatibility ≠ Semantic Compatibility
Compatibility ≠ Authorization
Unknown ≠ Compatible
```

## Kompatibilitätszustände

NovaOS verwendet mindestens:

```text
Compatible
CompatibleWithConstraints
Convertible
Incompatible
Unknown
```

Dabei gilt:

```text
Unknown ≠ Compatible
```

## Compatibility Model

Eine Prüfung berücksichtigt mindestens:

```text
SemanticCompatibility
├── Source SemanticTypeID
├── Target SemanticTypeID
├── Source Version
├── Target Version
├── Required Semantics
└── Compatibility State
```

Optional:

```text
Representation
Constraints
Precision
Loss Properties
Required Conversion
Security Requirements
Trust Requirements
```

## Direkte Kompatibilität

Direkte Kompatibilität besteht, wenn ein Wert ohne semantische Transformation verwendet werden kann.

```text
Source
  ↓
Compatibility Check
  ↓
Target Operation
```

Dabei dürfen die vom Ziel benötigten Eigenschaften nicht verletzt werden.

## Typbeziehungen

Definierte Typbeziehungen können bei der Kompatibilitätsprüfung berücksichtigt werden.

```text
Nova.Document
      ↑
Nova.Document.Text
      ↑
Nova.Document.Markdown
```

Ein Subtyp kann dort verwendet werden, wo der Basistyp akzeptiert wird, sofern dessen definierte Constraints erfüllt sind.

Die Richtung darf nicht automatisch umgekehrt werden.

## Constraint-Kompatibilität

Zwei grundsätzlich kompatible Typen können unterschiedliche Constraints besitzen.

```text
Type Compatible
      +
Constraint Check
      ↓
CompatibleWithConstraints
```

Beispiele:

```text
Precision
Resolution
Encoding
Range
Unit
Determinism
Quality
```

## Repräsentationskompatibilität

Semantisch identische Daten können unterschiedliche Repräsentationen besitzen.

```text
Nova.Image.Raster
├── PNG
└── WebP
```

Kann der Verbraucher die vorhandene Repräsentation nicht direkt verarbeiten, kann eine Representation Conversion erforderlich sein.

## Convertible

Sind Source und Target nicht direkt kompatibel, kann ein definierter Conversion Path existieren.

```text
Source Type
    ↓
Not Directly Compatible
    ↓
Semantic Conversion
    ↓
Target Type
```

Dabei gilt:

```text
Convertible ≠ Compatible
```

Die Konvertierung bleibt eine explizite Operation.

## Versionskompatibilität

Semantic-Type-Versionen müssen explizit geprüft werden.

```text
Type v1
   ↓
Compatibility Rules
   ↓
Type v2
```

Inkompatible Änderungen dürfen nicht allein aufgrund derselben `SemanticTypeID` akzeptiert werden.

## Capability-Kompatibilität

Capability Inputs und Outputs können gegen Semantic Types geprüft werden.

```text
Object SemanticType
        ↓
Compatibility Check
        ↓
Capability Input Type
```

Nur bei erfolgreicher Prüfung darf die semantische Zuordnung erfolgen.

Authorization bleibt separat erforderlich.

## IPC-Kompatibilität

Bei Semantic IPC muss der Empfänger prüfen können, ob die empfangenen Typen mit seiner erwarteten Semantik kompatibel sind.

```text
Sender Type
    ↓
IPC
    ↓
Receiver Expected Type
    ↓
Compatibility Evaluation
```

Schema-Kompatibilität und semantische Kompatibilität bleiben getrennt.

## ExecutionContract

Der ExecutionContract kann die zulässige Kompatibilität einschränken.

```text
Exact Type Required
Compatible Type Allowed
Conversion Allowed
Lossless Conversion Required
Lossy Conversion Forbidden
```

Hard Requirements dürfen durch Kompatibilitätsauflösung nicht abgeschwächt werden.

## Validation

Kompatibilität ersetzt keine Validierung.

```text
Compatible Type
      ↓
Semantic Validation
      ↓
Usable Instance
```

Ein Objekt eines kompatiblen Typs kann trotzdem ungültige Inhalte besitzen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Source Type
Target Type
Compatibility State
Constraints
Required Conversion
Loss Properties
Version Compatibility
Reason
```

## Normative Anforderungen

1. NovaOS MUSS semantische Kompatibilität explizit bestimmen können.
2. `Unknown` DARF NICHT als kompatibel behandelt werden.
3. Direkte Kompatibilität und Konvertierbarkeit MÜSSEN getrennt bleiben.
4. Typbeziehungen DÜRFEN nur gemäß ihrer definierten Richtung und Semantik verwendet werden.
5. Constraints MÜSSEN Teil der Kompatibilitätsprüfung sein können.
6. Semantic-Type- und Representation-Kompatibilität MÜSSEN getrennt behandelbar sein.
7. Versionskompatibilität MUSS explizit überprüfbar sein.
8. ExecutionContract Hard Requirements DÜRFEN NICHT durch Compatibility Resolution abgeschwächt werden.
9. Kompatibilität DARF weder Validation noch Authorization ersetzen.
10. Compatibility Decisions MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-RESOURCE-0001`
- `NPSPEC-SEMANTIC-IPC-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-CONVERSION-0001`
- `NPSPEC-SEMANTIC-VALIDATION-0001`
- `NPSPEC-CAPABILITY-VERSIONING-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-ARCH-EXECUTIONCONTRACT-0001`
- `ADR-ARCH-0018`

## Ergebnis

```text
Source Semantics
       ↓
Type + Version + Constraints
       ↓
Compatibility Evaluation
       ↓
Compatible
 / CompatibleWithConstraints
 / Convertible
 / Incompatible
 / Unknown
       ↓
Controlled Execution
```

NovaOS erhält damit eine einheitliche Kompatibilitätsschicht, die verhindert, dass technisch ähnliche Daten oder Schnittstellen automatisch als semantisch austauschbar behandelt werden, und ermöglicht gleichzeitig kontrollierte Kompatibilität und Konvertierung zwischen unterschiedlichen Systemkomponenten.