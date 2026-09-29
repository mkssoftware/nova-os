# NPSPEC-UNCERTAINTY-0004 – Uncertain Comparison & Decision Semantics

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS unsichere Werte vergleicht und daraus Entscheidungen ableitet.

Ziel ist, dass Unsicherheit nicht durch scheinbar eindeutige Wahr/Falsch-Entscheidungen verloren geht.

## Grundprinzip

Ein Vergleich unsicherer Werte kann mehr als zwei Ergebnisse besitzen.

Statt nur:

```text
TRUE
FALSE
```

muss NovaOS mindestens unterscheiden können:

```text
TRUE
FALSE
UNCERTAIN
INDETERMINATE
```

## Vergleich

Beispiel:

```text
A = 10 ± 2
B = 11 ± 2
```

Die Aussage:

```text
A < B
```

ist nicht zwingend eindeutig.

Ergebnis:

```text
UNCERTAIN
```

Die Unsicherheitsbereiche überlappen.

## Vergleichsergebnis

Ein Vergleich kann logisch beschrieben werden als:

```text
ComparisonResult {
    state
    confidence
    evidence
}
```

`confidence` und `evidence` sind optional.

## Zustände

### `TRUE`

Der Vergleich gilt innerhalb der definierten Unsicherheitssemantik als ausreichend bestätigt.

### `FALSE`

Der Vergleich gilt als ausreichend widerlegt.

### `UNCERTAIN`

Mehrere Ergebnisse sind aufgrund vorhandener Unsicherheit möglich.

### `INDETERMINATE`

Eine Entscheidung ist nicht möglich, weil notwendige Unsicherheitsinformationen fehlen oder nicht kompatibel sind.

Beispiel:

```text
uncertainty:
    unknown
```

## Schwellenwerte

Entscheidungen dürfen Mindestanforderungen besitzen.

Beispiel:

```text
Decision {
    condition:
        temperature > 80°C

    required_confidence:
        0.99
}
```

Wird die erforderliche Sicherheit nicht erreicht, darf NovaOS die Bedingung nicht als eindeutig erfüllt behandeln.

## Sicherheitskritische Entscheidungen

Bei sicherheitskritischen Operationen muss die Entscheidungsregel explizit festgelegt werden.

Mögliche Strategien:

```text
FAIL_SAFE
FAIL_OPEN
REQUEST_MORE_DATA
DEFER
```

Beispiel:

```text
risk uncertain
    ↓
FAIL_SAFE
```

NovaOS darf Unsicherheit nicht willkürlich in eine günstige Entscheidung umwandeln.

## Vergleich von Bereichen

Intervalle können direkt verglichen werden.

Beispiel:

```text
A:
    10 .. 12

B:
    20 .. 25
```

Dann ist:

```text
A < B
```

eindeutig:

```text
TRUE
```

Bei:

```text
A:
    10 .. 20

B:
    15 .. 25
```

ist das Ergebnis:

```text
UNCERTAIN
```

## Gleichheit

Bei unsicheren Werten darf Gleichheit nicht grundsätzlich als bitweise oder exakt numerische Gleichheit interpretiert werden.

Mögliche Semantik:

```text
EXACT
OVERLAPS
WITHIN_TOLERANCE
STATISTICALLY_COMPATIBLE
```

Die verwendete Vergleichssemantik muss explizit sein.

## Entscheidungsregeln

Eine Decision Policy kann beschreiben:

```text
threshold
required_confidence
uncertainty_tolerance
fallback
```

Beispiel:

```text
DecisionPolicy {
    threshold:
        50

    required_confidence:
        0.95

    fallback:
        REQUEST_MORE_DATA
}
```

## Mehrere unsichere Eingaben

Entscheidungen dürfen von mehreren unsicheren Bedingungen abhängen.

Beispiel:

```text
temperature > 80°C
AND
pressure > 10bar
```

Die resultierende Unsicherheit muss aus allen relevanten Bedingungen abgeleitet werden.

## Sortierung

Unsichere Werte besitzen nicht immer eine eindeutige totale Reihenfolge.

Beispiel:

```text
A = 10 ± 5
B = 12 ± 5
```

NovaOS darf eine solche Reihenfolge nicht ohne definierte Policy als sicher darstellen.

Für Sortierung kann eine explizite Strategie verwendet werden, beispielsweise:

```text
nominal_value
lower_bound
upper_bound
expected_value
confidence_order
```

Die verwendete Strategie muss erkennbar bleiben.

## Fehlende Unsicherheit

Wenn ein Wert bekannte Unsicherheit besitzt und ein anderer Wert `uncertainty_unknown`, darf NovaOS den unbekannten Wert nicht automatisch als exakt behandeln.

Beispiel:

```text
A:
    10 ± 1

B:
    11
    uncertainty: unknown
```

Ein eindeutiger Vergleich kann daher unzulässig sein.

## Decision Provenance

Eine relevante Entscheidung soll nachvollziehbar machen können:

```text
welche Werte verwendet wurden
welche Unsicherheiten vorlagen
welche Vergleichsregel galt
welcher Schwellwert galt
welche Confidence erforderlich war
```

Diese Informationen können mit `Nova.Causality` und `Nova.Evidence` verknüpft werden.

## Beispiel

```text
Measurement:
    temperature = 79 ± 3 °C

Rule:
    temperature > 80 °C

Result:
    UNCERTAIN
```

Policy:

```text
fallback:
    REQUEST_MORE_DATA
```

NovaOS könnte daher eine zusätzliche Messung verlangen, anstatt die Schwelle als eindeutig über- oder unterschritten zu behandeln.

## Normative Anforderungen

1. Vergleiche unsicherer Werte MÜSSEN mehr als binäre Wahr/Falsch-Ergebnisse unterstützen können.
2. `UNCERTAIN` und `INDETERMINATE` MÜSSEN unterscheidbar sein.
3. Unsicherheit DARF bei Vergleichen nicht stillschweigend verworfen werden.
4. Sicherheitskritische Entscheidungen MÜSSEN eine definierte Unsicherheitsstrategie besitzen.
5. Gleichheits- und Sortiersemantik MÜSSEN explizit bestimmbar sein.
6. Werte mit unbekannter Unsicherheit DÜRFEN nicht automatisch als exakt behandelt werden.
7. Entscheidungsregeln SOLLEN erforderliche Confidence und Fallback-Verhalten definieren können.
8. Entscheidungsgrundlagen SOLLEN nachvollziehbar bleiben.

## Abgrenzung

Diese NPSPEC definiert:

- Vergleich unsicherer Werte
- mehrwertige Vergleichsergebnisse
- Decision Policies
- Schwellenwerte
- Sortierungs- und Gleichheitssemantik

Nicht Bestandteil sind:

- Unsicherheitsfortpflanzung
- konkrete statistische Algorithmen
- Serialisierung
- Benutzeroberfläche
- dynamische Rechenpräzision

## Zugehörige NPSPECs

- `NPSPEC-UNCERTAINTY-0001 – Uncertainty Type System`
- `NPSPEC-UNCERTAINTY-0002 – Confidence & Error Representation`
- `NPSPEC-UNCERTAINTY-0003 – Uncertainty Propagation`
- `NPSPEC-UNCERTAINTY-0005 – Uncertainty Serialization`
- `NPSPEC-UNCERTAINTY-0006 – Uncertainty Presentation`
- `NPSPEC-EVIDENCE-0005 – Evidence Query & Explanation`