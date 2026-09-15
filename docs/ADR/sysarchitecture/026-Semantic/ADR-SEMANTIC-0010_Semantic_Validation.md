# ADR-SEMANTIC-0010 – Semantic Validation

## Status

Angenommen

## Kategorie

Semantic Architecture / Validation / Type Safety

## Kontext

Ein gültiger Semantic Type bedeutet nicht automatisch, dass ein konkreter Wert auch semantisch gültig ist.

Beispielsweise kann ein Wert technisch als `Temperature.Celsius` typisiert sein, aber außerhalb eines für einen bestimmten Contract zulässigen Bereichs liegen.

NovaOS benötigt deshalb neben der Typprüfung eine explizite semantische Validierung.

## Entscheidung

NovaOS unterstützt **Semantic Validation** auf Basis deklarierter Validation Contracts.

```text
Typed Value
    ↓
Semantic Validation
    ↓
Valid / Invalid
```

Die Validierung prüft nicht nur die technische Repräsentation, sondern semantische Regeln des jeweiligen Typs oder Contracts.

## Validation Rules

Semantic Types können Validierungsregeln definieren.

Beispiele:

```text
Range
Format
Unit
Precision
Required Metadata
Allowed Values
Relationships
```

Zusätzliche Regeln können durch einen Capability Contract festgelegt werden.

## Type und Validation

Typprüfung und Validierung bleiben getrennte Schritte.

```text
Type Check
    ↓
Semantic Validation
    ↓
Execution
```

Ein typkompatibler Wert kann trotzdem semantisch ungültig sein.

## Kontextabhängige Validierung

Validierungsregeln können vom jeweiligen Contract abhängen.

Beispiel:

```text
Semantic Type:
    Temperature.Celsius

Capability Contract:
    Allowed Range: -50 .. 150
```

Der Semantic Type bleibt gleich, während unterschiedliche Capabilities unterschiedliche Einschränkungen definieren können.

## Fehler

Eine fehlgeschlagene Validierung muss strukturiert beschreibbar sein.

```text
Validation Failed
 ├── Rule
 ├── Expected
 └── Actual
```

Dadurch können Anwendungen und Benutzer nachvollziehen, warum ein Wert abgelehnt wurde.

## Capabilities

Capabilities müssen Semantic Validation vor der Verarbeitung verlangen können.

```text
Typed Input
    ↓
Validate
    ↓
Capability
```

Ungültige Eingaben dürfen nicht stillschweigend korrigiert oder umgedeutet werden.

## Normative Anforderungen

1. NovaOS MUSS Semantic Validation unterstützen.
2. Semantic Types MÜSSEN Validierungsregeln definieren können.
3. Capability Contracts MÜSSEN zusätzliche Validierungsregeln festlegen können.
4. Typkompatibilität DARF nicht automatisch semantische Gültigkeit bedeuten.
5. Fehlgeschlagene Validierungen MÜSSEN strukturiert beschreibbar sein.
6. Ungültige Werte MÜSSEN an validierten Schnittstellen zurückgewiesen werden.
7. Validierung DARF Werte nicht stillschweigend semantisch verändern.

## Konsequenzen

### Positive Konsequenzen

- semantisch ungültige Daten werden früh erkannt,
- Capability Contracts werden zuverlässiger,
- Fehler können strukturiert erklärt werden,
- Datenqualität steigt systemweit.

### Negative Konsequenzen

- Validierungsregeln müssen gepflegt werden,
- zusätzliche Prüfungen verursachen geringen Laufzeitaufwand.

## Abhängigkeiten

- `ADR-SEMANTIC-0001_Semantic_Types_zusätzlich_zu_primitiven_Typen`
- `ADR-SEMANTIC-0002_Strong_Semantic_Typing`
- `ADR-SEMANTIC-0006_Typed_Capabilities`
- `ADR-SEMANTIC-0007_Semantic_Metadata`
- `ADR-SEMANTIC-0009_Semantic_Conversion`

## Zugehörige NPSPECs

- `NPSPEC-SEMANTIC-VALIDATION-0001`
- `NPSPEC-SEMANTIC-VALIDATION-CONTRACT-0001`

## Ergebnis

NovaOS prüft nicht nur, ob ein Wert den richtigen Semantic Type besitzt, sondern auch, ob er dessen Regeln erfüllt:

```text
Typed Value
    ↓
Type Check
    ↓
Semantic Validation
    ↓
Valid Data
```

Der zentrale Grundsatz lautet:

```text
Der richtige Typ allein
macht einen Wert
noch nicht semantisch gültig.
```