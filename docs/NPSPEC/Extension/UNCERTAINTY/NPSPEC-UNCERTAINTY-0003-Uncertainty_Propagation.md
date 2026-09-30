# NPSPEC-UNCERTAINTY-0003 – Uncertainty Propagation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Unsicherheit bei Berechnungen, Transformationen und Ableitungen durch NovaOS weitergegeben wird.

Ziel ist, dass ein Ergebnis seine Unsicherheit nicht verliert, nur weil mehrere Verarbeitungsschritte dazwischenliegen.

## Grundprinzip

```text
Input mit Unsicherheit
    ↓
Operation
    ↓
Output mit abgeleiteter Unsicherheit
```

Beispiel:

```text
10.0 ± 0.2
    +
5.0 ± 0.1
    ↓
Resultat mit berechneter Unsicherheit
```

## Propagation Model

Eine Operation muss beschreiben können, wie Unsicherheit ihrer Inputs auf Outputs wirkt.

Logisch:

```text
OutputUncertainty =
    propagate(
        operation,
        input_values,
        input_uncertainties
    )
```

Die konkrete mathematische Methode hängt von der Operation und der Unsicherheitsform ab.

## Direkte Propagation

Für bekannte mathematische Operationen dürfen standardisierte Regeln verwendet werden.

Beispiele:

```text
Addition
Subtraktion
Multiplikation
Division
Transformation
Aggregation
```

NovaOS darf unterschiedliche mathematisch gültige Verfahren unterstützen.

## Capability-basierte Propagation

Capabilities müssen für relevante Outputs angeben können, wie deren Unsicherheit bestimmt wurde.

Beispiel:

```text
Capability {
    output:
        Measurement.Distance

    uncertainty:
        derived_from_inputs
}
```

Alternativ:

```text
uncertainty:
    measured
```

oder:

```text
uncertainty:
    model_estimated
```

## Mehrere Inputs

Ein Ergebnis darf von mehreren unsicheren Eingaben abhängen.

```text
Input A ± uncertainty_A ─┐
                         ├─ Operation ─→ Result ± uncertainty_R
Input B ± uncertainty_B ─┘
```

Alle kausal relevanten Unsicherheitsquellen müssen berücksichtigt werden können.

## Korrelation

NovaOS darf Unsicherheiten nicht grundsätzlich als unabhängig behandeln.

Wenn Eingaben korreliert sind, muss dies ausdrückbar sein.

Beispiel:

```text
correlation:
    shared_sensor
```

oder über eine mathematische Struktur wie:

```text
covariance_matrix
```

Fehlen Korrelationsinformationen, muss das verwendete Annahmemodell nachvollziehbar bleiben.

## Transformationen

Bei nichtlinearen oder komplexen Transformationen darf die Unsicherheit durch geeignete Verfahren bestimmt werden.

Beispiele:

```text
analytic
interval
sampling
simulation
model_specific
```

Die konkrete Methode darf durch `Nova.Math`, `Nova.Compute` oder die ausführende Capability bereitgestellt werden.

## Unknown Propagation

Wenn NovaOS die Unsicherheit eines Ergebnisses nicht zuverlässig bestimmen kann, darf sie nicht verworfen werden.

Stattdessen muss beispielsweise entstehen:

```text
uncertainty:
    unknown
```

oder:

```text
uncertainty:
    partially_known
```

Unsicherheit darf nicht stillschweigend zu `0` werden.

## Verlustbehaftete Operationen

Operationen können zusätzliche Unsicherheit erzeugen.

Beispiele:

```text
compression
sampling
quantization
prediction
approximation
sensor_conversion
```

Diese zusätzliche Unsicherheit muss zur bestehenden Unsicherheit hinzukommen oder separat referenzierbar bleiben.

## Unsicherheitsquellen

Ein Output darf mehrere Unsicherheitsquellen besitzen.

Beispiel:

```text
uncertainty_sources {
    sensor_noise
    calibration_error
    model_error
    quantization_error
}
```

Die Quellen können über `Nova.Causality` nachvollziehbar bleiben.

## Präzision

Unsicherheitsfortpflanzung und Rechenpräzision müssen getrennt behandelt werden.

Beispiel:

```text
measurement uncertainty:
    ±0.5

numeric error:
    ±0.00001
```

Die numerische Berechnung darf genauer sein als die zugrunde liegenden Daten.

`Nova.Precision` darf deshalb nicht automatisch die fachliche Unsicherheit reduzieren.

## Propagation über Intent-Grenzen

Unsicherheit muss auch über mehrere Intents und Capabilities hinweg erhalten bleiben.

Beispiel:

```text
Sensor
    ↓
Intent A
    ↓
Measurement ± uncertainty
    ↓
Intent B
    ↓
Prediction ± uncertainty
```

Ein neuer Verarbeitungsschritt darf vorhandene Unsicherheitsinformationen nicht unbegründet entfernen.

## Provenienz

Abgeleitete Unsicherheit soll nachvollziehbar machen können:

```text
welche Inputs beteiligt waren
welche Methode verwendet wurde
welche zusätzlichen Fehlerquellen entstanden
```

Diese Informationen können mit `Nova.Causality` und `Nova.Evidence` verknüpft werden.

## Beispiel

```text
Input A:
    20.0 ± 0.5

Input B:
    10.0 ± 0.2

Operation:
    difference

Output:
    10.0

Uncertainty:
    derived
```

Der genaue Unsicherheitswert wird entsprechend der definierten mathematischen Propagationsregel bestimmt.

## Normative Anforderungen

1. Unsicherheit MUSS über relevante Transformationen hinweg erhalten werden können.
2. Mehrere Unsicherheitsquellen MÜSSEN gemeinsam berücksichtigt werden können.
3. Unsicherheiten DÜRFEN nicht grundsätzlich als unabhängig angenommen werden.
4. Nicht berechenbare Unsicherheit MUSS als unbekannt oder unvollständig markiert werden.
5. Operationen DÜRFEN zusätzliche Unsicherheit erzeugen und müssen diese ausdrücken können.
6. Rechenpräzision und fachliche Unsicherheit MÜSSEN getrennt behandelt werden.
7. Unsicherheitsinformationen MÜSSEN über Intent- und Capability-Grenzen hinweg propagierbar sein.
8. Die verwendete Propagationsmethode SOLL nachvollziehbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Unsicherheitsfortpflanzung
- mehrere Unsicherheitsquellen
- Korrelation
- zusätzliche Fehlerquellen
- Propagation über Verarbeitungsketten

Nicht Bestandteil sind:

- konkrete mathematische Algorithmen
- Vergleich unsicherer Werte
- Entscheidungslogik
- Serialisierungsformat
- UI-Darstellung

## Zugehörige NPSPECs

- `NPSPEC-UNCERTAINTY-0001 – Uncertainty Type System`
- `NPSPEC-UNCERTAINTY-0002 – Confidence & Error Representation`
- `NPSPEC-UNCERTAINTY-0004 – Uncertain Comparison & Decision Semantics`
- `NPSPEC-UNCERTAINTY-0005 – Uncertainty Serialization`
- `NPSPEC-UNCERTAINTY-0006 – Uncertainty Presentation`
- `NPSPEC-PRECISION-0001 – Precision Contract`
- `NPSPEC-CAUSAL-0004 – Causality Propagation`