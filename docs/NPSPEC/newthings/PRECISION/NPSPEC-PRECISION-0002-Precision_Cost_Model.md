# NPSPEC-PRECISION-0002 – Precision Cost Model

## Status

Angenommen

## Zweck

Diese Spezifikation definiert ein Kostenmodell für numerische Präzision in NovaOS.

Ziel ist, die benötigte Genauigkeit gegen Ressourcenverbrauch abzuwägen, ohne den geltenden `Precision Contract` zu verletzen.

## Grundprinzip

```text
Precision Requirement
    +
Execution Options
    ↓
Precision Cost Model
    ↓
geeignete Ausführungsvariante
```

Höhere Präzision kann unter anderem mehr:

- Rechenzeit
- Speicher
- Energie
- Speicherbandbreite

benötigen.

NovaOS soll deshalb nicht automatisch immer die höchstmögliche Präzision verwenden.

## Cost Dimensions

Das Kostenmodell muss mindestens folgende Dimensionen berücksichtigen können:

```text
latency
compute
memory
bandwidth
energy
hardware_availability
conversion_overhead
```

Optional können weitere Kosten ergänzt werden.

## Precision Option

Eine Ausführungsoption kann logisch beschrieben werden als:

```text
PrecisionOption {
    representation
    estimated_error
    latency_cost
    memory_cost
    energy_cost
    conversion_cost
}
```

Beispiel:

```text
FP16:
    low memory
    low energy
    higher numeric error

FP32:
    medium memory
    medium energy
    medium numeric error

FP64:
    high memory
    high energy
    low numeric error
```

Diese Werte sind hardware- und operationsabhängig.

## Contract Filtering

Nur Optionen, die den `Precision Contract` erfüllen können, dürfen regulär berücksichtigt werden.

Beispiel:

```text
required_error:
    <= 0.001
```

Kandidaten:

```text
FP16:
    rejected

FP32:
    accepted

FP64:
    accepted
```

Das Cost Model entscheidet anschließend zwischen den gültigen Kandidaten.

## Mehrdimensionale Kosten

Kosten dürfen nicht auf einen einzigen Zahlenwert reduziert werden müssen.

Beispiel:

```text
Option A:
    faster
    more energy

Option B:
    slower
    less energy
```

Die Auswahl richtet sich nach den geltenden Execution Contracts und Policies.

## Gewichtung

Eine Ausführung darf Prioritäten für Kostenarten festlegen.

Beispiel:

```text
prefer:
    low_latency

secondary:
    low_energy
```

oder:

```text
prefer:
    low_energy
```

Das Cost Model liefert die Grundlage für diese Optimierung, trifft aber nicht zwingend allein die endgültige Entscheidung.

## Hardwareabhängigkeit

Kosten müssen pro Hardwareklasse unterschiedlich bewertet werden können.

Beispiel:

```text
FP64 on CPU:
    moderate cost

FP64 on GPU A:
    low cost

FP64 on GPU B:
    very high cost
```

Ein Precision Format besitzt daher keine universell festen Kosten.

## Conversion Cost

Wechsel zwischen Präzisionsformaten müssen berücksichtigt werden.

Beispiel:

```text
FP16
    ↓ convert
FP32
    ↓ compute
FP16
```

Die Konvertierung kann zusätzliche:

```text
latency
energy
memory traffic
numeric error
```

verursachen.

Mixed Precision darf nur gewählt werden, wenn der Gesamtpfad weiterhin sinnvoll ist.

## Datenmenge

Die Kosten können von der Datenmenge abhängen.

Beispiel:

```text
100 Werte:
    FP64 cost negligible

1 billion values:
    FP64 memory and bandwidth cost significant
```

Das Cost Model muss deshalb operations- und datenabhängig arbeiten können.

## Dynamische Aktualisierung

Kostenmodelle dürfen anhand tatsächlicher Ausführungsmessungen aktualisiert werden.

Beispiel:

```text
estimated latency:
    10 ms

actual latency:
    18 ms
```

Spätere Planungen dürfen diese Abweichung berücksichtigen.

Dabei dürfen Precision Contracts selbst nicht verändert werden.

## Determinismus

Für deterministische Ausführung muss das Cost Model einen reproduzierbaren Modus unterstützen können.

Unter identischen:

```text
hardware state
cost data
constraints
precision requirements
```

soll dieselbe Auswahlgrundlage entstehen.

## Beispiel

Precision Contract:

```text
error <= 0.01 %
```

Kandidaten:

```text
FP16 {
    estimated_error: 0.08 %
    energy: low
}

FP32 {
    estimated_error: 0.005 %
    energy: medium
}

FP64 {
    estimated_error: 0.00001 %
    energy: high
}
```

Ergebnis:

```text
FP16:
    rejected

FP32:
    valid

FP64:
    valid
```

Wenn keine zusätzliche höhere Genauigkeit verlangt wird, kann `FP32` wegen geringerer Kosten bevorzugt werden.

## Normative Anforderungen

1. Das Cost Model MUSS mehrere Kostenarten berücksichtigen können.
2. Optionen, die einen zwingenden Precision Contract nicht erfüllen, MÜSSEN ausgeschlossen werden.
3. Präzisionskosten MÜSSEN hardware- und operationsabhängig modellierbar sein.
4. Konvertierungskosten MÜSSEN bei Mixed Precision berücksichtigt werden können.
5. Tatsächliche Laufzeitdaten DÜRFEN zur Verbesserung zukünftiger Kostenschätzungen verwendet werden.
6. Das Cost Model DARF Precision Contracts nicht eigenständig abschwächen.
7. Kostenpräferenzen MÜSSEN von zwingenden Genauigkeitsanforderungen getrennt bleiben.

## Abgrenzung

Diese NPSPEC definiert:

- Kostenmodell für numerische Präzision
- Kostenarten
- Hardwareabhängigkeit
- Conversion Cost
- Laufzeitaktualisierung

Nicht Bestandteil sind:

- eigentliche Präzisionsauswahl
- Mixed-Precision-Planung
- Hardware-Lowering
- Verifikation von Fehlergrenzen

## Zugehörige NPSPECs

- `NPSPEC-PRECISION-0001 – Precision Contract`
- `NPSPEC-PRECISION-0003 – Dynamic Precision Selection`
- `NPSPEC-PRECISION-0004 – Mixed-Precision Execution`
- `NPSPEC-PRECISION-0005 – Precision Bound Verification`
- `NPSPEC-PRECISION-0006 – Hardware Precision Mapping`