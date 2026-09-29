# NPSPEC-PRECISION-0003 – Dynamic Precision Selection

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS die numerische Präzision einer Berechnung automatisch auswählt.

Ziel ist, nur so viel Präzision zu verwenden wie erforderlich, um den geltenden `Precision Contract` sicher einzuhalten.

## Grundprinzip

```text
Precision Contract
    +
Cost Model
    +
Operation
    +
Hardware
    ↓
Precision Selection
    ↓
geeignete numerische Repräsentation
```

Mögliche Repräsentationen:

```text
Integer
Fixed Point
FP16
BF16
FP32
FP64
Arbitrary Precision
```

## Auswahl

NovaOS muss zunächst alle Präzisionsoptionen bestimmen, die:

- von der Operation unterstützt werden
- auf verfügbarer Hardware ausführbar sind
- den Precision Contract voraussichtlich erfüllen
- bestehende Execution Contracts und Policies einhalten

Nicht gültige Kandidaten werden ausgeschlossen.

## Selection Policy

Die Auswahl darf unter anderem optimieren nach:

```text
latency
energy
memory
bandwidth
throughput
conversion_cost
```

Zwingende Genauigkeitsanforderungen haben dabei Vorrang vor Optimierungszielen.

## Minimum Required Precision

Standardmäßig soll NovaOS die kostengünstigste Präzision wählen, die den Contract zuverlässig erfüllt.

Beispiel:

```text
FP16:
    insufficient

FP32:
    sufficient

FP64:
    sufficient
```

Auswahl:

```text
FP32
```

sofern keine andere Anforderung `FP64` notwendig macht.

## Adaptive Selection

Bei `ADAPTIVE` darf NovaOS die Präzision während der Berechnung verändern.

Beispiel:

```text
Start:
    FP32

Error Estimate:
    too high

Escalation:
    FP64
```

oder:

```text
Start:
    FP64

Error Margin:
    far below requirement

Later Stage:
    FP32
```

Eine Reduzierung ist nur zulässig, wenn der Contract weiterhin eingehalten werden kann.

## Precision Escalation

Ist eine gewählte Präzision unzureichend, muss NovaOS auf eine genauere Repräsentation wechseln können.

Beispiel:

```text
FP16
    ↓ insufficient
FP32
    ↓ insufficient
FP64
```

Kann keine verfügbare Variante den Contract erfüllen, wird die definierte Fallback-Strategie verwendet.

## Inputabhängigkeit

Die notwendige Präzision darf von den konkreten Eingabedaten abhängen.

Beispiel:

```text
gleicher Algorithmus
    +
unterschiedliche Wertebereiche
    ↓
unterschiedlicher Precision Bedarf
```

NovaOS darf deshalb statische und laufzeitabhängige Auswahl kombinieren.

## Fehlerabschätzung

Die Auswahl kann auf Fehlerabschätzungen beruhen.

Beispiele:

```text
static_error_bound
runtime_error_estimate
interval_analysis
reference_result
operation_profile
```

Die verwendete Abschätzung muss zum Precision Contract passen.

## Hardwarewechsel

Wenn eine andere Hardware die geforderte Präzision effizienter bereitstellt, darf NovaOS den Ausführungspfad entsprechend wählen.

Beispiel:

```text
CPU FP64
GPU FP32
NPU FP16
```

Die Auswahl erfolgt nur unter Einhaltung aller relevanten Contracts.

## Replanning

Ändern sich Hardware, Ressourcen oder Fehlerabschätzungen, darf die Precision Selection neu durchgeführt werden.

Beispiel:

```text
GPU unavailable
    ↓
Replan
    ↓
CPU FP64
```

Das ursprüngliche Genauigkeitsziel bleibt unverändert.

## Determinismus

Bei deterministischen Execution Contracts muss die Auswahl reproduzierbar sein.

Adaptive Entscheidungen dürfen nur auf deterministisch zulässigen Eingaben und Messwerten beruhen.

## Beispiel

```text
PrecisionContract {
    error:
        <= 0.001

    mode:
        ADAPTIVE
}
```

Kandidaten:

```text
FP16:
    estimated_error = 0.01

FP32:
    estimated_error = 0.0008

FP64:
    estimated_error = 0.000001
```

Auswahl:

```text
FP32
```

Falls die Laufzeitprüfung später ergibt:

```text
actual_error_estimate:
    0.0014
```

erfolgt:

```text
FP32
    ↓ precision escalation
FP64
```

## Normative Anforderungen

1. Dynamic Precision Selection MUSS den geltenden Precision Contract einhalten.
2. Nicht ausreichend genaue Kandidaten MÜSSEN ausgeschlossen werden.
3. Zwingende Genauigkeitsanforderungen MÜSSEN Vorrang vor Kostenoptimierung haben.
4. NovaOS MUSS bei unzureichender Präzision auf eine genauere Variante wechseln können.
5. Präzision DARF während der Ausführung angepasst werden, wenn der Contract erhalten bleibt.
6. Hardware- und datenabhängige Auswahl MUSS unterstützt werden können.
7. Kann kein Kandidat den Contract erfüllen, MUSS die definierte Fallback-Strategie ausgelöst werden.
8. Deterministische Ausführung MUSS eine reproduzierbare Precision Selection ermöglichen.

## Abgrenzung

Diese NPSPEC definiert:

- automatische Präzisionsauswahl
- Precision Escalation
- adaptive Auswahl
- Replanning

Nicht Bestandteil sind:

- Precision Cost Model
- detaillierte Mixed-Precision-Graphplanung
- Fehlergrenzen-Verifikation
- Hardware-Lowering

## Zugehörige NPSPECs

- `NPSPEC-PRECISION-0001 – Precision Contract`
- `NPSPEC-PRECISION-0002 – Precision Cost Model`
- `NPSPEC-PRECISION-0004 – Mixed-Precision Execution`
- `NPSPEC-PRECISION-0005 – Precision Bound Verification`
- `NPSPEC-PRECISION-0006 – Hardware Precision Mapping`