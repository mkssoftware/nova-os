# NPSPEC-CHECKPOINT-0004 – Checkpoint Consistency

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wann ein `Nova.MicroCheckpoint` als konsistent und sicher wiederherstellbar gilt.

Ziel ist, nur Zustände zu akzeptieren, die nach einem Restore eine logisch gültige Fortsetzung erlauben.

## Grundprinzip

```text
Captured State
    ↓
Consistency Validation
    ↓
VALID / PARTIAL / INVALID
```

Ein Checkpoint darf nicht allein deshalb als gültig gelten, weil seine Daten vollständig geschrieben wurden.

## Konsistenzzustände

Mindestens folgende Zustände müssen unterschieden werden:

```text
VALID
PARTIAL
INVALID
```

### `VALID`

Alle für Resume notwendigen Zustände und Abhängigkeiten sind konsistent.

### `PARTIAL`

Ein Teilzustand ist vorhanden, reicht aber nicht für eine vollständige Wiederaufnahme.

### `INVALID`

Der Checkpoint darf nicht für Resume verwendet werden.

## Konsistenzbereiche

Eine Prüfung muss mindestens folgende Bereiche berücksichtigen können:

- Execution State
- Datenzustand
- Objektversionen
- Abhängigkeiten
- Transaktionszustand
- externe Seiteneffekte
- Parent-/Child-Task-Zustände

Nicht jeder Checkpoint muss den gesamten Systemzustand umfassen.

## Konsistenzgrenze

Jeder Checkpoint muss definieren, welchen Bereich er konsistent repräsentiert.

Beispiele:

```text
single_task
task_group
intent
execution_graph
```

Ein lokaler Checkpoint darf nicht fälschlich als global konsistent dargestellt werden.

## Objektkonsistenz

Referenzierte Objekte müssen zu den erwarteten Versionen passen.

Beispiel:

```text
object:42
expected_version: 7
```

Wurde das Objekt zwischenzeitlich verändert, muss dies beim Restore erkannt werden.

## Transaktionskonsistenz

Ein Checkpoint darf keine unklaren halbfertigen Transaktionen enthalten.

Bevorzugt:

```text
Transaction
    ↓ COMMIT
Safe Point
    ↓
Checkpoint
```

Unbestätigte Transaktionen dürfen nur enthalten sein, wenn deren vollständiger Recovery-Zustand mitgespeichert wird.

## Externe Seiteneffekte

Für externe Wirkungen muss eindeutig erkennbar sein, ob sie:

```text
NOT_STARTED
PENDING
COMMITTED
FAILED
UNKNOWN
```

sind.

`UNKNOWN` darf nicht wie `NOT_STARTED` behandelt werden.

Dadurch wird verhindert, dass nicht idempotente Aktionen nach einem Restore versehentlich doppelt ausgeführt werden.

## Parallelität

Bei paralleler Ausführung müssen zusammengehörige Zustände eine gemeinsame konsistente Sicht bilden.

Beispiel:

```text
Task A: state 10
Task B: state 20
```

dürfen nur gemeinsam gespeichert werden, wenn diese Kombination tatsächlich gleichzeitig gültig war oder durch eine definierte Snapshot-Semantik erzeugt wurde.

## Inkrementelle Checkpoints

Bei Delta-Checkpoints hängt die Konsistenz auch von allen erforderlichen Basis-Checkpoints ab.

```text
Base A
    ↓
Delta B
    ↓
Delta C
```

Ist `B` ungültig, kann `C` nicht ohne zusätzlichen gültigen Wiederherstellungspfad als vollständig gültig gelten.

## Validierung

Eine Checkpoint-Prüfung kann logisch umfassen:

```text
validate_structure
validate_dependencies
validate_versions
validate_transactions
validate_external_effects
validate_continuation
```

Erst nach erfolgreicher Prüfung darf der Checkpoint als `VALID` bestätigt werden.

## Integrität

Konsistenz und Datenintegrität sind getrennte Eigenschaften.

Ein Checkpoint kann technisch unverändert gespeichert sein, aber semantisch inkonsistent.

Beispiel:

```text
checksum:
    valid

transaction_state:
    inconsistent
```

Ergebnis:

```text
INVALID
```

## Resume-Fähigkeit

Ein gültiger Checkpoint muss einen definierten Fortsetzungspunkt besitzen.

Beispiel:

```text
resume_at:
    execution_node:17
```

Fehlt eine sichere Continuation, darf der Checkpoint nicht als vollständig resume-fähig gelten.

## Beispiel

```text
Checkpoint {
    execution_state:
        node:17

    object_versions {
        object:42 = version:7
        object:81 = version:3
    }

    transactions:
        all_committed

    external_effects:
        none_pending

    continuation:
        valid
}
```

Ergebnis:

```text
consistency:
    VALID
```

## Normative Anforderungen

1. Ein Checkpoint MUSS seinen Konsistenzstatus eindeutig beschreiben können.
2. `VALID`, `PARTIAL` und `INVALID` MÜSSEN unterscheidbar sein.
3. Ein gültiger Checkpoint MUSS einen definierten Fortsetzungspunkt besitzen.
4. Objektversionen und relevante Abhängigkeiten MÜSSEN prüfbar sein.
5. Unklare externe Seiteneffekte MÜSSEN als solche erhalten bleiben.
6. Halbfertige Transaktionen DÜRFEN nicht ohne Recovery-Semantik als konsistent gelten.
7. Inkrementelle Checkpoints MÜSSEN die Konsistenz ihrer notwendigen Vorgänger berücksichtigen.
8. Datenintegrität allein DARF nicht als ausreichender Nachweis semantischer Konsistenz gelten.

## Abgrenzung

Diese NPSPEC definiert:

- Checkpoint-Konsistenz
- Konsistenzgrenzen
- Objekt- und Transaktionszustände
- externe Seiteneffekte
- Resume-Fähigkeit

Nicht Bestandteil sind:

- Safe-Point-Erzeugung
- Delta-Speicherung
- Crash-Recovery-Ablauf
- Retention
- Migration

## Zugehörige NPSPECs

- `NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model`
- `NPSPEC-CHECKPOINT-0002 – Safe Checkpoint Points`
- `NPSPEC-CHECKPOINT-0003 – Incremental State Capture`
- `NPSPEC-CHECKPOINT-0005 – Crash Recovery`
- `NPSPEC-CHECKPOINT-0006 – Checkpoint Resource & Retention Policy`
- `NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume`