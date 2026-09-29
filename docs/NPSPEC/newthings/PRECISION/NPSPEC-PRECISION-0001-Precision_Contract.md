# NPSPEC-PRECISION-0001 – Precision Contract

## Status

Angenommen

## Zweck

Diese Spezifikation definiert Anforderungen an die rechnerische Präzision einer NovaOS-Ausführung.

Ein `Precision Contract` beschreibt, welche numerische Genauigkeit ein Ergebnis mindestens erreichen muss, ohne eine konkrete Datenrepräsentation oder Hardware vorzuschreiben.

## Grundprinzip

```text
Intent
    ↓
Precision Contract
    ↓
Nova.Compute
    ↓
geeignete Repräsentation und Hardware
    ↓
Ergebnis innerhalb der geforderten Grenzen
```

Beispiel:

```text
required_error:
    <= 0.01 %
```

NovaOS kann daraus abhängig von Aufgabe und Hardware beispielsweise wählen:

```text
Integer
FP16
BF16
FP32
FP64
Fixed Point
Arbitrary Precision
```

## Precision Contract

Die logische Grundstruktur lautet:

```text
PrecisionContract {
    target
    error_bound
    precision_mode
    verification
    fallback
}
```

Nicht jedes Feld ist für jede Berechnung erforderlich.

## Ziel

`target` beschreibt, für welchen Wert oder Output die Präzisionsanforderung gilt.

Beispiel:

```text
target:
    simulation.position
```

Mehrere Outputs dürfen unterschiedliche Anforderungen besitzen.

```text
position:
    error <= 1 mm

temperature:
    error <= 0.1 °C
```

## Fehlergrenzen

Ein Contract muss mindestens folgende Formen unterstützen können:

```text
ABSOLUTE
RELATIVE
SIGNIFICANT_DIGITS
EXACT
```

Beispiele:

```text
ABSOLUTE:
    error <= 0.001

RELATIVE:
    error <= 0.01 %

SIGNIFICANT_DIGITS:
    >= 8

EXACT:
    no numeric approximation
```

## Precision Mode

Zusätzlich darf die gewünschte Präzisionsstrategie beschrieben werden.

Beispiele:

```text
MINIMUM_REQUIRED
MAXIMUM_AVAILABLE
EXACT
ADAPTIVE
```

### `MINIMUM_REQUIRED`

NovaOS verwendet mindestens die Präzision, die zur Einhaltung des Contracts erforderlich ist.

### `MAXIMUM_AVAILABLE`

Es wird die sinnvoll höchste verfügbare Präzision verwendet.

### `EXACT`

Approximationen sind nicht zulässig.

### `ADAPTIVE`

NovaOS darf die Präzision während der Berechnung dynamisch verändern, solange der Contract eingehalten wird.

## Hardwareunabhängigkeit

Ein Precision Contract darf keine bestimmte Hardware voraussetzen, sofern dies nicht ausdrücklich Teil des Intents ist.

Beispiel:

```text
required_error:
    <= 0.001
```

statt:

```text
must_use:
    FP64 GPU
```

Die konkrete Umsetzung wird durch `Nova.Compute` bestimmt.

## Mixed Precision

Ein einzelner Execution Graph darf unterschiedliche Präzisionsstufen verwenden.

Beispiel:

```text
Input
    ↓ FP16
Preprocessing
    ↓ FP32
Critical Calculation
    ↓ FP64
Output
```

Voraussetzung ist, dass der Gesamtfehler innerhalb des Contracts bleibt.

## Unsicherheit

Rechenpräzision und fachliche Unsicherheit müssen getrennt behandelt werden.

Beispiel:

```text
Measurement:
    20.0 ± 0.5 °C

Numeric precision:
    ±0.0001 °C
```

Eine genauere Berechnung reduziert nicht automatisch die Unsicherheit der Eingangsdaten.

## Verification

Ein Contract darf festlegen, ob das Ergebnis nach der Berechnung überprüft werden muss.

Beispiel:

```text
verification:
    required
```

Mögliche Verfahren sind:

```text
error_estimation
reference_comparison
interval_check
recompute
```

Die konkrete Verifikationsmethode wird separat spezifiziert.

## Contract Violation

Kann die geforderte Präzision nicht erreicht werden, muss NovaOS dies erkennen.

Mögliche Reaktionen:

```text
RETRY_HIGHER_PRECISION
REPLAN
DEGRADE_WITH_WARNING
FAIL
```

Ein Ergebnis außerhalb zwingender Fehlergrenzen darf nicht stillschweigend als gültig gelten.

## Fallback

Ein Contract darf ein definiertes Verhalten bei Nichterfüllung enthalten.

Beispiel:

```text
fallback:
    REPLAN
```

oder:

```text
fallback:
    FAIL
```

Bei optionalen Anforderungen kann erlaubt sein:

```text
fallback:
    DEGRADE_WITH_WARNING
```

## Vererbung

Child-Intents und Execution Nodes dürfen Präzisionsanforderungen vom Parent übernehmen.

Sie dürfen die notwendige Genauigkeit erhöhen.

Eine Abschwächung ist nur zulässig, wenn dadurch der übergeordnete Precision Contract weiterhin erfüllt wird.

## Beispiel

```text
PrecisionContract {
    target:
        simulation.position

    error_bound {
        type:
            ABSOLUTE

        value:
            0.001

        unit:
            meter
    }

    precision_mode:
        ADAPTIVE

    verification:
        required

    fallback:
        REPLAN
}
```

NovaOS darf intern beispielsweise:

```text
FP32
```

verwenden.

Reicht dies nicht aus:

```text
FP32
    ↓ insufficient
FP64
```

Der Intent selbst bleibt unverändert.

## Normative Anforderungen

1. Precision Contracts MÜSSEN hardwareunabhängige Genauigkeitsanforderungen beschreiben können.
2. Absolute, relative und exakte Präzisionsanforderungen MÜSSEN darstellbar sein.
3. NovaOS DARF die konkrete numerische Repräsentation dynamisch wählen.
4. Mixed-Precision-Ausführung DARF verwendet werden, wenn der Gesamtvertrag erfüllt bleibt.
5. Rechenpräzision und fachliche Unsicherheit MÜSSEN getrennt behandelt werden.
6. Verletzungen zwingender Precision Contracts MÜSSEN erkannt werden.
7. Ein außerhalb zwingender Fehlergrenzen liegendes Ergebnis DARF nicht stillschweigend als gültig gelten.
8. Child-Operationen DÜRFEN Präzisionsanforderungen nur abschwächen, wenn der übergeordnete Contract weiterhin garantiert wird.

## Abgrenzung

Diese NPSPEC definiert:

- Precision Contracts
- Fehlergrenzen
- Precision Modes
- Mixed Precision
- grundlegendes Verhalten bei Contract-Verletzung

Nicht Bestandteil sind:

- Precision Cost Model
- automatische Präzisionsauswahl
- Hardware-Mapping
- konkrete Verifikationsalgorithmen
- Unsicherheitsfortpflanzung

## Zugehörige NPSPECs

- `NPSPEC-PRECISION-0002 – Precision Cost Model`
- `NPSPEC-PRECISION-0003 – Dynamic Precision Selection`
- `NPSPEC-PRECISION-0004 – Mixed-Precision Execution`
- `NPSPEC-PRECISION-0005 – Precision Bound Verification`
- `NPSPEC-PRECISION-0006 – Hardware Precision Mapping`
- `NPSPEC-UNCERTAINTY-0001 – Uncertainty Type System`
- `NPSPEC-EXECIR-0004 – Execution Contract Integration`