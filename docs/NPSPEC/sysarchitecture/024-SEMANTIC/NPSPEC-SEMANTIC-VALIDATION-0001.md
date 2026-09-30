# NPSPEC-SEMANTIC-VALIDATION-0001 – Nova Semantic Validation

## Status

Angenommen

## Kategorie

Semantic / Validation / Integrity

## Zweck

NovaOS definiert eine systemweite Validierung semantischer Daten, Typen, Metadaten, Beziehungen und Operationen.

```text
Object / Message / Resource
          ↓
Semantic Validation
          ↓
Valid / Invalid / Unknown
          ↓
Controlled Processing
```

Validierung prüft nicht nur die technische Struktur, sondern auch, ob Daten ihrer deklarierten semantischen Bedeutung entsprechen.

## Grundprinzipien

```text
Syntactically Valid ≠ Semantically Valid
Semantically Valid ≠ Trusted
Valid ≠ Authorized
Schema Valid ≠ Safe
Known Type ≠ Valid Instance
Validation Failure ≠ Permission to Repair
Unknown ≠ Valid
```

## Validierungsmodell

Eine Validierung berücksichtigt mindestens:

```text
SemanticValidation
├── SemanticTypeID
├── Schema Version
├── Structural Rules
├── Semantic Constraints
└── Validation State
```

Optional:

```text
Metadata Constraints
Relationship Constraints
Representation Rules
Security Requirements
Trust Requirements
Provenance Requirements
Context Constraints
```

## Validierungszustände

NovaOS verwendet mindestens:

```text
Valid
Invalid
Unknown
```

Optional können detailliertere Zustände verwendet werden:

```text
ValidWithWarnings
Unsupported
Incomplete
```

`Unknown` darf nicht automatisch als `Valid` interpretiert werden.

## Strukturelle Validierung

Zunächst wird geprüft, ob die Repräsentation korrekt aufgebaut ist.

```text
Input
 ↓
Representation Validation
 ↓
Schema Validation
```

Beispiele:

```text
Required Fields
Field Types
Encoding
Length
Ranges
Structure
```

## Semantische Validierung

Anschließend wird geprüft, ob die Inhalte den Regeln des Semantic Type entsprechen.

Beispiel:

```text
SemanticType:
Nova.Image.Raster

Constraints:
Width  > 0
Height > 0
PixelFormat supported
```

Ein strukturell korrektes Objekt kann semantisch trotzdem ungültig sein.

## Metadata Validation

Semantic Metadata wird gegen definierte Typen und Regeln geprüft.

```text
MetadataTypeID
      ↓
Expected ValueType
      ↓
Constraints
      ↓
Validation
```

Beispiel:

```text
Duration >= 0
Resolution > 0
Timestamp valid
Unit compatible
```

## Relationship Validation

Semantic Relationships müssen ebenfalls validiert werden.

```text
Source Type
     +
RelationshipType
     +
Target Type
     ↓
Relationship Validation
```

Beispiel:

```text
DerivedFrom
```

kann Regeln für zulässige Source- und Target-Typen definieren.

Graphregeln wie:

```text
Directional
Symmetric
Transitive
Inverse
```

müssen berücksichtigt werden.

## Conversion Validation

Nach einer Semantic Conversion muss das Ergebnis erneut validiert werden.

```text
Source
  ↓
Conversion
  ↓
Target
  ↓
Semantic Validation
```

Ein erfolgreicher Converter-Aufruf bedeutet nicht automatisch ein gültiges Ergebnis.

## IPC Validation

Eingehende Semantic IPC-Daten müssen vor sicherheitskritischer Verarbeitung validierbar sein.

```text
IPC Message
    ↓
Schema Validation
    ↓
Semantic Validation
    ↓
Authorization
    ↓
Operation
```

Validierung ersetzt keine Capability-Prüfung.

## Context Validation

Bestimmte semantische Regeln können vom Kontext abhängen.

```text
Semantic Object
      +
Execution Context
      ↓
Context Validation
```

Dabei können beispielsweise gelten:

```text
Operation
Purpose
ExecutionContract
Security Domain
Required Precision
Determinism
```

## Trust

Validierung und Trust bleiben getrennt.

```text
Semantic Validation
        +
Provenance
        +
Trust Evidence
        ↓
Trust Evaluation
```

Ein semantisch gültiges Objekt kann aus einer nicht vertrauenswürdigen Quelle stammen.

## Fehlerbehandlung

Validation Errors sollen strukturiert beschrieben werden.

```text
ValidationError
├── RuleID
├── Location
├── Expected
├── Observed
└── Severity
```

Dadurch können Komponenten Fehler nachvollziehbar behandeln.

## Reparatur

NovaOS darf zwischen Validierung und Reparatur unterscheiden.

```text
Validate
   ↓
Invalid
   ↓
Repair Capability
   ↓
Revalidate
```

Automatische Reparatur darf nur erfolgen, wenn Policy und Autorität dies erlauben.

## Caching

Validierungsergebnisse können gecacht werden.

Der Cache muss an mindestens folgende Zustände gebunden sein:

```text
Object Version
Semantic Type Version
Validation Rule Version
```

Eine Änderung dieser Zustände kann eine erneute Validierung erforderlich machen.

## Introspection

Autorisierte Komponenten sollen abfragen können:

```text
Validation State
SemanticTypeID
Schema Version
Validation Rule Version
Validation Errors
Warnings
Validation Timestamp
Validator Identity
```

## Normative Anforderungen

1. NovaOS MUSS strukturelle und semantische Validierung unterscheiden.
2. Semantic Types MÜSSEN Validierungsregeln definieren können.
3. `Unknown` DARF NICHT automatisch als `Valid` gelten.
4. Metadata und Relationships MÜSSEN semantisch validierbar sein.
5. Conversion-Ergebnisse SOLLEN erneut validiert werden.
6. Eingehende Semantic IPC-Daten MÜSSEN vor kritischer Verarbeitung validierbar sein.
7. Validierung DARF weder Trust noch Authorization ersetzen.
8. Validation Errors MÜSSEN strukturiert darstellbar sein.
9. Reparatur MUSS von Validierung getrennt bleiben und explizite Autorität benötigen.
10. Validation State MUSS introspektierbar und versionierbar sein.

## Abhängigkeiten

- `NPSPEC-SEMANTIC-TYPE-0001`
- `NPSPEC-SEMANTIC-FILE-0001`
- `NPSPEC-SEMANTIC-IPC-0001`
- `NPSPEC-SEMANTIC-CAPABILITY-0001`
- `NPSPEC-SEMANTIC-METADATA-0001`
- `NPSPEC-SEMANTIC-RELATIONSHIP-0001`
- `NPSPEC-SEMANTIC-CONVERSION-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-TRUST-PROVENANCE-0001`
- `NPSPEC-SECURITY-CAPABILITY-0001`
- `ADR-ARCH-0017`

## Ergebnis

```text
Semantic Data
     ↓
Structural Validation
     ↓
Semantic Validation
     ↓
Context Validation
     ↓
Valid / Invalid / Unknown
     ↓
Authorized Processing
```

NovaOS erhält damit eine einheitliche Validierungsschicht, die sicherstellt, dass semantisch beschriebene Daten, Metadaten, Beziehungen und Nachrichten nicht nur technisch lesbar, sondern auch gemäß ihrer definierten Bedeutung konsistent und überprüfbar sind.