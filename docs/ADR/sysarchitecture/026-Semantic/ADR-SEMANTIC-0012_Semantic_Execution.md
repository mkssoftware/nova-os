# ADR-SEMANTIC-0012 – Semantic Execution

## Status

Angenommen

## Kategorie

Semantic Architecture / Execution

## Kontext

NovaOS beschreibt Daten, Ressourcen und Capabilities semantisch. Diese Informationen sollen nicht nur für Discovery und Validierung verwendet werden, sondern auch die tatsächliche Ausführung steuern können.

Eine Ausführungsanforderung soll daher beschreiben können, **welches Ergebnis benötigt wird**, ohne eine konkrete Implementierung vorzugeben.

## Entscheidung

NovaOS unterstützt **Semantic Execution**.

```text
Typed Input
    ↓
Semantic Requirement
    ↓
Capability Resolution
    ↓
Execution Contract
    ↓
Provider
    ↓
Typed Output
```

Die Ausführung basiert auf semantischen Anforderungen und expliziten Contracts.

## Semantic Requirement

Eine Ausführung kann definieren:

```text
Input Types
Required Output Type
Required Capability
Semantic Metadata
Execution Constraints
```

NovaOS bestimmt daraus einen gültigen Ausführungspfad.

## Provider-Auswahl

Semantic Execution ist von der konkreten Implementierung getrennt.

```text
Semantic Operation
       ↓
Compatible Capability
       ↓
Compatible Provider
       ↓
Execution
```

Der Provider wird anhand der geltenden Capability- und Execution-Contracts ausgewählt.

## Conversion

Sind Ein- und Ausgabetypen nicht direkt kompatibel, können definierte Semantic Conversions Bestandteil des Ausführungspfads sein.

```text
Type A
  ↓
Conversion
  ↓
Capability
  ↓
Type B
```

Nicht erlaubte oder verlustbehaftete Konvertierungen dürfen nicht stillschweigend eingefügt werden.

## Validation

Vor der Ausführung müssen relevante Eingaben semantisch validiert werden.

```text
Typed Input
    ↓
Validation
    ↓
Execution
```

Das Ergebnis muss ebenfalls dem deklarierten Output Contract entsprechen.

## Execution Contract

Semantic Execution arbeitet mit dem systemweiten Execution Contract zusammen.

Dieser kann beispielsweise Anforderungen an:

```text
Determinism
Latency
Resource Budget
Trust
Data Sovereignty
```

enthalten.

Semantische Kompatibilität allein reicht nicht für die Ausführung.

## Normative Anforderungen

1. NovaOS MUSS semantisch beschriebene Ausführungsanforderungen unterstützen.
2. Ein- und Ausgaben MÜSSEN über Semantic Types beschreibbar sein.
3. NovaOS MUSS passende Capabilities anhand semantischer Contracts bestimmen können.
4. Die konkrete Provider-Auswahl MUSS von der semantischen Anforderung getrennt bleiben.
5. Semantic Conversions DÜRFEN Bestandteil eines Ausführungspfads sein.
6. Eingaben MÜSSEN entsprechend ihrer Contracts validierbar sein.
7. Ergebnisse MÜSSEN dem geforderten Semantic Output Contract entsprechen.
8. Semantic Execution MUSS Execution Contracts berücksichtigen.

## Konsequenzen

### Positive Konsequenzen

- Anwendungen müssen konkrete Implementierungen nicht kennen.
- Capabilities können dynamisch kombiniert werden.
- Hardware-, Software- und andere Provider bleiben austauschbar.
- Ausführung kann anhand von Bedeutung und Anforderungen geplant werden.

### Negative Konsequenzen

- Resolution und Ausführungsplanung werden komplexer.
- Semantic Contracts müssen präzise definiert sein.

## Abhängigkeiten

- `ADR-SEMANTIC-0002_Strong_Semantic_Typing`
- `ADR-SEMANTIC-0006_Typed_Capabilities`
- `ADR-SEMANTIC-0009_Semantic_Conversion`
- `ADR-SEMANTIC-0010_Semantic_Validation`
- `ADR-SEMANTIC-0011_Semantic_Discovery`
- `ADR-UPDATE-0009_Capability_Based_Dependency_Resolution`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-EXECUTION-0001`
- `NPSPEC-SEMANTIC-EXECUTION-PLAN-0001`
- `NPSPEC-SEMANTIC-EXECUTION-VALIDATION-0001`

## Ergebnis

NovaOS kann eine gewünschte Operation semantisch beschreiben und daraus einen gültigen Ausführungspfad bestimmen:

```text
What is needed?
      ↓
Semantic Resolution
      ↓
How can it be done?
      ↓
Provider Selection
      ↓
Execution
```

Der zentrale Grundsatz lautet:

```text
Der Nutzer oder Consumer beschreibt,
was erreicht werden soll.

NovaOS bestimmt,
wie es ausgeführt wird.
```