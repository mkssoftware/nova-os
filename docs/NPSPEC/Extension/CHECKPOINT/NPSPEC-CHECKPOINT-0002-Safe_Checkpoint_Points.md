# NPSPEC-CHECKPOINT-0002 – Safe Checkpoint Points

## Status

Angenommen

## Zweck

Diese Spezifikation definiert sichere Punkte, an denen NovaOS einen `MicroCheckpoint` erzeugen kann.

Ein Safe Checkpoint Point stellt sicher, dass eine Ausführung später aus einem konsistenten Zustand fortgesetzt werden kann.

## Grundprinzip

```text
Operation A
    ↓
SAFE CHECKPOINT POINT
    ↓
Operation B
```

Ein Safe Point liegt nur dann vor, wenn keine unkontrollierte oder unvollständig dokumentierte Zustandsänderung offen ist.

## Safe Point

Ein Safe Point kann logisch beschrieben werden als:

```text
SafePoint {
    id
    execution_position
    consistency_scope
    pending_effects
    dependencies
}
```

Er markiert einen definierten Wiederaufnahmezustand.

## Voraussetzungen

Ein Safe Point darf nur bestätigt werden, wenn relevante Zustände konsistent sind.

Dazu gehören insbesondere:

- interne Datenstrukturen
- laufende Transaktionen
- Speicherzustände
- offene Schreiboperationen
- externe Seiteneffekte
- Capability-Zustände

## Sichere Grenzen

Typische Safe Points liegen:

```text
vor einer Operation
nach einer abgeschlossenen Operation
zwischen Execution-Graph-Nodes
nach einem Transaction Commit
nach bestätigten externen Effekten
```

Nicht sicher sind beispielsweise Zustände mitten in einer teilweise ausgeführten nicht-idempotenten Operation.

## Execution Graph

Execution-Graph-Nodes dürfen explizite Checkpoint-Grenzen definieren.

Beispiel:

```text
Node A
    ↓
CHECKPOINT_ALLOWED
    ↓
Node B
```

Ein Node darf zusätzlich markieren:

```text
CHECKPOINT_FORBIDDEN
```

wenn sein interner Zustand nicht sicher rekonstruierbar ist.

## Capability Safe Points

Capabilities müssen sichere Wiederaufnahmegrenzen beschreiben können, wenn ihre Ausführung nicht beliebig unterbrochen werden darf.

Beispiel:

```text
Capability {
    checkpoint_policy:
        between_chunks
}
```

NovaOS muss diese Grenzen respektieren.

## Nicht idempotente Operationen

Vor oder nach nicht idempotenten Operationen müssen Safe Points besonders eindeutig sein.

Beispiel:

```text
Checkpoint
    ↓
external write
    ↓
commit confirmation
    ↓
Checkpoint
```

Nach einem Crash muss erkennbar sein, ob die externe Wirkung tatsächlich ausgeführt wurde.

## Offene Transaktionen

Ein Safe Point darf grundsätzlich nicht innerhalb einer unbestätigten Transaktion liegen, sofern deren vollständiger Zustand nicht checkpointfähig ist.

Bevorzugt:

```text
Transaction
    ↓
COMMIT
    ↓
SAFE POINT
```

## Asynchrone Operationen

Bei parallelen oder asynchronen Tasks muss ein Safe Point den relevanten gemeinsamen Zustand konsistent erfassen.

Mögliche Strategien:

```text
local safe point
task-group safe point
graph-wide safe point
```

Nicht jeder Checkpoint muss den gesamten Execution Graph anhalten.

## Safe-Point-Anforderung

NovaOS darf einen Safe Point aktiv anfordern.

Beispiel:

```text
checkpoint requested
    ↓
continue until next safe point
    ↓
capture state
```

Eine laufende Operation muss dafür nicht unsicher mitten in der Ausführung gestoppt werden.

## Zeitlimit

Kann ein Safe Point längere Zeit nicht erreicht werden, darf NovaOS abhängig vom Execution Contract:

```text
wait
request_partial_checkpoint
replan
cancel
```

Ein unsicherer Zustand darf nicht allein wegen eines Zeitlimits als sicher markiert werden.

## Validierung

Vor dem Speichern kann NovaOS prüfen:

```text
no_untracked_side_effects
dependencies_known
transaction_state_valid
continuation_defined
```

Erst danach wird der Checkpoint als:

```text
VALID
```

markiert.

## Beispiel

```text
Execution:

Read Input
    ↓
SAFE POINT
    ↓
Process Block 1
    ↓
SAFE POINT
    ↓
Process Block 2
    ↓
SAFE POINT
    ↓
Write Result
    ↓
Commit
    ↓
SAFE POINT
```

Bei einem Crash nach `Process Block 2` kann vom letzten bestätigten Safe Point fortgesetzt werden.

## Normative Anforderungen

1. Ein MicroCheckpoint DARF nur an einem ausreichend konsistenten Safe Point als vollständig gültig bestätigt werden.
2. Capabilities MÜSSEN nicht beliebig unterbrechbare Bereiche kennzeichnen können.
3. Nicht idempotente Seiteneffekte MÜSSEN an eindeutigen Checkpoint-Grenzen nachvollziehbar sein.
4. Unbestätigte Transaktionen DÜRFEN nicht ohne vollständige Recovery-Semantik als sichere Checkpoints behandelt werden.
5. Asynchrone Ausführung MUSS konsistente lokale oder gemeinsame Safe Points unterstützen können.
6. Eine Checkpoint-Anforderung MUSS bis zum nächsten sicheren Punkt verzögert werden können.
7. Zeitdruck DARF keinen unsicheren Zustand in einen gültigen Safe Point umdeuten.
8. Safe Points MÜSSEN für Resume eindeutig referenzierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Safe Checkpoint Points
- sichere Ausführungsgrenzen
- Capability- und Transaction-Grenzen
- Safe Points bei asynchroner Ausführung

Nicht Bestandteil sind:

- Speicherung inkrementeller Zustände
- Checkpoint-Konsistenzprüfung im Detail
- Crash Recovery
- Retention
- Migration

## Zugehörige NPSPECs

- `NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model`
- `NPSPEC-CHECKPOINT-0003 – Incremental State Capture`
- `NPSPEC-CHECKPOINT-0004 – Checkpoint Consistency`
- `NPSPEC-CHECKPOINT-0005 – Crash Recovery`
- `NPSPEC-CHECKPOINT-0006 – Checkpoint Resource & Retention Policy`
- `NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume`