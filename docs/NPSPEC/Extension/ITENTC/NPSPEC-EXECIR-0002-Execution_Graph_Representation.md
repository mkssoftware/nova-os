# NPSPEC-EXECIR-0002 – Execution Graph Representation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Graphdarstellung der `Nova Execution IR`.

Ziel ist, Operationen, Datenflüsse und Ausführungsabhängigkeiten als gerichteten Graph darzustellen.

## Grundprinzip

```text
Input
    ↓
Node A
    ↓
Node B
    ├── Node C
    └── Node D
```

Der Graph beschreibt Abhängigkeiten und mögliche Parallelität, nicht die konkrete Scheduling-Reihenfolge.

## Graphmodell

Ein Execution Graph besteht aus:

```text
Nodes
Edges
Values
Entry Points
Outputs
```

Nodes repräsentieren Operationen.

Edges verbinden Daten- oder Kontrollabhängigkeiten.

## Datenfluss

Ein Output eines Nodes darf als Input anderer Nodes verwendet werden.

```text
Node A
    ↓ value:1
Node B
```

Ein Value darf von mehreren nachfolgenden Nodes gelesen werden.

```text
        ┌── Node B
value:1 ┤
        └── Node C
```

## Abhängigkeiten

Mindestens folgende Beziehungen müssen darstellbar sein:

```text
DATA
CONTROL
ORDER
RESOURCE
```

Eine `ORDER`-Abhängigkeit darf nur verwendet werden, wenn eine tatsächliche Reihenfolge erforderlich ist.

## Parallelität

Nodes ohne gegenseitige Abhängigkeit dürfen parallel ausführbar sein.

```text
Node A
    ├── Node B
    └── Node C
```

Der Graph soll keine künstliche Serialisierung erzeugen.

## Teilgraphen

Zusammengehörige Nodes dürfen zu logischen Teilgraphen gruppiert werden.

Beispiel:

```text
Subgraph:
    AudioProcessing
```

Teilgraphen können eigene:

```text
inputs
outputs
contracts
metadata
```

besitzen.

## Entry und Output

Der Graph muss seine externen Eingaben und Ergebnisse eindeutig beschreiben.

```text
Graph Input:
    Media.Audio

Graph Output:
    Document.Transcript
```

Interne Values bleiben innerhalb des Graphen.

## Zyklen

Der normale Execution Graph soll azyklisch sein.

Iterative Verarbeitung muss explizit modelliert werden, beispielsweise durch definierte Loop- oder Iteration-Nodes.

Unbeabsichtigte Zyklen sind ungültig.

## Beispiel

```text
value:input
    ↓
node:decode
    ↓ value:audio
node:transcribe
    ↓ value:text
node:store
```

Parallel:

```text
              ┌── node:transcribe
value:audio ──┤
              └── node:analyze
```

## Normative Anforderungen

1. Execution IR MUSS als gerichteter Graph darstellbar sein.
2. Daten- und Ausführungsabhängigkeiten MÜSSEN explizit repräsentierbar sein.
3. Nodes ohne gegenseitige Abhängigkeiten MÜSSEN als parallel ausführbar erkennbar sein.
4. Graph-Inputs und -Outputs MÜSSEN eindeutig definiert sein.
5. Teilgraphen MÜSSEN unterstützt werden können.
6. Unbeabsichtigte Zyklen MÜSSEN erkannt werden.
7. Der Graph DARF keine unnötige Ausführungsreihenfolge erzwingen.

## Abgrenzung

Diese NPSPEC definiert:

- Execution Graph
- Nodes und Edges
- Parallelität
- Teilgraphen
- Graph-Ein- und Ausgänge

Nicht Bestandteil sind:

- detaillierte Dataflow-Semantik
- Execution Contracts
- IR-Optimierung
- Scheduling
- Hardware-Lowering

## Zugehörige NPSPECs

- `NPSPEC-EXECIR-0001 – Execution IR Object Model`
- `NPSPEC-EXECIR-0003 – Dataflow & Dependency Semantics`
- `NPSPEC-EXECIR-0004 – Execution Contract Integration`
- `NPSPEC-EXECIR-0005 – IR Optimization & Transformation`
- `NPSPEC-EXECIR-0006 – Backend & Hardware Lowering`
- `NPSPEC-INTENTC-0004 – Intent-to-Execution Lowering`