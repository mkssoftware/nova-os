# NPSPEC-EXECIR-0004 – Execution Contract Integration

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie `Nova.ExecutionContract` in die `Execution IR` eingebunden wird.

Ziel ist, Ausführungsanforderungen vom Intent bis zur tatsächlichen Ausführung unverändert und maschinenlesbar zu erhalten.

## Grundprinzip

```text
Intent Constraints
    ↓
Execution Contract
    ↓
Execution IR
    ↓
Backend / Scheduler
```

## Contract-Bindung

Execution Contracts dürfen an unterschiedliche IR-Ebenen gebunden werden:

```text
graph
subgraph
node
```

Beispiel:

```text
node:42 {
    contract:
        contract:17
}
```

## Contract-Inhalte

Ein Contract kann unter anderem Anforderungen enthalten für:

```text
determinism
deadline
latency
resource_budget
precision
trust
data_sovereignty
preferred_algorithm
forced_algorithm
```

Die Execution IR muss diese Anforderungen transportieren können.

## Vererbung

Contracts höherer Ebenen dürfen auf untergeordnete Nodes vererbt werden.

```text
Graph Contract
    ↓
Subgraph
    ↓
Node
```

Untergeordnete Contracts dürfen zwingende übergeordnete Anforderungen nicht abschwächen.

## Mehrere Contracts

Ein Node darf Anforderungen aus mehreren Contracts erhalten.

Diese müssen vor Ausführung zu einer konsistenten effektiven Anforderung zusammengeführt werden.

Widersprüche führen zu:

```text
CONTRACT_CONFLICT
```

## Transformation

IR-Optimierungen oder Transformationen müssen bestehende Contract-Anforderungen erhalten.

Beispiel:

```text
Node A + Node B
    ↓ fusion
Node C
```

`Node C` muss alle weiterhin relevanten Anforderungen von A und B erfüllen.

## Backend-Lowering

Beim Backend-Lowering werden abstrakte Contract-Anforderungen auf konkrete Ausführungsentscheidungen abgebildet.

Beispiel:

```text
precision:
    FP32_or_better

deadline:
    10ms
```

kann die Auswahl von:

```text
GPU
```

gegenüber:

```text
CPU
```

beeinflussen.

Die Contract-Anforderungen selbst bleiben dabei erhalten.

## Validierung

Vor Ausführung muss geprüft werden, ob die geplante Ausführung den wirksamen Contract erfüllt.

Mögliche Ergebnisse:

```text
SATISFIED
UNSATISFIED
CONFLICT
UNKNOWN
```

`UNKNOWN` darf bei zwingenden Anforderungen nicht automatisch als erfüllt gelten.

## Beispiel

```text
node:matrix_operation {
    operation:
        MATRIX_MULTIPLY

    contract {
        deterministic:
            true

        precision:
            FP32_or_better

        deadline:
            5ms
    }
}
```

Der Backend-Layer muss eine Ausführung wählen, die diese Anforderungen erfüllt.

## Normative Anforderungen

1. Execution IR MUSS `Nova.ExecutionContract` auf Graph-, Subgraph- und Node-Ebene referenzieren können.
2. Zwingende Contract-Anforderungen MÜSSEN bei IR-Transformationen erhalten bleiben.
3. Untergeordnete Contracts DÜRFEN übergeordnete Hard Constraints nicht abschwächen.
4. Konflikte zwischen Contracts MÜSSEN erkannt werden.
5. Backend- und Hardware-Auswahl MÜSSEN relevante Contract-Anforderungen berücksichtigen.
6. Vor Ausführung MUSS die Contract-Erfüllung prüfbar sein.
7. `UNKNOWN` DARF bei zwingenden Anforderungen nicht automatisch als `SATISFIED` gelten.

## Abgrenzung

Diese NPSPEC definiert:

- Contract-Bindung an Execution IR
- Contract-Vererbung
- Konflikterkennung
- Erhaltung bei IR-Transformationen
- Contract-Validierung

Nicht Bestandteil sind:

- Definition von `Nova.ExecutionContract`
- Scheduling-Algorithmen
- Precision-Implementierung
- Backend-Auswahl im Detail

## Zugehörige NPSPECs

- `NPSPEC-EXECIR-0001 – Execution IR Object Model`
- `NPSPEC-EXECIR-0002 – Execution Graph Representation`
- `NPSPEC-EXECIR-0003 – Dataflow & Dependency Semantics`
- `NPSPEC-EXECIR-0005 – IR Optimization & Transformation`
- `NPSPEC-EXECIR-0006 – Backend & Hardware Lowering`
- `NPSPEC-INTENTC-0004 – Intent-to-Execution Lowering`
- `NPSPEC-PRECISION-0001 – Precision Contract`