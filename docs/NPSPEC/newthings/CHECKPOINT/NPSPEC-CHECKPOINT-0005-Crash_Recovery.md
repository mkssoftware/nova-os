# NPSPEC-CHECKPOINT-0005 – Crash Recovery

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS nach einem Absturz gültige `MicroCheckpoints` erkennt und zur kontrollierten Wiederaufnahme verwendet.

Ziel ist, laufende Aufgaben möglichst ohne Datenverlust und ohne unbeabsichtigte Doppel-Ausführung fortzusetzen.

## Grundprinzip

```text
Crash
    ↓
Recovery Scan
    ↓
gültigen Checkpoint bestimmen
    ↓
Abhängigkeiten prüfen
    ↓
Resume / Replan / Fail
```

## Recovery Source

NovaOS darf für eine Wiederherstellung verwenden:

- letzten gültigen MicroCheckpoint
- konsolidierten Basis-Checkpoint
- gültige Delta-Kette
- bestätigte Intent-Persistenz
- TaskCapsule-Zustand

Ungültige oder unvollständige Checkpoints dürfen nicht automatisch verwendet werden.

## Recovery Auswahl

Wenn mehrere Checkpoints existieren, soll NovaOS den neuesten gültigen Zustand wählen, der:

- konsistent ist
- vollständig rekonstruierbar ist
- gültige Abhängigkeiten besitzt
- keine ungeklärten Seiteneffekte enthält

Beispiel:

```text
Checkpoint A: VALID
Checkpoint B: VALID
Checkpoint C: PARTIAL

Recovery:
    Checkpoint B
```

## Recovery Ablauf

Ein typischer Ablauf lautet:

```text
Load Checkpoint
    ↓
Validate Integrity
    ↓
Validate Dependencies
    ↓
Validate External Effects
    ↓
Restore State
    ↓
Revalidate Execution Plan
    ↓
Resume
```

## Externe Seiteneffekte

Besondere Aufmerksamkeit gilt nicht idempotenten Operationen.

Beispiel:

```text
payment.send
message.send
device.command
remote.commit
```

Nach einem Crash muss geprüft werden, ob die Wirkung:

```text
NOT_STARTED
COMMITTED
FAILED
UNKNOWN
```

ist.

`UNKNOWN` darf nicht blind erneut ausgeführt werden.

## Re-Resolution und Replanning

Ist die ursprüngliche Ausführungsumgebung nicht mehr verfügbar, darf NovaOS den Intent neu auflösen oder planen.

Beispiel:

```text
Vor Crash:
    GPU Execution

Nach Neustart:
    GPU unavailable

Recovery:
    Replan
    ↓
CPU Execution
```

Der ursprüngliche Intent sowie harte Constraints und Policies bleiben bestehen.

## Teilweise Wiederherstellung

Kann nur ein Teil des Zustands sicher wiederhergestellt werden, muss dies sichtbar bleiben.

Mögliche Ergebnisse:

```text
RECOVERED
PARTIAL
REPLAN_REQUIRED
MANUAL_ACTION_REQUIRED
FAILED
```

`PARTIAL` darf nicht als vollständige Wiederherstellung dargestellt werden.

## Parent- und Child-Tasks

Bei zusammengesetzten Intents müssen bereits bestätigte Child-Ergebnisse erhalten bleiben.

Beispiel:

```text
Child A:
    Completed

Child B:
    Executing

Child C:
    Ready
```

Nach Recovery darf `Child A` nicht unnötig erneut ausgeführt werden.

## Recovery Point

NovaOS muss eindeutig bestimmen können, ab welchem Punkt die Ausführung fortgesetzt wird.

Beispiel:

```text
resume_at:
    execution_node:17
```

Bereits bestätigte Operationen vor diesem Punkt gelten als abgeschlossen.

Nicht bestätigte Operationen müssen erneut bewertet werden.

## Recovery Logging

Ein Recovery-Vorgang soll mindestens dokumentieren:

```text
crash_reference
checkpoint_used
restored_state
replanned_components
unresolved_effects
recovery_result
```

Diese Informationen können mit `Nova.Causality` und `Nova.Evidence` verknüpft werden.

## Beispiel

Vor Crash:

```text
Node 15:
    Completed

Node 16:
    Completed

Checkpoint:
    VALID

Node 17:
    Executing

Crash
```

Nach Neustart:

```text
Restore Checkpoint
    ↓
Resume at Node 17
```

Falls Node 17 eine externe Operation gestartet hatte:

```text
verify external state
    ↓
continue / compensate / request user action
```

## Normative Anforderungen

1. Crash Recovery MUSS nur gültige und rekonstruierbare Checkpoints verwenden.
2. Der neueste Checkpoint DARF nicht allein aufgrund seines Alters bevorzugt werden, wenn ein älterer Zustand konsistenter ist.
3. Externe und nicht idempotente Seiteneffekte MÜSSEN vor Retry geprüft werden.
4. Bereits bestätigte Ergebnisse DÜRFEN nicht unnötig erneut ausgeführt werden.
5. Re-Resolution und Replanning MÜSSEN möglich sein, wenn ursprüngliche Abhängigkeiten nicht mehr verfügbar sind.
6. Teilweise Recovery MUSS als solche erkennbar bleiben.
7. Der verwendete Recovery Point MUSS eindeutig bestimmbar sein.
8. Recovery-Vorgänge SOLLEN nachvollziehbar protokolliert werden.

## Abgrenzung

Diese NPSPEC definiert:

- Crash Recovery mit MicroCheckpoints
- Recovery-Auswahl
- Umgang mit externen Seiteneffekten
- Replanning nach Absturz
- teilweise Wiederherstellung

Nicht Bestandteil sind:

- Checkpoint-Erzeugung
- inkrementelle Speicherung
- Retention Policy
- Migration zwischen Systemen

## Zugehörige NPSPECs

- `NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model`
- `NPSPEC-CHECKPOINT-0002 – Safe Checkpoint Points`
- `NPSPEC-CHECKPOINT-0003 – Incremental State Capture`
- `NPSPEC-CHECKPOINT-0004 – Checkpoint Consistency`
- `NPSPEC-CHECKPOINT-0006 – Checkpoint Resource & Retention Policy`
- `NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`