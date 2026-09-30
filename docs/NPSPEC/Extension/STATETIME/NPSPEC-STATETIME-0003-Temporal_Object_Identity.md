# NPSPEC-STATETIME-0003 – Temporal Object Identity

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die zeitübergreifende Identität von Objekten in `Nova.StateTime`.

Ziel ist, verschiedene Zustände desselben logischen Objekts eindeutig miteinander zu verbinden.

## Grundprinzip

```text
Object Identity
    ├── State@t0
    ├── State@t1
    └── State@t2
```

Die Objektidentität bleibt bestehen, während sich der Zustand verändert.

## Identitätsmodell

Ein temporales Objekt kann logisch beschrieben werden als:

```text
TemporalObject {
    object_id
    state_id
    predecessor
    timeline_position
}
```

`object_id` identifiziert das logische Objekt.

`state_id` identifiziert einen konkreten historischen Zustand.

## Stabile Identität

Die Objektidentität darf nicht von kurzlebigen technischen Kennungen abhängen.

Nicht geeignet:

```text
memory_address
process_id
temporary_handle
```

Bevorzugt:

```text
object:document:42
```

## Zustandsidentität

Jeder bestätigte Zustand muss separat referenzierbar sein.

Beispiel:

```text
object:document:42

states:
    state:100
    state:101
    state:102
```

Damit kann NovaOS sowohl das Objekt als auch einen bestimmten historischen Zustand adressieren.

## Branches

Bei verzweigten Entwicklungen bleibt die ursprüngliche Abstammung erhalten.

```text
state:100
    ├── state:101
    └── state:102
```

Branches müssen eigene Zustandsidentitäten besitzen.

## Kopie und neues Objekt

Eine normale Kopie erzeugt grundsätzlich eine neue Objektidentität.

```text
object:A
    ↓ copy
object:B
```

Die Herkunft darf über Causality oder Lineage referenziert werden.

Eine historische Version desselben Objekts bleibt dagegen unter derselben logischen `object_id`.

## Beispiel

```text
object_id:
    object:document:42

state_id:
    state:102

predecessor:
    state:101
```

Damit ist eindeutig:

```text
welches Objekt
+
welcher Zustand
+
welcher Vorgänger
```

gemeint.

## Normative Anforderungen

1. Logische Objektidentität und konkrete Zustandsidentität MÜSSEN getrennt behandelbar sein.
2. Eine Objektidentität MUSS über Zustandsänderungen hinweg stabil bleiben.
3. Jeder persistente historische Zustand MUSS eindeutig referenzierbar sein.
4. Kurzlebige Prozess- oder Speicherkennungen DÜRFEN nicht als alleinige Objektidentität verwendet werden.
5. Branches MÜSSEN eigene Zustandsidentitäten besitzen.
6. Kopien MÜSSEN von historischen Zuständen desselben Objekts unterscheidbar sein.
7. Objekt- und Zustandsidentitäten MÜSSEN mit Timeline, Causality und Lineage verknüpfbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- stabile Objektidentität
- Zustandsidentität
- Branch-Identität
- Unterscheidung zwischen Kopie und Historie

Nicht Bestandteil sind:

- System State Timeline
- Snapshot-Koordination
- historische Abfragen
- Restore und Branching

## Zugehörige NPSPECs

- `NPSPEC-STATETIME-0001 – Temporal State Model`
- `NPSPEC-STATETIME-0002 – System State Timeline`
- `NPSPEC-STATETIME-0004 – State Snapshot Coordination`
- `NPSPEC-STATETIME-0005 – Historical State Query`
- `NPSPEC-STATETIME-0006 – Temporal Restore & Branching`
- `NPSPEC-STATETIME-0007 – State Retention & Garbage Collection`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`