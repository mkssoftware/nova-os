# NPSPEC-STATETIME-0004 – State Snapshot Coordination

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie zusammengehörige Zustände mehrerer Objekte als konsistenter Snapshot erfasst werden.

Ziel ist, einen gültigen Systemzustand zu einem bestimmten logischen Zeitpunkt referenzierbar zu machen.

## Grundprinzip

```text
Object A State
Object B State
Object C State
      ↓
Snapshot Coordination
      ↓
Consistent Snapshot
```

## Snapshot

Ein Snapshot kann logisch beschrieben werden als:

```text
StateSnapshot {
    id
    timeline_position
    scope
    object_states
    consistency
}
```

Der Snapshot speichert oder referenziert die zugehörigen Temporal States.

## Scope

Ein Snapshot darf unterschiedliche Bereiche umfassen:

```text
OBJECT_SET
TASK
INTENT
WORKSPACE
SYSTEM
```

Nur Zustände innerhalb des definierten Scope müssen gemeinsam koordiniert werden.

## Konsistenz

Ein Snapshot darf nur als vollständig gültig gelten, wenn alle erforderlichen Zustände logisch zusammenpassen.

Mindestens folgende Zustände müssen unterscheidbar sein:

```text
CONSISTENT
PARTIAL
INVALID
```

Ein `PARTIAL` Snapshot darf nicht automatisch wie ein vollständig konsistenter Zustand behandelt werden.

## Laufende Änderungen

Während der Snapshot-Erstellung können Objekte weiter verändert werden.

NovaOS muss sicherstellen, dass der Snapshot trotzdem eine konsistente logische Sicht repräsentiert.

Geeignete Mechanismen können sein:

```text
version references
copy-on-write
transaction boundaries
logical barriers
```

Die konkrete Implementierung bleibt offen.

## Transaktionen

Nicht abgeschlossene Transaktionen dürfen nicht als vollständig bestätigter Zustand erscheinen.

```text
Transaction
    ├── Object A changed
    └── Object B pending
```

Der Snapshot muss entweder den Zustand vor oder nach der abgeschlossenen Transaktion darstellen.

## Verteilte Zustände

Sind mehrere Tasks oder Ausführungseinheiten beteiligt, muss keine globale Pause aller Komponenten erforderlich sein.

Entscheidend ist eine logisch konsistente Sicht der relevanten Zustände.

## Integration mit MicroCheckpoint

`State Snapshot` und `MicroCheckpoint` erfüllen unterschiedliche Aufgaben.

```text
StateTime Snapshot:
    konsistente historische Systemsicht

MicroCheckpoint:
    Wiederaufnahme einer laufenden Ausführung
```

Beide dürfen miteinander referenziert werden.

## Beispiel

```text
Snapshot:
    snapshot:500

Timeline:
    position:1200

States:
    document:42 → state:18
    project:7   → state:31
    settings:4  → state:9

Consistency:
    CONSISTENT
```

## Normative Anforderungen

1. NovaOS MUSS mehrere zusammengehörige Temporal States als Snapshot koordinieren können.
2. Jeder Snapshot MUSS einen eindeutig definierten Scope besitzen.
3. NovaOS MUSS mindestens `CONSISTENT`, `PARTIAL` und `INVALID` unterscheiden können.
4. Laufende Änderungen DÜRFEN keinen logisch widersprüchlichen Snapshot erzeugen.
5. Nicht abgeschlossene Transaktionen DÜRFEN nicht als vollständig bestätigter Snapshot-Zustand erscheinen.
6. Snapshots MÜSSEN eindeutig referenzierbar und einer Timeline-Position zuordenbar sein.
7. State Snapshots MÜSSEN mit MicroCheckpoints verknüpfbar sein können.

## Abgrenzung

Diese NPSPEC definiert:

- Snapshot-Koordination
- Snapshot Scope
- Konsistenz
- Behandlung paralleler Änderungen

Nicht Bestandteil sind:

- historische Abfragen
- Restore und Branching
- Snapshot-Retention
- MicroCheckpoint-Interna

## Zugehörige NPSPECs

- `NPSPEC-STATETIME-0001 – Temporal State Model`
- `NPSPEC-STATETIME-0002 – System State Timeline`
- `NPSPEC-STATETIME-0003 – Temporal Object Identity`
- `NPSPEC-STATETIME-0005 – Historical State Query`
- `NPSPEC-STATETIME-0006 – Temporal Restore & Branching`
- `NPSPEC-STATETIME-0007 – State Retention & Garbage Collection`
- `NPSPEC-CHECKPOINT-0004 – Checkpoint Consistency`