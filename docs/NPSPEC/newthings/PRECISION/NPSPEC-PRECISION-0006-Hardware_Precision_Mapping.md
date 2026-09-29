# NPSPEC-PRECISION-0006 – Hardware Precision Mapping

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS abstrakte Präzisionsanforderungen auf konkrete Hardwarefähigkeiten abbildet.

Ziel ist, geeignete numerische Formate und Recheneinheiten auszuwählen, ohne den `Precision Contract` zu verletzen.

## Grundprinzip

```text
Precision Requirement
    +
Execution Operation
    +
Hardware Capabilities
    ↓
Hardware Precision Mapping
    ↓
konkretes Format + Recheneinheit
```

Beispiele:

```text
FP32 → CPU SIMD
FP16 → GPU
BF16 → NPU
INT8 → Accelerator
FP64 → CPU / GPU
```

## Hardware Capability Descriptor

Hardware muss ihre unterstützten Präzisionsformate beschreiben können.

Beispiel:

```text
HardwarePrecisionCapability {
    device
    formats[]
    native_formats[]
    emulated_formats[]
    throughput
    conversion_support
}
```

## Native und emulierte Präzision

NovaOS muss unterscheiden zwischen:

```text
NATIVE
EMULATED
UNSUPPORTED
```

### `NATIVE`

Das Format wird direkt durch die Hardware unterstützt.

### `EMULATED`

Das Format wird durch Software oder mehrere Hardwareoperationen nachgebildet.

### `UNSUPPORTED`

Das Format kann auf diesem Ausführungspfad nicht verwendet werden.

Emulation darf nur verwendet werden, wenn Precision Contract und Execution Contract weiterhin erfüllt werden können.

## Unterstützte Formate

Das Mapping muss unter anderem folgende Formate berücksichtigen können:

```text
Integer
Fixed Point
INT8
INT16
INT32
INT64
FP16
BF16
FP32
FP64
Extended Precision
Arbitrary Precision
```

Die Liste muss erweiterbar bleiben.

## Operationsabhängigkeit

Die Unterstützung eines Formats kann von der konkreten Operation abhängen.

Beispiel:

```text
GPU:
    FP16 matrix multiply → native
    FP16 division        → emulated
```

Hardware Precision Mapping darf deshalb nicht nur anhand des Datentyps erfolgen.

## Capability Matching

Für jeden relevanten Execution Node wird geprüft:

```text
required_precision
supported_format
operation_support
hardware_availability
precision_cost
```

Nur gültige Kombinationen dürfen als Mapping-Kandidaten verwendet werden.

## Mapping Result

Ein Mapping kann logisch beschrieben werden als:

```text
PrecisionMapping {
    execution_node
    device
    representation
    execution_mode
    conversions
}
```

Beispiel:

```text
Node:
    matrix.multiply

Device:
    GPU0

Representation:
    FP16

Accumulation:
    FP32
```

## Mixed Precision

Das Mapping muss unterschiedliche Input-, Rechen- und Akkumulationsformate unterstützen.

Beispiel:

```text
Input:
    FP16

Compute:
    FP16

Accumulation:
    FP32

Output:
    FP16
```

Diese Konfiguration gilt als eine gemeinsame Hardware-Mapping-Entscheidung.

## Conversion Mapping

Wenn Hardware ein benötigtes Format nicht direkt akzeptiert, darf eine Konvertierung eingefügt werden.

Beispiel:

```text
FP64 Input
    ↓ convert
FP32 GPU Compute
    ↓ convert
FP64 Output
```

Dies ist nur zulässig, wenn:

- der Precision Contract erfüllt bleibt
- Conversion Cost akzeptabel ist
- keine unzulässige Information verloren geht

## Hardwarewechsel

Ein Execution Graph darf Präzisionsbereiche auf unterschiedliche Geräte verteilen.

Beispiel:

```text
Preprocessing:
    CPU FP32

Matrix Compute:
    GPU FP16 + FP32 accumulation

Critical Solver:
    CPU FP64
```

Die Übergänge müssen in die Mixed-Precision- und Cost-Bewertung einfließen.

## Fallback

Ist ein gewünschtes Format nicht verfügbar, darf NovaOS alternative Mappings prüfen.

Beispiel:

```text
Preferred:
    GPU FP64

Unavailable

Fallback:
    CPU FP64
```

oder:

```text
Arbitrary Precision
    ↓
software backend
```

Der Fallback darf zwingende Precision Contracts nicht abschwächen.

## Geräteänderungen

Wird Hardware während der Ausführung:

```text
unavailable
degraded
removed
```

muss das Mapping neu bestimmt werden können.

Beispiel:

```text
GPU unavailable
    ↓
Re-Mapping
    ↓
CPU FP64
```

## Performance-Daten

Hardware Precision Mapping darf reale Laufzeitdaten berücksichtigen.

Beispiele:

```text
throughput
latency
energy
memory_bandwidth
conversion_cost
```

Diese Daten werden durch das `Precision Cost Model` bewertet.

## Hardwareeigenschaften

Zusätzliche Eigenschaften dürfen berücksichtigt werden:

```text
vector_width
tensor_units
rounding_modes
denormal_support
fused_operations
overflow_behavior
```

Solche Eigenschaften sind relevant, wenn sie Präzision oder Reproduzierbarkeit beeinflussen.

## Determinismus

Bei deterministischer Ausführung darf nur Hardware verwendet werden, deren numerisches Verhalten mit dem geforderten Determinismus vereinbar ist.

Beispiel:

```text
Device A:
    deterministic supported

Device B:
    result ordering nondeterministic
```

Bei einem deterministischen Contract darf `Device B` ausgeschlossen werden.

## Beispiel

Precision Contract:

```text
relative_error:
    <= 0.01 %
```

Operation:

```text
matrix.multiply
```

Verfügbare Hardware:

```text
CPU:
    FP32
    FP64

GPU:
    FP16
    FP32
    FP16 + FP32 accumulation

NPU:
    INT8
    FP16
```

Mögliche gültige Zuordnung:

```text
GPU

Input:
    FP16

Compute:
    FP16

Accumulation:
    FP32

Output:
    FP32
```

Wenn die Bound Verification bestätigt:

```text
error <= 0.01 %
```

kann dieses Mapping verwendet werden.

## Normative Anforderungen

1. NovaOS MUSS abstrakte Precision Requirements auf konkrete Hardwareformate abbilden können.
2. Native, emulierte und nicht unterstützte Formate MÜSSEN unterscheidbar sein.
3. Hardwareunterstützung MUSS operationsabhängig beschrieben werden können.
4. Input-, Compute-, Accumulation- und Output-Präzision MÜSSEN getrennt abbildbar sein.
5. Hardwarewechsel DÜRFEN Precision Contracts nicht abschwächen.
6. Eingefügte Konvertierungen MÜSSEN in Fehler- und Kostenbewertung einfließen.
7. Nicht verfügbare Hardware MÜSSEN Re-Mapping auslösen können.
8. Deterministische Contracts MÜSSEN hardwarebedingte numerische Unterschiede berücksichtigen.

## Abgrenzung

Diese NPSPEC definiert:

- Hardware Precision Mapping
- native und emulierte Formate
- operationsabhängige Hardwarefähigkeiten
- Precision Mapping über mehrere Geräte
- Hardware-Fallback

Nicht Bestandteil sind:

- Precision Cost Model
- eigentliche Precision Selection
- Mixed-Precision-Fehleranalyse
- konkrete Gerätetreiber
- Hardware Scheduling

## Zugehörige NPSPECs

- `NPSPEC-PRECISION-0001 – Precision Contract`
- `NPSPEC-PRECISION-0002 – Precision Cost Model`
- `NPSPEC-PRECISION-0003 – Dynamic Precision Selection`
- `NPSPEC-PRECISION-0004 – Mixed-Precision Execution`
- `NPSPEC-PRECISION-0005 – Precision Bound Verification`
- `NPSPEC-EXECIR-0006 – Backend & Hardware Lowering`