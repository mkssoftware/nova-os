# NPSPEC-UNCERTAINTY-0002 – Confidence & Error Representation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS Unsicherheit, Fehlergrenzen und Vertrauenswerte einheitlich repräsentiert.

Ziel ist, unterschiedliche Quellen von Unsicherheit vergleichbar und maschinenlesbar darzustellen.

## Grundprinzip

Ein Wert kann zusätzlich zu seinem eigentlichen Inhalt eine Unsicherheitsbeschreibung besitzen.

Beispiel:

```text
value:
    23.7 °C

uncertainty:
    ±0.4 °C
```

oder:

```text
prediction:
    "cat"

confidence:
    0.94
```

## Error Representation

Numerische Fehler müssen mindestens in folgenden Formen darstellbar sein:

```text
ABSOLUTE
RELATIVE
LOWER_UPPER
INTERVAL
```

Beispiele:

```text
ABSOLUTE:
    100 ± 2

RELATIVE:
    100 ± 2 %

LOWER_UPPER:
    +3 / -1

INTERVAL:
    98 .. 102
```

## Confidence Representation

Confidence beschreibt den Grad des Vertrauens in eine Aussage oder ein Ergebnis.

Logische Struktur:

```text
Confidence {
    value
    scale
    meaning
}
```

Beispiel:

```text
confidence {
    value: 0.94
    scale: 0..1
    meaning: model_estimate
}
```

Ein Confidence-Wert muss zusammen mit seiner Semantik interpretiert werden.

`0.95` darf nicht automatisch als statistisches 95-%-Konfidenzintervall verstanden werden.

## Confidence-Arten

Mindestens folgende Kategorien sollen unterscheidbar sein:

```text
STATISTICAL
MODEL
MEASUREMENT
ESTIMATED
SOURCE_REPORTED
UNKNOWN
```

Damit bleibt erkennbar, woher der Vertrauenswert stammt.

## Asymmetrische Fehler

Fehler müssen asymmetrisch darstellbar sein.

Beispiel:

```text
value:
    10.0

error:
    +0.8
    -0.3
```

NovaOS darf Fehler nicht grundsätzlich als symmetrisch annehmen.

## Grenzen

Unsicherheitsbereiche können explizite Unter- und Obergrenzen besitzen.

Beispiel:

```text
range {
    lower: 18.0
    upper: 22.0
}
```

Offene Grenzen müssen darstellbar sein.

Beispiel:

```text
lower:
    unknown

upper:
    100
```

## Mehrdimensionale Werte

Bei mehrdimensionalen Daten muss Unsicherheit pro Komponente oder gemeinsam dargestellt werden können.

Beispiel:

```text
Position {
    x: 10 ± 0.2m
    y: 20 ± 0.3m
}
```

Für korrelierte Größen darf zusätzlich eine gemeinsame Unsicherheitsstruktur verwendet werden.

Beispiel:

```text
covariance_matrix
```

Die mathematische Verarbeitung wird separat spezifiziert.

## Qualität der Unsicherheitsangabe

Eine Unsicherheitsangabe selbst darf eine Qualitätsklassifikation besitzen.

Beispiel:

```text
quality:
    measured
```

Mögliche Werte:

```text
measured
estimated
derived
reported
unknown
```

Damit kann NovaOS unterscheiden, ob eine Fehlergrenze gemessen oder lediglich geschätzt wurde.

## Herkunft

Confidence- und Error-Werte sollen auf ihre Quelle verweisen können.

Beispiel:

```text
source:
    sensor:temperature_01
```

oder:

```text
source:
    model:vision_classifier_v3
```

Die vollständige Provenienz kann über `Nova.Causality` und `Nova.Evidence` bereitgestellt werden.

## Fehlende Information

NovaOS muss unterscheiden zwischen:

```text
zero_error
unknown_error
not_applicable
not_provided
```

Beispiel:

```text
error:
    unknown
```

darf nicht wie:

```text
error:
    0
```

behandelt werden.

## Beispiel

```text
Uncertain<Measurement.Temperature> {
    value:
        23.7 °C

    error {
        type:
            ABSOLUTE

        lower:
            0.3 °C

        upper:
            0.4 °C
    }

    confidence {
        value:
            0.95

        type:
            MEASUREMENT
    }

    source:
        sensor:temp_01
}
```

## Normative Anforderungen

1. NovaOS MUSS absolute, relative, asymmetrische und intervallbasierte Fehler darstellen können.
2. Confidence und Error MÜSSEN getrennt modellierbar sein.
3. Confidence-Werte MÜSSEN ihre Skala und Bedeutung beschreiben können.
4. Fehlende Unsicherheitsinformation DARF nicht mit null Fehler gleichgesetzt werden.
5. Mehrdimensionale Werte MÜSSEN komponentenbezogene Unsicherheit darstellen können.
6. Herkunft und Qualität einer Unsicherheitsangabe SOLLEN referenzierbar sein.
7. Confidence DARF nicht automatisch als statistische Wahrscheinlichkeit interpretiert werden.

## Abgrenzung

Diese NPSPEC definiert:

- Error Representation
- Confidence Representation
- Fehlergrenzen
- Confidence-Arten
- Herkunft und Qualität

Nicht Bestandteil sind:

- Unsicherheitsfortpflanzung
- Entscheidungslogik
- konkrete Serialisierung
- UI-Darstellung

## Zugehörige NPSPECs

- `NPSPEC-UNCERTAINTY-0001 – Uncertainty Type System`
- `NPSPEC-UNCERTAINTY-0003 – Uncertainty Propagation`
- `NPSPEC-UNCERTAINTY-0004 – Uncertain Comparison & Decision Semantics`
- `NPSPEC-UNCERTAINTY-0005 – Uncertainty Serialization`
- `NPSPEC-UNCERTAINTY-0006 – Uncertainty Presentation`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`
- `NPSPEC-EVIDENCE-0002 – Source & Provenance References`