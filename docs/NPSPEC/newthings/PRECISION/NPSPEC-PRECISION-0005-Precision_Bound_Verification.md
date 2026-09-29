# NPSPEC-PRECISION-0005 – Precision Bound Verification

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS überprüft, ob eine Berechnung die durch einen `Precision Contract` vorgegebenen Fehlergrenzen tatsächlich einhält.

Ziel ist, numerische Genauigkeit nicht nur zu planen, sondern nachweisbar zu kontrollieren.

## Grundprinzip

```text
Berechnung
    ↓
Ergebnis
    ↓
Precision Verification
    ↓
PASS / FAIL / UNKNOWN
```

## Verification Result

Eine Prüfung muss mindestens folgende Ergebnisse unterscheiden können:

```text
PASS
FAIL
UNKNOWN
```

### `PASS`

Die geforderte Fehlergrenze wurde nach dem verwendeten Verfahren eingehalten.

### `FAIL`

Die geforderte Fehlergrenze wurde verletzt.

### `UNKNOWN`

Die Einhaltung konnte nicht zuverlässig bestimmt werden.

`UNKNOWN` darf nicht automatisch als erfolgreich gelten.

## Verifikationsverfahren

NovaOS darf unterschiedliche Verfahren verwenden:

```text
ERROR_ESTIMATION
INTERVAL_CHECK
REFERENCE_COMPARISON
RECOMPUTATION
RESIDUAL_CHECK
ALGORITHM_SPECIFIC
```

Das gewählte Verfahren muss zur jeweiligen Operation und zum Precision Contract passen.

## Fehlergrenze

Die Verifikation muss dieselbe Semantik verwenden wie der zugrunde liegende Precision Contract.

Beispiele:

```text
absolute_error <= 0.001
```

```text
relative_error <= 0.01 %
```

```text
significant_digits >= 8
```

```text
EXACT
```

## Laufzeitprüfung

Precision Bounds dürfen während der Berechnung überprüft werden.

Beispiel:

```text
FP32
    ↓
Error Estimate exceeds bound
    ↓
Precision Escalation
    ↓
FP64
```

Dadurch kann eine ungeeignete Präzision früh erkannt werden.

## Endverifikation

Für relevante Ergebnisse darf zusätzlich eine abschließende Prüfung durchgeführt werden.

```text
Execution
    ↓
Final Result
    ↓
Bound Verification
    ↓
Accept Result
```

Ein zwingender Precision Contract darf nur dann als erfüllt gelten, wenn die erforderliche Verifikation erfolgreich abgeschlossen wurde.

## Mixed Precision

Bei Mixed-Precision-Ausführung muss die Gesamtwirkung aller relevanten Teilfehler berücksichtigt werden.

Beispiel:

```text
Input Conversion Error
    +
Node A Error
    +
Node B Error
    +
Output Conversion Error
    ↓
Total Error
```

Nicht jeder Einzelfehler muss separat unterhalb der finalen Grenze liegen, sofern das Gesamtfehlerbudget eingehalten wird.

## Exact Mode

Bei:

```text
precision_mode:
    EXACT
```

muss geprüft werden, dass keine unzulässige Approximation verwendet wurde.

Ein bloß kleiner numerischer Fehler reicht in diesem Modus nicht aus.

## Verification Failure

Bei Verletzung einer Fehlergrenze darf NovaOS abhängig vom Contract:

```text
RETRY_HIGHER_PRECISION
REPLAN
FAIL
DEGRADE_WITH_WARNING
```

ausführen.

`DEGRADE_WITH_WARNING` ist nur zulässig, wenn der Precision Contract dies ausdrücklich erlaubt.

## Unverifizierbare Ergebnisse

Kann keine zuverlässige Prüfung durchgeführt werden:

```text
verification:
    UNKNOWN
```

Mögliche Reaktionen:

```text
use_safer_precision
recompute
request_alternative_method
fail
```

NovaOS darf fehlende Verifikation nicht als bestätigte Genauigkeit darstellen.

## Verification Evidence

Eine Verifikation soll nachvollziehbar machen können:

```text
contract
method
estimated_error
required_bound
result
```

Beispiel:

```text
Verification {
    method:
        ERROR_ESTIMATION

    required:
        <= 0.001

    estimated:
        0.00042

    result:
        PASS
}
```

Diese Informationen können mit `Nova.Evidence` verknüpft werden.

## Determinismus

Bei deterministischer Ausführung muss auch die Verifikationsmethode reproduzierbar sein, soweit dies für das Ergebnis relevant ist.

Zufallsbasierte Prüfverfahren müssen dafür einen definierten deterministischen Modus unterstützen oder ausgeschlossen werden.

## Beispiel

Precision Contract:

```text
relative_error:
    <= 0.01 %
```

Ausführung:

```text
FP32
```

Verifikation:

```text
estimated_error:
    0.018 %

result:
    FAIL
```

Reaktion:

```text
FP32
    ↓
Precision Escalation
    ↓
FP64
```

Neue Verifikation:

```text
estimated_error:
    0.0004 %

result:
    PASS
```

## Normative Anforderungen

1. Zwingende Precision Bounds MÜSSEN verifizierbar sein.
2. NovaOS MUSS `PASS`, `FAIL` und `UNKNOWN` unterscheiden können.
3. `UNKNOWN` DARF nicht automatisch als erfolgreiche Verifikation gelten.
4. Die Verifikationsmethode MUSS zur Semantik des Precision Contracts passen.
5. Mixed-Precision-Verifikation MUSS den Gesamtfehler berücksichtigen können.
6. Verletzte Fehlergrenzen MÜSSEN eine definierte Reaktion auslösen.
7. Ergebnisse außerhalb zwingender Fehlergrenzen DÜRFEN nicht stillschweigend akzeptiert werden.
8. Verifikationsergebnisse SOLLEN nachvollziehbar dokumentiert werden können.

## Abgrenzung

Diese NPSPEC definiert:

- Prüfung von Precision Bounds
- Verification Results
- Laufzeit- und Endverifikation
- Verhalten bei Verifikationsfehlern

Nicht Bestandteil sind:

- Precision Cost Model
- automatische Präzisionsauswahl
- konkrete numerische Fehleralgorithmen
- Hardware Precision Mapping

## Zugehörige NPSPECs

- `NPSPEC-PRECISION-0001 – Precision Contract`
- `NPSPEC-PRECISION-0002 – Precision Cost Model`
- `NPSPEC-PRECISION-0003 – Dynamic Precision Selection`
- `NPSPEC-PRECISION-0004 – Mixed-Precision Execution`
- `NPSPEC-PRECISION-0006 – Hardware Precision Mapping`
- `NPSPEC-EVIDENCE-0001 – Evidence Bundle`