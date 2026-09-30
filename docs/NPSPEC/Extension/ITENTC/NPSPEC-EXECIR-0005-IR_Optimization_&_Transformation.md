# NPSPEC-EXECIR-0005 – IR Optimization & Transformation

## Status

Angenommen

## Zweck

Diese Spezifikation definiert Optimierungen und Transformationen der `Nova Execution IR`.

Ziel ist, Ausführungskosten zu reduzieren, ohne Intent-Semantik, Abhängigkeiten oder Execution Contracts zu verletzen.

## Grundprinzip

```text
Execution IR
    ↓
Analysis
    ↓
Optimization / Transformation
    ↓
Equivalent Execution IR
```

## Optimierungen

Mögliche Optimierungen sind:

```text
dead_node_elimination
constant_folding
node_fusion
parallelization
dataflow_simplification
redundant_conversion_removal
```

Weitere Optimierungen dürfen ergänzt werden.

## Semantikerhalt

Eine Transformation darf die geforderte Bedeutung des Intents nicht verändern.

```text
IR before
    ≡
IR after
```

Abweichungen sind nur zulässig, wenn die geltenden Contracts sie ausdrücklich erlauben.

## Dependency Preservation

Transformationen müssen relevante Abhängigkeiten erhalten:

```text
DATA
CONTROL
ORDER
RESOURCE
```

Eine Optimierung darf notwendige Reihenfolgen oder Seiteneffektgrenzen nicht entfernen.

## Contract Preservation

Execution Contracts müssen auch nach einer Transformation erfüllt bleiben.

Beispiel:

```text
Node A + Node B
    ↓ fusion
Node C
```

`Node C` muss alle weiterhin relevanten Anforderungen von A und B erfüllen.

## Graph Transformation

Optimierungen dürfen:

```text
nodes hinzufügen
nodes entfernen
nodes zusammenführen
nodes aufteilen
edges verändern
```

sofern der resultierende Graph semantisch gültig bleibt.

## Optimierungsgrenzen

Optimierungen dürfen durch Anforderungen eingeschränkt werden.

Beispiele:

```text
deterministic
exact_precision
no_reordering
side_effect_boundary
checkpoint_boundary
```

Solche Grenzen müssen respektiert werden.

## Nachvollziehbarkeit

Transformierte Nodes sollen auf ihre ursprünglichen IR-Nodes zurückführbar bleiben.

```text
Original Nodes
    ↓
Transformation Record
    ↓
Optimized Node
```

Dies unterstützt Debugging, Evidence und Causality.

## Beispiel

Vorher:

```text
Node A:
    LOAD constant 4

Node B:
    LOAD constant 5

Node C:
    ADD A B
```

Nach Constant Folding:

```text
Node C:
    CONSTANT 9
```

Die beobachtbare Semantik bleibt identisch.

## Normative Anforderungen

1. IR-Optimierungen MÜSSEN die geforderte Intent-Semantik erhalten.
2. Relevante Daten-, Kontroll-, Reihenfolge- und Ressourcenabhängigkeiten MÜSSEN erhalten bleiben.
3. Execution Contracts DÜRFEN durch Optimierungen nicht verletzt werden.
4. Seiteneffekt- und Checkpoint-Grenzen MÜSSEN berücksichtigt werden.
5. Graphstrukturen DÜRFEN verändert werden, sofern die resultierende IR gültig bleibt.
6. Optimierungen MÜSSEN deaktivierbar oder begrenzbar sein können.
7. Transformierte IR SOLL auf ihre ursprüngliche Struktur zurückführbar bleiben.

## Abgrenzung

Diese NPSPEC definiert:

- IR-Optimierungen
- Graphtransformationen
- Semantikerhalt
- Contract- und Dependency-Erhalt

Nicht Bestandteil sind:

- konkrete Optimierungsalgorithmen
- Backend-Codegenerierung
- Hardwareauswahl
- Scheduling

## Zugehörige NPSPECs

- `NPSPEC-EXECIR-0001 – Execution IR Object Model`
- `NPSPEC-EXECIR-0002 – Execution Graph Representation`
- `NPSPEC-EXECIR-0003 – Dataflow & Dependency Semantics`
- `NPSPEC-EXECIR-0004 – Execution Contract Integration`
- `NPSPEC-EXECIR-0006 – Backend & Hardware Lowering`
- `NPSPEC-INTENTC-0004 – Intent-to-Execution Lowering`