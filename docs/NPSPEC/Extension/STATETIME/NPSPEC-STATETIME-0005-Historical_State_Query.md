# NPSPEC-STATETIME-0005 – Historical State Query

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie frühere Zustände von Objekten, Tasks oder Systembereichen über `Nova.StateTime` abgefragt werden.

Ziel ist, historische Informationen gezielt zu lesen, ohne den aktuellen Zustand verändern zu müssen.

## Grundprinzip

```text
Query
    ↓
StateTime
    ↓
Timeline / Snapshot
    ↓
Historical State
```

## Query

Eine historische Abfrage kann logisch enthalten:

```text
HistoricalStateQuery {
    target
    temporal_selector
    scope
}
```

## Zeitliche Auswahl

Ein historischer Zustand darf über unterschiedliche Selektoren angefordert werden.

Beispiele:

```text
state_id
snapshot_id
timeline_position
timestamp
```

Eindeutige Zustandskennungen sollen bevorzugt werden, wenn exakte Reproduzierbarkeit erforderlich ist.

## Objektabfrage

Ein konkreter Zustand eines Objekts kann direkt abgefragt werden.

Beispiel:

```text
object:
    document:42

state:
    state:18
```

Ergebnis:

```text
HistoricalState
```

## Systemabfrage

Auch ein größerer historischer Scope darf abgefragt werden.

Beispiele:

```text
TASK
INTENT
WORKSPACE
SYSTEM
```

Hierfür können koordinierte State Snapshots verwendet werden.

## Read-Only

Historische Abfragen verändern den aktuellen Zustand nicht.

```text
Historical Query
    ≠
Restore
```

Soll ein früherer Zustand wieder aktiv werden, erfolgt dies über `Temporal Restore`.

## Unvollständige Historie

Ein angeforderter Zustand kann aufgrund von Retention oder fehlenden Daten nicht mehr vollständig verfügbar sein.

Mindestens folgende Ergebnisse müssen unterscheidbar sein:

```text
FOUND
PARTIAL
NOT_FOUND
```

`PARTIAL` darf nicht als vollständige historische Rekonstruktion dargestellt werden.

## Causality

Historische Zustände dürfen mit Causality-Daten kombiniert werden.

Damit können beispielsweise Fragen beantwortet werden wie:

```text
Welcher Zustand bestand vorher?

Welche Änderung führte zu diesem Zustand?
```

Die eigentliche Ursachenanalyse bleibt Aufgabe von `Nova.Causality`.

## Beispiel

```text
query {
    target:
        object:document:42

    timeline_position:
        1200
}
```

Ergebnis:

```text
object:
    document:42

state:
    state:18

status:
    FOUND
```

## Normative Anforderungen

1. NovaOS MUSS historische Zustände über stabile temporale Referenzen abfragen können.
2. Objekt-, Snapshot- und Timeline-basierte Abfragen MÜSSEN unterstützt werden können.
3. Historische Abfragen DÜRFEN den aktuellen Zustand nicht verändern.
4. NovaOS MUSS mindestens `FOUND`, `PARTIAL` und `NOT_FOUND` unterscheiden können.
5. Teilweise rekonstruierte Zustände DÜRFEN nicht als vollständig dargestellt werden.
6. Historische Abfragen MÜSSEN bestehende Zugriffs- und Information-Flow-Regeln einhalten.
7. Ergebnisse MÜSSEN mit Causality- und Timeline-Informationen verknüpfbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- historische Zustandsabfragen
- zeitliche Selektoren
- Read-Only-Zugriff
- unvollständige Historie

Nicht Bestandteil sind:

- Restore
- Branching
- Snapshot-Erstellung
- Retention und Garbage Collection

## Zugehörige NPSPECs

- `NPSPEC-STATETIME-0001 – Temporal State Model`
- `NPSPEC-STATETIME-0002 – System State Timeline`
- `NPSPEC-STATETIME-0003 – Temporal Object Identity`
- `NPSPEC-STATETIME-0004 – State Snapshot Coordination`
- `NPSPEC-STATETIME-0006 – Temporal Restore & Branching`
- `NPSPEC-STATETIME-0007 – State Retention & Garbage Collection`
- `NPSPEC-CAUSAL-0005 – Causal Query Interface`