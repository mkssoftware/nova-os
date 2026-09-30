# NPSPEC-STATETIME-0001 – Temporal State Model

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das zeitbezogene Zustandsmodell von `Nova.StateTime`.

Ziel ist, System- und Objektzustände nicht nur als aktuellen Zustand, sondern als zeitlich nachvollziehbare Entwicklung darstellen zu können.

## Grundprinzip

```text
State(t0)
    ↓
State(t1)
    ↓
State(t2)
```

Ein Zustand besitzt eine zeitliche Position und eine Beziehung zu vorherigen oder nachfolgenden Zuständen.

## Temporal State

Ein zeitlicher Zustand kann logisch beschrieben werden als:

```text
TemporalState {
    object
    state
    timestamp
    predecessor
    version
}
```

Die konkrete Speicherung ist nicht Bestandteil dieser Spezifikation.

## Objektzustände

Ein Objekt darf mehrere zeitlich unterscheidbare Zustände besitzen.

Beispiel:

```text
Document@t0
    ↓ edit
Document@t1
    ↓ edit
Document@t2
```

Der aktuelle Zustand ist nur ein Punkt innerhalb dieser Zustandsfolge.

## Zeit und Reihenfolge

NovaOS muss zwischen:

```text
wall_clock_time
logical_order
```

unterscheiden können.

Die logische Reihenfolge darf verwendet werden, wenn reale Zeitstempel für eine eindeutige Zustandsordnung nicht ausreichen.

## Zustandsbeziehungen

Zustände müssen Beziehungen ausdrücken können wie:

```text
PREVIOUS
NEXT
DERIVED_FROM
BRANCHED_FROM
```

Dadurch können auch verzweigte Zustandsentwicklungen dargestellt werden.

## Unveränderlichkeit

Ein bestätigter historischer Zustand soll nicht nachträglich verändert werden.

Neue Änderungen erzeugen einen neuen Zustand.

```text
State A
    ↓ change
State B
```

statt:

```text
modify State A
```

## Integration

Temporal State muss mit anderen NovaOS-Konzepten verknüpfbar sein.

Beispiele:

```text
Causality
MicroCheckpoint
TaskCapsule
Undo
Evidence
```

Dabei bleibt `Nova.StateTime` für die zeitliche Zustandsidentität zuständig.

## Beispiel

```text
object:
    document:42

timeline:
    state:100 @ t0
        ↓
    state:101 @ t1
        ↓
    state:102 @ t2
```

Eine Abfrage kann damit gezielt einen früheren Zustand referenzieren.

## Normative Anforderungen

1. NovaOS MUSS mehrere zeitlich unterscheidbare Zustände desselben logischen Objekts darstellen können.
2. Jeder persistente Temporal State MUSS eindeutig referenzierbar sein.
3. Zeitliche Reihenfolge MUSS unabhängig von reinen Wall-Clock-Zeitstempeln darstellbar sein.
4. Zustände MÜSSEN Beziehungen zu vorherigen oder abgeleiteten Zuständen ausdrücken können.
5. Historische bestätigte Zustände SOLLEN unveränderlich behandelt werden.
6. Verzweigte Zustandsentwicklungen MÜSSEN darstellbar sein.
7. Temporal States MÜSSEN mit Causality, Checkpoints und Undo verknüpfbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- temporale Zustände
- zeitliche Zustandsidentität
- Zustandsreihenfolge
- grundlegende Zustandsbeziehungen

Nicht Bestandteil sind:

- vollständige System-Timeline
- Snapshot-Koordination
- historische Abfragen
- Restore und Branching
- Retention

## Zugehörige NPSPECs

- `NPSPEC-STATETIME-0002 – System State Timeline`
- `NPSPEC-STATETIME-0003 – Temporal Object Identity`
- `NPSPEC-STATETIME-0004 – State Snapshot Coordination`
- `NPSPEC-STATETIME-0005 – Historical State Query`
- `NPSPEC-STATETIME-0006 – Temporal Restore & Branching`
- `NPSPEC-STATETIME-0007 – State Retention & Garbage Collection`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`