# NPSPEC-CHECKPOINT-0003 – Incremental State Capture

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie `Nova.MicroCheckpoint` nur die seit einem vorherigen Checkpoint veränderten Zustände speichert.

Ziel ist, Checkpoints schnell und ressourcenschonend zu erzeugen, ohne bei jeder Sicherung den vollständigen Ausführungszustand kopieren zu müssen.

## Grundprinzip

```text
Checkpoint A
    ↓
Änderungen
    ↓
Checkpoint B = Delta zu A
    ↓
Änderungen
    ↓
Checkpoint C = Delta zu B
```

## Basis und Delta

Ein inkrementeller Checkpoint besteht aus:

```text
Base Checkpoint
    +
State Delta
```

Das Delta enthält nur Zustände, die sich seit der referenzierten Basis verändert haben.

Beispiele:

- geänderte Speicherbereiche
- neue oder veränderte Objekte
- aktualisierte Execution States
- geänderte Metadaten
- neue Zwischenergebnisse

## Checkpoint-Kette

Inkrementelle Checkpoints dürfen voneinander abhängen.

Beispiel:

```text
Full A
    ↓
Delta B
    ↓
Delta C
    ↓
Delta D
```

Zur Wiederherstellung von `D` werden die erforderlichen Vorgänger angewendet.

Die Abhängigkeitskette muss eindeutig referenzierbar sein.

## State Delta

Ein Delta kann logisch beschrieben werden als:

```text
StateDelta {
    base_checkpoint
    changed_state
    added_state
    removed_state
    metadata
}
```

Nicht veränderte Daten müssen nicht erneut gespeichert werden.

## Änderungsverfolgung

NovaOS darf Änderungen unter anderem erkennen durch:

```text
dirty pages
object versioning
write tracking
copy-on-write
explicit capability reporting
```

Die konkrete Methode darf je nach Subsystem unterschiedlich sein.

## Objektzustände

Bei objektbasierten Daten soll nach Möglichkeit auf Objektidentität und Versionen zurückgegriffen werden.

Beispiel:

```text
object:42
version: 5
    ↓ modified
version: 6
```

Der Checkpoint kann dann nur die neue oder geänderte Version referenzieren.

## Große Daten

Große unveränderte Daten sollen weiterhin referenziert werden.

Beispiel:

```text
Dataset:
    unchanged
    → reference existing object
```

Nur tatsächlich veränderte Bereiche werden Bestandteil des Deltas.

## Kettenlänge

Sehr lange Delta-Ketten können Restore-Zeit und Fehleranfälligkeit erhöhen.

NovaOS darf deshalb periodisch einen neuen vollständigen oder konsolidierten Basis-Checkpoint erzeugen.

Beispiel:

```text
Full A
↓
Delta B
↓
Delta C
↓
Delta D
↓
Consolidated Full E
```

Die konkrete Schwelle wird durch die Retention- und Ressourcenpolitik bestimmt.

## Konsistenz

Ein Delta darf nur auf einen gültigen und kompatiblen Basis-Checkpoint verweisen.

Fehlt ein notwendiger Vorgänger oder ist dieser beschädigt, muss der abhängige Checkpoint als nicht vollständig wiederherstellbar gelten.

Beispiel:

```text
base_missing
```

oder:

```text
dependency_invalid
```

## Atomare Erzeugung

Ein inkrementeller Checkpoint muss entweder vollständig bestätigt oder verworfen werden.

Teilweise geschriebene Deltas dürfen nicht als gültige Checkpoints verwendet werden.

## Parallelität

Bei parallelen Tasks muss eindeutig sein, zu welchem konsistenten Zustand ein Delta gehört.

Änderungen verschiedener Execution Nodes dürfen nur gemeinsam gespeichert werden, wenn ihre Konsistenzbeziehung bekannt ist.

## Restore

Beim Restore wird der Zustand aus Basis und notwendigen Deltas rekonstruiert.

```text
Full A
    +
Delta B
    +
Delta C
    ↓
State C
```

NovaOS darf konsolidierte Zwischenergebnisse verwenden, um die Wiederherstellung zu beschleunigen.

## Beispiel

```text
Checkpoint A:
    full state

Checkpoint B:
    base: A

    changed {
        memory_page: 12
        object:result:41
        execution_node: 17
    }

Checkpoint C:
    base: B

    changed {
        memory_page: 13
        execution_node: 18
    }
```

Restore von `C`:

```text
A
↓ apply B
↓ apply C
State C
```

## Normative Anforderungen

1. Inkrementelle Checkpoints MÜSSEN ihren Basis-Checkpoint eindeutig referenzieren.
2. Nur veränderte Zustände SOLLEN erneut gespeichert werden.
3. Fehlende oder ungültige Basis-Checkpoints MÜSSEN erkannt werden.
4. Delta-Checkpoints MÜSSEN atomar bestätigt werden.
5. Lange Delta-Ketten MÜSSEN konsolidierbar sein.
6. Große unveränderte Daten SOLLEN referenziert statt kopiert werden.
7. Parallel erfasste Zustände MÜSSEN einer definierten Konsistenzgrenze zugeordnet sein.
8. Restore MUSS den vollständigen Zustand aus Basis und erforderlichen Deltas rekonstruieren können.

## Abgrenzung

Diese NPSPEC definiert:

- inkrementelle Checkpoints
- State Deltas
- Checkpoint-Ketten
- Konsolidierung
- grundlegenden Restore

Nicht Bestandteil sind:

- Safe Checkpoint Points
- detaillierte Konsistenzprüfung
- Crash Recovery
- Retention Policy
- Migration

## Zugehörige NPSPECs

- `NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model`
- `NPSPEC-CHECKPOINT-0002 – Safe Checkpoint Points`
- `NPSPEC-CHECKPOINT-0004 – Checkpoint Consistency`
- `NPSPEC-CHECKPOINT-0005 – Crash Recovery`
- `NPSPEC-CHECKPOINT-0006 – Checkpoint Resource & Retention Policy`
- `NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume`