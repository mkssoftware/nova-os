# NPSPEC-UNCERTAINTY-0001 – Uncertainty Type System

## Status

Angenommen

## Zweck

Diese Spezifikation definiert Unsicherheit als systemweit verwendbaren Bestandteil semantischer Werte in NovaOS.

Ein Wert kann dadurch nicht nur einen Mess- oder Ergebniswert enthalten, sondern zusätzlich ausdrücken, wie sicher dieser Wert ist.

Beispiel:

```text
Temperature {
    value: 23.7 °C
    uncertainty: ±0.4 °C
}
```

## Grundprinzip

```text
Value
    +
Uncertainty
    =
Uncertain Value
```

Unsicherheit ist Teil der Bedeutung eines Wertes und darf nicht nur als frei formulierter Metadaten-Text behandelt werden.

## Uncertain Type

Ein unsicherer Wert kann logisch beschrieben werden als:

```text
Uncertain<T> {
    value
    uncertainty
    confidence
    provenance
}
```

`T` ist ein beliebiger kompatibler semantischer Typ.

Beispiele:

```text
Uncertain<Measurement.Temperature>
Uncertain<Measurement.Distance>
Uncertain<Prediction.Probability>
Uncertain<Location.Position>
```

## Unsicherheitsformen

NovaOS muss mindestens folgende grundlegende Formen unterscheiden können:

```text
ABSOLUTE
RELATIVE
RANGE
CONFIDENCE
DISTRIBUTION
UNKNOWN
```

### `ABSOLUTE`

```text
10.0 ± 0.2 mm
```

### `RELATIVE`

```text
100 W ± 2 %
```

### `RANGE`

```text
18 °C .. 22 °C
```

### `CONFIDENCE`

```text
classification:
    cat

confidence:
    0.94
```

### `DISTRIBUTION`

Für Werte, deren Unsicherheit durch eine Wahrscheinlichkeitsverteilung beschrieben wird.

Beispiel:

```text
mean: 100
stddev: 2.5
```

### `UNKNOWN`

Es ist bekannt, dass Unsicherheit vorhanden ist, aber keine quantitative Beschreibung verfügbar ist.

## Trennung von Wert und Unsicherheit

Der eigentliche semantische Typ bleibt erhalten.

Beispiel:

```text
Measurement.Temperature
```

wird nicht zu einem neuen fachlichen Typ.

Stattdessen entsteht:

```text
Uncertain<Measurement.Temperature>
```

Dadurch bleiben bestehende Semantic-Type-Beziehungen nutzbar.

## Einheiten

Bei physikalischen Größen muss die Unsicherheit mit der Einheit des zugrunde liegenden Wertes kompatibel sein.

Beispiel:

```text
value:
    23.7 °C

uncertainty:
    ±0.4 °C
```

Ungültig wäre ohne definierte Konvertierung:

```text
value:
    23.7 °C

uncertainty:
    ±3 m
```

## Confidence

`confidence` beschreibt die Sicherheit einer Aussage oder Schätzung.

Beispiel:

```text
confidence:
    0.95
```

Der Wertebereich und die genaue Semantik müssen durch den jeweiligen Typ oder das verwendete Modell definiert sein.

Ein Confidence-Wert darf nicht automatisch mit statistischer Wahrscheinlichkeit gleichgesetzt werden.

## Herkunft

Unsicherheit darf ihre Herkunft beschreiben.

Beispiele:

```text
sensor_accuracy
measurement_noise
model_prediction
estimated
user_provided
derived
```

Damit bleibt nachvollziehbar, warum ein Wert unsicher ist.

## Kombination mit Semantic Types

`Nova.Uncertainty` muss mit dem semantischen Typsystem kombinierbar sein.

Beispiel:

```text
Semantic Type:
    Measurement.Distance

Value:
    125.4 m

Uncertainty:
    ±0.1 m
```

Typkompatibilität wird weiterhin über `Measurement.Distance` bestimmt.

## Kombination mit Precision

Unsicherheit und Rechenpräzision sind getrennte Konzepte.

```text
Uncertainty:
    Wie sicher ist der zugrunde liegende Wert?

Precision:
    Wie genau wird dieser Wert technisch verarbeitet?
```

Beispiel:

```text
Measurement:
    23.7 ± 0.4 °C

Representation:
    Float64
```

Eine höhere technische Präzision reduziert nicht automatisch die Unsicherheit der Messung.

## Unbekannte Unsicherheit

Fehlt eine quantitative Unsicherheitsangabe, muss NovaOS zwischen:

```text
exact
uncertainty_unknown
uncertainty_not_applicable
```

unterscheiden können.

Fehlende Information darf nicht automatisch als perfekte Sicherheit interpretiert werden.

## Serialisierbarkeit

Unsichere Werte müssen gemeinsam mit ihrer Unsicherheitsbeschreibung serialisierbar sein.

Beispiel:

```text
Uncertain {
    type: Measurement.Temperature
    value: 23.7
    unit: celsius

    uncertainty {
        kind: ABSOLUTE
        value: 0.4
    }
}
```

## Beispiel

```text
Uncertain<Measurement.Position> {
    value {
        x: 125.2
        y: 41.8
    }

    uncertainty {
        kind: RANGE
        radius: 2.5m
    }

    provenance:
        gps_sensor
}
```

Damit weiß NovaOS nicht nur:

```text
Wo befindet sich das Objekt?
```

sondern zusätzlich:

```text
Wie sicher ist diese Position?
```

## Normative Anforderungen

1. NovaOS MUSS Unsicherheit als typisierten Bestandteil semantischer Werte darstellen können.
2. Wert und Unsicherheitsbeschreibung MÜSSEN logisch getrennt bleiben.
3. Unsicherheit MUSS mit bestehenden Semantic Types kombinierbar sein.
4. Physikalische Unsicherheiten MÜSSEN einheitenkompatibel sein.
5. Fehlende Unsicherheitsinformation DARF nicht automatisch als exakter Wert interpretiert werden.
6. Confidence MUSS von technischer Präzision unterschieden werden.
7. Unsichere Werte MÜSSEN vollständig serialisierbar sein.
8. Die Herkunft einer Unsicherheitsangabe SOLL referenzierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- grundlegendes Uncertainty Type System
- Unsicherheitsformen
- Kombination mit Semantic Types
- Trennung von Unsicherheit und Präzision

Nicht Bestandteil sind:

- mathematische Unsicherheitsfortpflanzung
- Vergleich unsicherer Werte
- Serialisierungsformat im Detail
- Darstellung in der Benutzeroberfläche
- dynamische Präzisionswahl

## Zugehörige NPSPECs

- `NPSPEC-UNCERTAINTY-0002 – Confidence & Error Representation`
- `NPSPEC-UNCERTAINTY-0003 – Uncertainty Propagation`
- `NPSPEC-UNCERTAINTY-0004 – Uncertain Comparison & Decision Semantics`
- `NPSPEC-UNCERTAINTY-0005 – Uncertainty Serialization`
- `NPSPEC-UNCERTAINTY-0006 – Uncertainty Presentation`
- `NPSPEC-PRECISION-0001 – Precision Contract`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`