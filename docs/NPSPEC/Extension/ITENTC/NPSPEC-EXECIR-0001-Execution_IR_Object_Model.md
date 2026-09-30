# NPSPEC-EXECIR-0001 – Execution IR Object Model

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das grundlegende Objektmodell der `Nova Execution IR`.

Die Execution IR beschreibt ausführungsrelevante Operationen, Datenflüsse und Abhängigkeiten in einer hardware- und backendneutralen Form.

## Grundprinzip

```text
Intent
    ↓
Intent Compiler
    ↓
Execution IR
    ↓
Optimization / Synthesis
    ↓
Backend Lowering
```

## IR Object

Die grundlegende Struktur kann logisch beschrieben werden als:

```text
ExecutionIR {
    id
    version
    nodes
    values
    dependencies
    contracts
    metadata
}
```

## Nodes

Ein IR Node repräsentiert eine ausführbare oder semantisch relevante Operation.

```text
IRNode {
    id
    operation
    inputs
    outputs
    attributes
}
```

Beispiele:

```text
LOAD
TRANSFORM
COMPUTE
STORE
CALL_CAPABILITY
```

Operationen dürfen erweitert werden.

## Values

Daten innerhalb der IR werden über eindeutige Values referenziert.

```text
value:1
value:2
value:3
```

Ein Value besitzt mindestens einen Semantic Type oder einen kompatiblen IR-Typ.

Beispiel:

```text
value:1:
    Media.Audio
```

## Inputs und Outputs

Nodes referenzieren ihre Daten explizit.

```text
node:1 {
    input:
        value:1

    output:
        value:2
}
```

Dadurch bleiben Datenfluss und Abhängigkeiten unabhängig von konkreten Speicheradressen.

## Identität

Nodes und Values müssen innerhalb einer IR eindeutig identifizierbar sein.

Identitäten dürfen nicht von:

```text
memory_address
process_id
hardware_handle
```

abhängen.

## Attribute

Nodes dürfen zusätzliche Eigenschaften besitzen.

Beispiele:

```text
deterministic
pure
parallelizable
checkpointable
side_effect
precision
```

Diese Attribute können spätere Optimierungs- und Ausführungsentscheidungen unterstützen.

## Execution Contracts

IR-Nodes oder Teilgraphen müssen mit `Nova.ExecutionContract` verknüpfbar sein.

Beispiel:

```text
node:42 {
    contract:
        contract:17
}
```

Dadurch bleiben Anforderungen wie:

```text
deadline
resource_budget
precision
trust
determinism
```

bis zur tatsächlichen Ausführung erhalten.

## Abstraktion

Die Execution IR beschreibt noch keine zwingende konkrete Hardwareausführung.

Beispiel:

```text
MATRIX_MULTIPLY
```

kann später auf:

```text
CPU
GPU
NPU
Accelerator
```

abgebildet werden.

## Versionierung

Das IR-Format muss versioniert sein.

```text
ExecutionIR@1
```

Unbekannte inkompatible Operationen oder Strukturen dürfen nicht stillschweigend ausgeführt werden.

## Beispiel

```text
ExecutionIR {
    node:1 {
        operation:
            LOAD

        output:
            value:1
    }

    node:2 {
        operation:
            TRANSCRIBE

        input:
            value:1

        output:
            value:2
    }

    node:3 {
        operation:
            STORE

        input:
            value:2
    }
}
```

## Normative Anforderungen

1. Execution IR MUSS Nodes, Values und Abhängigkeiten eindeutig darstellen können.
2. Nodes und Values MÜSSEN stabile IR-interne Identitäten besitzen.
3. Inputs und Outputs MÜSSEN explizit referenzierbar sein.
4. IR-Operationen MÜSSEN relevante Semantic Types erhalten können.
5. Execution Contracts MÜSSEN mit Nodes oder Teilgraphen verknüpfbar sein.
6. Die IR SOLL bis zum Backend-Lowering hardwareunabhängig bleiben.
7. Das IR-Format MUSS versionierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Execution-IR-Grundmodell
- Nodes
- Values
- Inputs und Outputs
- grundlegende Attribute

Nicht Bestandteil sind:

- Execution Graph im Detail
- Dataflow-Semantik
- IR-Optimierung
- Backend- und Hardware-Lowering

## Zugehörige NPSPECs

- `NPSPEC-EXECIR-0002 – Execution Graph Representation`
- `NPSPEC-EXECIR-0003 – Dataflow & Dependency Semantics`
- `NPSPEC-EXECIR-0004 – Execution Contract Integration`
- `NPSPEC-EXECIR-0005 – IR Optimization & Transformation`
- `NPSPEC-EXECIR-0006 – Backend & Hardware Lowering`
- `NPSPEC-INTENTC-0004 – Intent-to-Execution Lowering`