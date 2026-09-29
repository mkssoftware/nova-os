# NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das grundlegende Objektmodell von `Nova.MicroCheckpoint`.

Ein MicroCheckpoint speichert den minimal notwendigen Zustand, um eine laufende Aufgabe nach Pause, Fehler oder Neustart möglichst genau fortsetzen zu können.

## Grundprinzip

```text
laufende Ausführung
    ↓
MicroCheckpoint
    ↓
Pause / Crash / Migration
    ↓
Restore
    ↓
Fortsetzung
```

MicroCheckpoints sind feingranularer als vollständige System- oder Prozess-Snapshots.

## MicroCheckpoint Object

Die logische Grundstruktur lautet:

```text
MicroCheckpoint {
    id
    owner
    execution_state
    data_state
    dependencies
    consistency
    timestamp
    metadata
}
```

## `id`

Jeder MicroCheckpoint besitzt eine eindeutige Identität.

Beispiel:

```text
checkpoint:01K...
```

Die Identität muss persistent referenzierbar sein.

## `owner`

`owner` beschreibt, zu welcher Ausführung der Checkpoint gehört.

Mögliche Referenzen:

```text
Intent
Task
Execution Graph
Capability
TaskCapsule
```

Beispiel:

```text
owner:
    intent:01JQX7...
```

## `execution_state`

`execution_state` enthält den Zustand, der zur Fortsetzung der Berechnung erforderlich ist.

Beispiele:

```text
current_node
instruction_state
logical_progress
pending_operations
continuation_state
```

Der Checkpoint soll nur den tatsächlich notwendigen Wiederaufnahmezustand enthalten.

## `data_state`

`data_state` beschreibt relevante Arbeitsdaten.

Beispiele:

```text
buffers
intermediate_results
state_objects
modified_pages
temporary_objects
```

Große unveränderte Daten sollen referenziert statt vollständig kopiert werden.

## Abhängigkeiten

Ein Checkpoint muss die für seine Wiederaufnahme erforderlichen Abhängigkeiten beschreiben können.

Beispiele:

```text
input_objects
capabilities
device_state
schema_versions
execution_contract
```

Beim Restore muss geprüft werden, ob diese Abhängigkeiten weiterhin gültig sind.

## Konsistenz

Ein Checkpoint besitzt einen definierten Konsistenzzustand.

Mindestens:

```text
COMPLETE
PARTIAL
INVALID
```

Nur ein gültiger und ausreichend vollständiger Checkpoint darf regulär für Resume verwendet werden.

## Safe Checkpoint

Ein MicroCheckpoint soll bevorzugt an einem sicheren Ausführungspunkt erzeugt werden.

Beispiel:

```text
Operation A
    ↓
SAFE POINT
    ↓
Operation B
```

An einem Safe Point dürfen keine unbekannten oder halb abgeschlossenen Zustandsänderungen bestehen, die eine sichere Wiederaufnahme verhindern.

## Inkrementeller Zustand

MicroCheckpoints dürfen inkrementell gespeichert werden.

Beispiel:

```text
Checkpoint A:
    Basiszustand

Checkpoint B:
    nur Änderungen seit A

Checkpoint C:
    nur Änderungen seit B
```

Dadurch wird Speicher- und Schreibaufwand reduziert.

## Externe Zustände

Nicht jeder Zustand kann direkt gespeichert werden.

Beispiele:

```text
network connection
remote transaction
hardware command
external service
```

Solche Abhängigkeiten müssen explizit beschrieben werden.

Beim Resume kann dadurch entschieden werden, ob:

```text
reconnect
revalidate
replay_safe
replan
fail
```

notwendig ist.

## Idempotenz

Noch nicht bestätigte Operationen müssen so dokumentiert sein, dass sie nach Restore nicht unbeabsichtigt doppelt ausgeführt werden.

Besonders relevant sind:

```text
external writes
messages
payments
device commands
irreversible actions
```

## Versionierung

MicroCheckpoints müssen die relevanten Versionen ihrer Umgebung speichern können.

Beispiele:

```text
schema_version
capability_version
execution_ir_version
checkpoint_format_version
```

Ein inkompatibler Checkpoint darf nicht ungeprüft wiederhergestellt werden.

## Lebenszyklus

Ein MicroCheckpoint kann mindestens folgende Zustände besitzen:

```text
CREATING
VALID
SUPERSEDED
INVALID
EXPIRED
```

`VALID` bedeutet, dass der Checkpoint zur Wiederaufnahme verwendet werden darf.

## Persistenz

Ein Checkpoint kann abhängig vom Zweck:

```text
memory_only
persistent
migratable
```

sein.

Nicht jeder kurzlebige Checkpoint muss dauerhaft auf Storage geschrieben werden.

## Beispiel

```text
MicroCheckpoint {
    id:
        checkpoint:2048

    owner:
        intent:01JQX7M8A4

    execution_state {
        current_node:
            node:17

        progress:
            0.64
    }

    data_state {
        modified_objects {
            object:buffer:12
            object:result:41
        }
    }

    dependencies {
        capability:
            nova.compute.solver@2

        input:
            object:dataset:8
    }

    consistency:
        COMPLETE
}
```

## Normative Anforderungen

1. Jeder MicroCheckpoint MUSS eindeutig referenzierbar sein.
2. Ein Checkpoint MUSS seinem zugehörigen Ausführungskontext zugeordnet werden können.
3. Nur für Resume notwendiger Zustand SOLL gespeichert werden.
4. Unveränderte große Daten SOLLEN referenziert statt dupliziert werden.
5. Checkpoints MÜSSEN ihren Konsistenzzustand beschreiben können.
6. Abhängigkeiten MÜSSEN beim Restore erneut prüfbar sein.
7. Nicht idempotente Operationen MÜSSEN vor unbeabsichtigter Doppel-Ausführung geschützt werden.
8. Checkpoint-Formate MÜSSEN versionierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- MicroCheckpoint-Objekt
- gespeicherten Ausführungs- und Datenzustand
- Abhängigkeiten
- Konsistenz
- grundlegenden Lifecycle

Nicht Bestandteil sind:

- Safe-Point-Erzeugung im Detail
- inkrementelle Speichermechanismen
- Crash-Recovery
- Retention
- Migration und Resume

## Zugehörige NPSPECs

- `NPSPEC-CHECKPOINT-0002 – Safe Checkpoint Points`
- `NPSPEC-CHECKPOINT-0003 – Incremental State Capture`
- `NPSPEC-CHECKPOINT-0004 – Checkpoint Consistency`
- `NPSPEC-CHECKPOINT-0005 – Crash Recovery`
- `NPSPEC-CHECKPOINT-0006 – Checkpoint Resource & Retention Policy`
- `NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`