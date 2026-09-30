# NPSPEC-UNDO-0004 – Cross-Capability Undo Coordination

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS Undo über mehrere beteiligte Capabilities hinweg koordiniert.

Ziel ist, dass eine fachlich zusammengehörige Aktion auch dann konsistent rückgängig gemacht werden kann, wenn ihre Änderungen von unterschiedlichen Capabilities ausgeführt wurden.

## Grundprinzip

```text
Undo Transaction
    ├── Capability A
    ├── Capability B
    └── Capability C
            ↓
    koordinierte Compensation
```

Die Koordination richtet sich nach semantischen Abhängigkeiten und nicht nach Prozessgrenzen.

## Teilnehmer

Jede beteiligte Capability kann für ihre eigenen Semantic Actions zuständig sein.

Beispiel:

```text
Project.Rename

Storage Capability
    → directory.rename

Metadata Capability
    → metadata.update

Reference Capability
    → references.update
```

Alle Aktionen können Bestandteil derselben Undo Transaction sein.

## Coordinator

Eine Cross-Capability Undo Transaction benötigt eine übergeordnete Koordination.

Der Coordinator verwaltet mindestens:

```text
participants
dependencies
compensation_order
status
failures
```

Der Coordinator führt die fachliche Undo-Operation zusammen, ohne die interne Implementierung einzelner Capabilities kennen zu müssen.

## Capability Contract

Eine undo-fähige Capability muss für relevante Aktionen beschreiben können:

```text
action
reversibility
compensation
preconditions
dependencies
```

Beispiel:

```text
CapabilityUndoDescriptor {
    action: metadata.update
    reversibility: REVERSIBLE
    compensation: metadata.restore
}
```

## Abhängigkeiten

Die Undo-Reihenfolge wird aus den semantischen Abhängigkeiten abgeleitet.

Ausführung:

```text
Storage
    ↓
Metadata
    ↓
References
```

Undo:

```text
References
    ↓
Metadata
    ↓
Storage
```

Unabhängige Compensation Operations dürfen parallel ausgeführt werden.

## Prepare-Phase

Vor dem tatsächlichen Undo darf NovaOS alle Teilnehmer prüfen.

Beispiel:

```text
PREPARE
    ├── Capability A → READY
    ├── Capability B → READY
    └── Capability C → READY
```

Geprüft werden insbesondere:

- Voraussetzungen
- aktueller Objektzustand
- Berechtigungen
- Verfügbarkeit
- Reversibility

Ist ein kritischer Teilnehmer nicht bereit, darf das Undo vor der eigentlichen Veränderung gestoppt werden.

## Compensation

Nach erfolgreicher Vorbereitung werden die Compensation Operations ausgeführt.

```text
PREPARED
    ↓
COMPENSATING
    ↓
UNDONE
```

Die Ausführung folgt dem Dependency Graph der Undo Transaction.

## Fehler während der Coordination

Eine Capability kann während der Compensation fehlschlagen.

Beispiel:

```text
Capability C → compensated
Capability B → failed
Capability A → not executed
```

Die gesamte Undo Transaction muss dann beispielsweise als:

```text
PARTIAL
```

oder:

```text
FAILED
```

markiert werden.

Bereits ausgeführte Compensation darf nicht verborgen werden.

## Capability-Ausfall

Ist eine ursprünglich verwendete Capability nicht mehr verfügbar, darf NovaOS eine kompatible alternative Capability verwenden, wenn:

- dieselbe semantische Compensation erfüllt wird
- Constraints und Policies eingehalten werden
- die Alternative ausreichend vertrauenswürdig ist

Beispiel:

```text
Original Capability:
    storage.v1

Unavailable

Compatible Capability:
    storage.v2
```

Die Substitution muss nachvollziehbar bleiben.

## Idempotenz

Der Coordinator muss erkennen können, welche Compensation Operations bereits erfolgreich ausgeführt wurden.

Nach einem Crash:

```text
Compensation A → completed
Compensation B → pending
Compensation C → pending
```

dürfen bereits bestätigte nicht idempotente Operationen nicht blind erneut ausgeführt werden.

## Persistenz

Der Coordinationszustand muss persistent rekonstruierbar sein.

Mindestens:

```text
transaction_id
participants
dependency_graph
participant_status
completed_compensations
pending_compensations
failure_state
```

Dadurch kann ein unterbrochenes Undo nach einem Neustart kontrolliert fortgesetzt werden.

## Isolation

Eine Capability darf während einer Cross-Capability-Undo-Operation nicht eigenmächtig Aktionen anderer Teilnehmer zurücknehmen.

Jede Capability kompensiert nur die ihr zugewiesenen Semantic Actions.

Die Gesamtkoordination bleibt Aufgabe des Coordinators.

## Beispiel

```text
Undo Transaction:
    project.rename

Actions:
    Storage.rename
    Metadata.update
    References.update

Dependencies:

Storage.rename
    ↓
Metadata.update
    ↓
References.update
```

Compensation:

```text
References.restore
    ↓
Metadata.restore
    ↓
Storage.rename_back
```

Status:

```text
References: COMPLETED
Metadata:   COMPLETED
Storage:    COMPLETED

Transaction:
    UNDONE
```

## Normative Anforderungen

1. Undo MUSS über mehrere Capabilities hinweg koordinierbar sein.
2. Jede beteiligte Capability MUSS ihre eigenen Compensation-Möglichkeiten beschreiben können.
3. Die Compensation-Reihenfolge MUSS semantische Abhängigkeiten berücksichtigen.
4. Kritische Preconditions SOLLEN vor Beginn der Compensation geprüft werden.
5. Teilweise erfolgreiche Cross-Capability-UnDos MÜSSEN erkennbar bleiben.
6. Bereits bestätigte Compensation Operations DÜRFEN bei Recovery nicht unkontrolliert erneut ausgeführt werden.
7. Alternative Capabilities DÜRFEN nur verwendet werden, wenn die gleiche semantische Wirkung garantiert werden kann.
8. Der Coordinationszustand MUSS persistent rekonstruierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Koordination mehrerer Capabilities
- Teilnehmermodell
- Dependency-basierte Compensation
- Recovery der Undo-Koordination

Nicht Bestandteil sind:

- Definition einzelner Semantic Actions
- konkrete Compensation-Logik
- Konfliktauflösung
- Behandlung irreversibler Aktionen

## Zugehörige NPSPECs

- `NPSPEC-UNDO-0001 – Semantic Action Model`
- `NPSPEC-UNDO-0002 – Compensation Operations`
- `NPSPEC-UNDO-0003 – Undo Transaction Boundaries`
- `NPSPEC-UNDO-0005 – Undo Conflict Detection & Resolution`
- `NPSPEC-UNDO-0006 – Irreversible Operation Handling`