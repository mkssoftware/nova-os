# NPSPEC-UNDO-0002 – Compensation Operations

## Status

Angenommen

## Zweck

Diese Spezifikation definiert Gegenoperationen für systemweites semantisches Undo.

Eine `Compensation Operation` versucht, die Wirkung einer zuvor ausgeführten `Semantic Action` kontrolliert zurückzunehmen oder semantisch auszugleichen.

Grundprinzip:

```text
Original Action
    ↓
State Change
    ↓
Compensation Operation
    ↓
Compensated State
```

Eine Compensation muss nicht immer den exakt vorherigen technischen Zustand wiederherstellen.

Entscheidend ist die semantisch korrekte Rücknahme der Wirkung.

## Compensation Model

Eine Compensation referenziert die ursprüngliche Aktion.

```text
CompensationOperation {
    id
    original_action
    type
    preconditions
    effects
    status
}
```

Beispiel:

```text
Original:
    storage.file.rename
    notes.txt → archive.txt

Compensation:
    storage.file.rename
    archive.txt → notes.txt
```

## Arten

NovaOS muss mindestens unterscheiden zwischen:

```text
INVERSE
RESTORE
COMPENSATE
CANCEL
```

### `INVERSE`

Eine direkte Gegenoperation existiert.

```text
move A → B
```

wird kompensiert durch:

```text
move B → A
```

### `RESTORE`

Ein vorheriger Zustand wird aus gespeicherten Daten wiederhergestellt.

Beispiel:

```text
document.edit
    ↓
restore previous version
```

### `COMPENSATE`

Die ursprüngliche Wirkung kann nicht vollständig rückgängig gemacht werden, aber eine fachlich passende Gegenaktion existiert.

Beispiel:

```text
reservation.create
```

kann durch:

```text
reservation.cancel
```

kompensiert werden.

### `CANCEL`

Eine noch nicht vollständig abgeschlossene Aktion wird kontrolliert beendet.

## Preconditions

Vor der Ausführung müssen die Voraussetzungen der Compensation geprüft werden.

Beispiel:

```text
preconditions {
    object_exists: true
    expected_name: archive.txt
    destination_free: true
}
```

Sind die Bedingungen nicht erfüllt, darf die Gegenoperation nicht blind ausgeführt werden.

Der Fall wird an die Konfliktbehandlung übergeben.

## Effects

Die erwartete Wirkung der Compensation muss beschreibbar sein.

Beispiel:

```text
effects {
    restore_name: notes.txt
}
```

Nach erfolgreicher Ausführung muss NovaOS prüfen können, ob die erwartete semantische Wirkung erreicht wurde.

## Idempotenz

Compensation Operations sollen nach Möglichkeit idempotent sein.

Eine erneute Ausführung darf keine zusätzliche unerwünschte Wirkung erzeugen.

Falls eine Compensation nicht idempotent ist, muss ihr Ausführungszustand persistent nachvollziehbar sein.

## Reihenfolge

Bei mehreren abhängigen Aktionen erfolgt Compensation grundsätzlich in umgekehrter Abhängigkeitsreihenfolge.

Beispiel:

```text
A
↓
B
↓
C
```

Undo:

```text
undo C
↓
undo B
↓
undo A
```

Die konkrete Transaktionskoordination wird separat spezifiziert.

## Fehler

Eine Compensation kann fehlschlagen.

Mögliche Zustände:

```text
PENDING
EXECUTING
COMPLETED
FAILED
CONFLICT
PARTIAL
```

Ein fehlgeschlagenes Undo darf nicht automatisch als erfolgreich dargestellt werden.

Teilweise kompensierte Zustände müssen erkennbar bleiben.

## Causality

Originalaktion und Compensation müssen kausal miteinander verknüpft sein.

Beispiel:

```text
SemanticAction A
    ↓ compensated_by
CompensationOperation B
```

Damit bleibt nachvollziehbar:

- was ursprünglich geschah
- warum die Compensation ausgeführt wurde
- welche Wirkung sie hatte

Die ursprüngliche Historie wird nicht gelöscht.

## Externe Operationen

Bei externen Systemen kann eine echte Rücknahme unmöglich sein.

Beispiel:

```text
email.send
```

Eine mögliche Compensation kann sein:

```text
email.recall
```

oder:

```text
followup.send
```

NovaOS darf solche Aktionen nicht als vollständige technische Rücknahme darstellen, wenn die ursprüngliche Wirkung bereits außerhalb des Systems eingetreten ist.

## Beispiel

```text
SemanticAction {
    id: action:1042

    type:
        storage.file.rename

    effects {
        old_name: notes.txt
        new_name: archive.txt
    }
}
```

Compensation:

```text
CompensationOperation {
    id: compensation:1042

    original_action:
        action:1042

    type:
        storage.file.rename

    preconditions {
        current_name: archive.txt
        target_name_available: true
    }

    effects {
        new_name: notes.txt
    }
}
```

## Normative Anforderungen

1. Jede Compensation MUSS die ursprüngliche Semantic Action referenzieren.
2. Preconditions MÜSSEN vor der Ausführung geprüft werden.
3. NovaOS MUSS direkte Inversen, Restore und semantische Compensation unterscheiden können.
4. Fehlgeschlagene oder teilweise Compensation MUSS erkennbar bleiben.
5. Originalaktion und Compensation MÜSSEN historisch getrennt erhalten bleiben.
6. Nicht idempotente Compensation MUSS vor Doppel-Ausführung geschützt werden.
7. Externe Wirkungen DÜRFEN nicht fälschlich als vollständig rückgängig gemacht dargestellt werden.

## Abgrenzung

Diese NPSPEC definiert:

- Compensation Operations
- Preconditions
- Compensation-Arten
- grundlegende Ausführungszustände

Nicht Bestandteil sind:

- Undo-Transaktionsgrenzen
- Cross-Capability-Koordination
- Konfliktauflösung
- Behandlung irreversibler Aktionen

## Zugehörige NPSPECs

- `NPSPEC-UNDO-0001 – Semantic Action Model`
- `NPSPEC-UNDO-0003 – Undo Transaction Boundaries`
- `NPSPEC-UNDO-0004 – Cross-Capability Undo Coordination`
- `NPSPEC-UNDO-0005 – Undo Conflict Detection & Resolution`
- `NPSPEC-UNDO-0006 – Irreversible Operation Handling`
- `NPSPEC-CAUSAL-0001 – Causality Graph Model`