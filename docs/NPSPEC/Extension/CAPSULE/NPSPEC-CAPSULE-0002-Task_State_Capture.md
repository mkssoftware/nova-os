# NPSPEC-CAPSULE-0002 – Task State Capture

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie `Nova.TaskCapsule` den logischen Zustand einer laufenden Aufgabe erfasst.

Ziel ist, eine Aufgabe so zu speichern, dass sie später mit möglichst wenig Kontextverlust fortgesetzt werden kann.

## Grundprinzip

```text
laufende Aufgabe
    ↓
Task State Capture
    ↓
TaskCapsule
    ↓
Restore
    ↓
Fortsetzung
```

Der erfasste Task-Zustand soll möglichst unabhängig von konkreten Prozessen oder Prozess-IDs sein.

## Task State

Der logische Zustand kann mindestens enthalten:

```text
TaskState {
    root_intent
    lifecycle_state
    active_step
    completed_steps
    pending_steps
    execution_context
    open_objects
    dependencies
    checkpoint_reference
}
```

## Aktiver Zustand

`active_step` beschreibt, welcher logische Teil der Aufgabe gerade ausgeführt wird.

Beispiel:

```text
active_step:
    media.audio.transcribe
```

Die Beschreibung soll semantisch und nicht nur technisch erfolgen.

## Abgeschlossene Schritte

Bereits erfolgreich abgeschlossene Teilaufgaben müssen erhalten bleiben.

Beispiel:

```text
completed_steps {
    audio.clean
    metadata.read
}
```

Diese Schritte sollen nach Restore nicht unnötig erneut ausgeführt werden.

## Offene Schritte

Noch ausstehende Arbeit muss unterscheidbar sein.

Beispiel:

```text
pending_steps {
    audio.transcribe
    chapter.detect
    media.export
}
```

## Execution Context

Der relevante Ausführungskontext darf enthalten:

```text
workspace
current_intent
child_intents
execution_graph
execution_contracts
user_context
```

Nur für die Wiederaufnahme notwendiger Kontext soll gespeichert werden.

## Offene Objekte

Eine TaskCapsule darf aktuell verwendete Objekte referenzieren.

Beispiel:

```text
open_objects {
    object:audio:42
    object:transcript:81
}
```

Eine offene Referenz bedeutet nicht automatisch, dass das Objekt vollständig in die Capsule eingebettet wird.

## UI-Zustand

Oberflächenzustand darf optional Bestandteil des Task State sein.

Beispiele:

```text
open_views
selected_object
cursor_position
scroll_position
window_context
```

UI-Zustand ist vom fachlichen Task-Zustand getrennt.

Fehlt er, muss die Aufgabe trotzdem grundsätzlich fortsetzbar bleiben.

## MicroCheckpoint

Für laufende Berechnungen darf der Task State auf einen `Nova.MicroCheckpoint` verweisen.

Beispiel:

```text
checkpoint:
    checkpoint:2048
```

Der MicroCheckpoint enthält den feingranularen Ausführungszustand.

Die TaskCapsule enthält den übergeordneten Aufgabenstatus.

## Konsistenz

Task State Capture muss an einer definierten Konsistenzgrenze erfolgen.

Beispiel:

```text
Parent Intent
    ├── Child A: Completed
    ├── Child B: Executing
    └── Child C: Ready
```

Dieser Zustand muss als zusammengehörige Sicht gespeichert werden.

Halb aktualisierte Parent-/Child-Zustände dürfen nicht als gültiger Task State bestätigt werden.

## Referenzen und eingebettete Daten

Task State Capture soll große Daten nicht unnötig kopieren.

Bevorzugt:

```text
object_reference
```

statt vollständiger Einbettung.

Nur nicht anderweitig verfügbare oder für Portabilität notwendige Zustände müssen eingebettet werden.

## Änderungen nach Capture

Der erfasste Zustand beschreibt einen konkreten Zeitpunkt.

Läuft die Aufgabe danach weiter, bleibt der gespeicherte Zustand unverändert.

Neue Zustände erzeugen:

```text
new task state
```

oder eine neue Capsule-Version.

## Restore

Beim Restore müssen relevante Referenzen und Abhängigkeiten erneut geprüft werden.

Mögliche Ergebnisse:

```text
READY
REPLAN_REQUIRED
BLOCKED
INCOMPATIBLE
```

Fehlende oder veränderte Ressourcen dürfen nicht stillschweigend als unverändert angenommen werden.

## Beispiel

```text
TaskState {
    root_intent:
        intent:podcast.publish

    lifecycle_state:
        Executing

    active_step:
        audio.transcribe

    completed_steps {
        audio.clean
    }

    pending_steps {
        chapter.detect
        media.export
    }

    open_objects {
        object:audio:42
    }

    checkpoint_reference:
        checkpoint:2048
}
```

## Normative Anforderungen

1. Task State Capture MUSS den logischen Zustand einer Aufgabe unabhängig von einzelnen Prozessen beschreiben können.
2. Abgeschlossene, aktive und ausstehende Schritte MÜSSEN unterscheidbar sein.
3. Parent-/Child-Intent-Zustände MÜSSEN konsistent erfasst werden.
4. Große unveränderte Daten SOLLEN referenziert statt dupliziert werden.
5. UI-Zustand DARF gespeichert werden, MUSS aber vom fachlichen Task-Zustand getrennt bleiben.
6. Laufende Berechnungen MÜSSEN auf einen MicroCheckpoint verweisen können.
7. Gespeicherte Zustände DÜRFEN nachträglich nicht stillschweigend verändert werden.
8. Referenzen und Abhängigkeiten MÜSSEN beim Restore erneut prüfbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Task State Capture
- aktive, abgeschlossene und offene Schritte
- Task-Kontext
- offene Objekte
- MicroCheckpoint-Referenzen

Nicht Bestandteil sind:

- Containerformat
- Capability Manifest
- Ressourcen-Packaging
- Security
- Migration und Resume

## Zugehörige NPSPECs

- `NPSPEC-CAPSULE-0001 – TaskCapsule Container Format`
- `NPSPEC-CAPSULE-0003 – Capability & Dependency Manifest`
- `NPSPEC-CAPSULE-0004 – Resource Packaging & References`
- `NPSPEC-CAPSULE-0005 – Capsule Security & Identity`
- `NPSPEC-CAPSULE-0006 – Capsule Migration & Resume`
- `NPSPEC-CAPSULE-0007 – Capsule Versioning & Compatibility`
- `NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model`