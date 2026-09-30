# NPSPEC-EXECIR-0006 – Backend & Hardware Lowering

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie hardwareunabhängige `Execution IR` in eine konkrete Backend- und Hardwareausführung überführt wird.

Ziel ist, dieselbe IR abhängig von Anforderungen und verfügbarer Hardware auf geeignete Ausführungsziele abzubilden.

## Grundprinzip

```text
Execution IR
    ↓
Backend Selection
    ↓
Hardware Lowering
    ↓
CPU / GPU / NPU / Accelerator
```

## Backend-Auswahl

Geeignete Backends werden anhand von:

```text
supported_operations
hardware_capabilities
execution_contracts
precision
resources
trust
```

bestimmt.

Ein Backend darf nur gewählt werden, wenn alle zwingenden Anforderungen erfüllt werden.

## Hardware Mapping

Abstrakte IR-Operationen werden auf konkrete Hardwareoperationen abgebildet.

Beispiel:

```text
MATRIX_MULTIPLY
```

kann umgesetzt werden durch:

```text
CPU SIMD
GPU Kernel
NPU Operation
Accelerator
```

Die ursprüngliche semantische Operation bleibt dabei nachvollziehbar.

## Precision

Hardware-Lowering muss die Anforderungen aus `Nova.Precision` berücksichtigen.

Beispiel:

```text
required:
    FP32_or_better
```

Ein Backend mit ausschließlich:

```text
FP16
```

ist für diesen Pfad nicht ausreichend.

## Fallback

Ist das bevorzugte Backend nicht verfügbar, darf ein kompatibles alternatives Backend gewählt werden.

```text
GPU unavailable
    ↓
CPU backend
```

Hard Constraints dürfen dabei nicht verletzt werden.

## Datenbewegung

Backend-Lowering muss notwendige Datenbewegungen berücksichtigen können.

Beispiele:

```text
RAM → VRAM
VRAM → RAM
CPU → accelerator
```

Solche Transfers dürfen als explizite Ausführungsoperationen in die abgesenkte IR aufgenommen werden.

## Backend-spezifische IR

Nach dem Lowering darf eine backend-spezifische Zwischenrepräsentation entstehen.

```text
Execution IR
    ↓
GPU IR
    ↓
GPU Backend
```

oder:

```text
Execution IR
    ↓
CPU IR
    ↓
Machine Code
```

## Replanning

Fällt ein gewähltes Backend vor oder während der Ausführung aus, muss eine erneute Zuordnung möglich sein, sofern der Ausführungszustand dies erlaubt.

```text
Backend unavailable
    ↓
Replan
    ↓
Alternative Backend
```

## Beispiel

```text
IR Operation:
    MATRIX_MULTIPLY

Contract:
    precision >= FP32
    deadline <= 10ms

Available:
    CPU
    GPU
```

Auswahl:

```text
GPU
```

Lowering:

```text
MATRIX_MULTIPLY
    ↓
GPU Kernel
    ↓
GPU Execution
```

## Normative Anforderungen

1. Hardwareunabhängige Execution IR MUSS auf konkrete Backends abbildbar sein.
2. Backend-Auswahl MUSS Execution Contracts und Hardwarefähigkeiten berücksichtigen.
3. Hard Constraints DÜRFEN durch Backend-Lowering nicht verletzt werden.
4. Notwendige Datenbewegungen MÜSSEN explizit darstellbar sein.
5. Precision-Anforderungen MÜSSEN bei der Hardwareauswahl berücksichtigt werden.
6. Kompatible Fallback-Backends MÜSSEN unterstützt werden können.
7. Die Beziehung zwischen ursprünglicher IR und abgesenkter Ausführung MUSS nachvollziehbar bleiben.

## Abgrenzung

Diese NPSPEC definiert:

- Backend-Auswahl
- Hardware Mapping
- backend-spezifisches Lowering
- Fallback
- notwendige Datenbewegungen

Nicht Bestandteil sind:

- Hardwaretreiber
- konkrete Codegeneratoren
- Scheduling
- allgemeine Hardwareerkennung

## Zugehörige NPSPECs

- `NPSPEC-EXECIR-0001 – Execution IR Object Model`
- `NPSPEC-EXECIR-0002 – Execution Graph Representation`
- `NPSPEC-EXECIR-0003 – Dataflow & Dependency Semantics`
- `NPSPEC-EXECIR-0004 – Execution Contract Integration`
- `NPSPEC-EXECIR-0005 – IR Optimization & Transformation`
- `NPSPEC-PRECISION-0006 – Hardware Precision Mapping`