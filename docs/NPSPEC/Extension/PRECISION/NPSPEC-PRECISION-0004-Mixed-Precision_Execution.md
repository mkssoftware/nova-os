# NPSPEC-PRECISION-0004 – Mixed-Precision Execution

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS innerhalb einer Berechnung mehrere numerische Präzisionsstufen kombinieren darf.

Ziel ist, Rechenzeit, Energie- und Speicherbedarf zu senken, ohne den geltenden `Precision Contract` zu verletzen.

## Grundprinzip

```text
Execution Graph
    ↓
Node A: FP16
Node B: FP32
Node C: FP64
    ↓
Result
```

Nicht jede Teiloperation muss dieselbe Präzision verwenden.

## Mixed-Precision Plan

Ein Ausführungsplan darf jedem relevanten Node eine eigene Präzision zuweisen.

Beispiel:

```text
Input Decode:
    FP16

Preprocessing:
    FP32

Critical Solver:
    FP64

Output Conversion:
    FP32
```

Die Gesamtberechnung muss weiterhin die geforderte Ergebnisgenauigkeit erfüllen.

## Precision Boundaries

Wechselt die Präzision zwischen zwei Nodes, entsteht eine Precision Boundary.

Beispiel:

```text
FP16
    ↓ convert
FP32
```

An einer solchen Grenze müssen berücksichtigt werden:

```text
conversion_error
rounding
overflow
underflow
conversion_cost
```

## Upcasting

Eine Erhöhung der Präzision ist zulässig.

Beispiel:

```text
FP16 → FP32
FP32 → FP64
```

Upcasting stellt verlorene Information jedoch nicht automatisch wieder her.

Beispiel:

```text
FP16 value
    ↓ FP64
```

besitzt weiterhin nur die ursprünglich vorhandene Genauigkeit.

## Downcasting

Eine Verringerung der Präzision darf nur erfolgen, wenn der dadurch entstehende Fehler mit dem Precision Contract vereinbar ist.

Beispiel:

```text
FP64 → FP32
```

Vor dem Downcast muss geprüft werden können:

```text
range
precision_loss
rounding_error
overflow_risk
```

## Akkumulation

Bestimmte Operationen dürfen eine höhere Akkumulationspräzision verwenden als ihre Eingaben.

Beispiel:

```text
FP16 inputs
    ↓
FP32 accumulation
    ↓
FP16 output
```

Dies kann numerische Stabilität erhöhen, ohne alle Daten dauerhaft mit hoher Präzision zu speichern.

## Kritische Nodes

Ein Execution Graph darf Nodes markieren, bei denen eine höhere Präzision notwendig ist.

Beispiel:

```text
precision_role:
    CRITICAL
```

Andere mögliche Rollen:

```text
NORMAL
APPROXIMATE
CRITICAL
EXACT
```

Die konkrete Präzision wird weiterhin durch Precision Selection bestimmt.

## Fehlerbudget

Der übergeordnete Precision Contract darf auf Teiloperationen verteilt werden.

Beispiel:

```text
total_error_budget:
    0.001

Node A:
    0.0002

Node B:
    0.0003

Node C:
    0.0005
```

Teilbudgets dürfen dynamisch angepasst werden, solange das Gesamtbudget eingehalten wird.

## Conversion Minimization

Zu viele Precision-Wechsel können ineffizient sein.

Beispiel:

```text
FP16 → FP32 → FP16 → FP32 → FP64
```

NovaOS soll daher:

- unnötige Konvertierungen vermeiden
- benachbarte Nodes sinnvoll gruppieren
- Conversion Cost berücksichtigen

## Hardware

Mixed Precision darf Hardwarefähigkeiten ausnutzen.

Beispiele:

```text
CPU:
    FP32 / FP64

GPU:
    FP16 / FP32

NPU:
    INT8 / FP16
```

Der Execution Graph darf dadurch unterschiedliche Hardware- und Präzisionsbereiche kombinieren.

## Runtime Adaptation

NovaOS darf die Mixed-Precision-Aufteilung während der Ausführung ändern.

Beispiel:

```text
Node B:
    FP32

runtime error estimate:
    too high

new execution:
    FP64
```

Eine Anpassung darf keine bereits bestätigten ungültigen Ergebnisse weiterverwenden.

## Determinismus

Bei deterministischer Ausführung muss die Mixed-Precision-Konfiguration reproduzierbar sein.

Hardwareabhängige Unterschiede müssen berücksichtigt werden, wenn sie die numerischen Ergebnisse beeinflussen.

## Beispiel

```text
Execution Graph:

Input
    ↓ FP16

Normalization
    ↓ FP16

Matrix Operation
    ↓ FP32 accumulation

Critical Solver
    ↓ FP64

Output
    ↓ FP32
```

Precision Contract:

```text
relative_error:
    <= 0.01 %
```

NovaOS darf diese Konfiguration verwenden, wenn die abschließende Verifikation bestätigt, dass der Contract eingehalten wird.

## Normative Anforderungen

1. NovaOS MUSS unterschiedliche Präzisionsstufen innerhalb eines Execution Graph unterstützen können.
2. Precision-Wechsel MÜSSEN ihre möglichen numerischen Fehler berücksichtigen.
3. Downcasting DARF nur erfolgen, wenn der Precision Contract weiterhin eingehalten werden kann.
4. Höhere Akkumulationspräzision MUSS unabhängig von Input- und Output-Präzision möglich sein.
5. Mixed-Precision-Ausführung MUSS das gesamte Fehlerbudget berücksichtigen.
6. Unnötige Precision Conversions SOLLEN vermieden werden.
7. Runtime-Anpassungen DÜRFEN erfolgen, wenn sie den Precision Contract erhalten.
8. Deterministische Ausführung MUSS eine reproduzierbare Mixed-Precision-Konfiguration ermöglichen.

## Abgrenzung

Diese NPSPEC definiert:

- Mixed-Precision-Ausführung
- Precision Boundaries
- Upcasting und Downcasting
- Akkumulationspräzision
- Fehlerbudgets

Nicht Bestandteil sind:

- Precision Cost Model
- automatische Kandidatenauswahl
- detaillierte Fehlerverifikation
- Hardware-Lowering

## Zugehörige NPSPECs

- `NPSPEC-PRECISION-0001 – Precision Contract`
- `NPSPEC-PRECISION-0002 – Precision Cost Model`
- `NPSPEC-PRECISION-0003 – Dynamic Precision Selection`
- `NPSPEC-PRECISION-0005 – Precision Bound Verification`
- `NPSPEC-PRECISION-0006 – Hardware Precision Mapping`
- `NPSPEC-EXECIR-0005 – IR Optimization & Transformation`