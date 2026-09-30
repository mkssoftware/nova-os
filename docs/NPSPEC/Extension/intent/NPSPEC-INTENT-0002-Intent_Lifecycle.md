# NPSPEC-INTENT-0002 – Intent Lifecycle

## Status

Angenommen

## Zweck

Diese Spezifikation definiert den Lebenszyklus eines `Nova.Intent`.

Sie legt fest:

- welche Zustände ein Intent besitzen kann
- welche Zustandsübergänge zulässig sind
- wie Fehler, Abbruch und Wiederaufnahme behandelt werden
- wie Parent- und Child-Intents koordiniert werden
- welche Zustandsinformationen dauerhaft erhalten bleiben

Der Lifecycle beschreibt den Zustand der **Absicht**, nicht den Zustand eines konkreten Prozesses oder Programms.

## Grundmodell

```text
Created
   ↓
Validated
   ↓
Resolved
   ↓
Planned
   ↓
Ready
   ↓
Executing
   ↓
Completed
```

Alternative Pfade:

```text
Executing
   ├── Paused
   ├── Blocked
   ├── Failed
   └── Cancelled
```

Ein Intent kann unabhängig von der Lebensdauer eines einzelnen Prozesses weiterbestehen.

## Zustände

### `Created`

Der Intent wurde erzeugt, aber noch nicht vollständig geprüft.

```text
Created
```

Typische Aktionen:

- ID vergeben
- Ursprung erfassen
- Inputs referenzieren
- Kontext erfassen
- Parent-Beziehung setzen

In diesem Zustand darf noch keine fachliche Ausführung stattfinden.

---

### `Validated`

Das Intent-Objekt wurde syntaktisch und semantisch geprüft.

```text
Created
    ↓
Validated
```

Geprüft werden unter anderem:

- erforderliche Felder
- Intent-Typ
- semantische Typen
- Input-Referenzen
- Constraints
- Policies
- Berechtigungen für referenzierte Ressourcen

Eine erfolgreiche Validierung bedeutet noch nicht, dass der Intent ausführbar ist.

---

### `Resolved`

NovaOS hat festgestellt, welche Fähigkeiten grundsätzlich zur Erfüllung des Intents geeignet sind.

```text
Validated
    ↓
Resolved
```

Dabei können unter anderem bestimmt werden:

- passende Capabilities
- mögliche Datenpfade
- notwendige Sub-Intents
- erforderliche Abhängigkeiten
- verfügbare Ausführungsstrategien

Die konkrete Ausführung muss zu diesem Zeitpunkt noch nicht festgelegt sein.

---

### `Planned`

Für den Intent wurde ein gültiger Ausführungsplan erzeugt.

```text
Resolved
    ↓
Planned
```

Der Plan kann unter anderem enthalten:

- Execution Graph
- Capability-Auswahl
- Datenabhängigkeiten
- Execution Contracts
- Ressourcenanforderungen
- Checkpoint-Strategie
- Failure-Strategie

Der Plan darf später neu erzeugt werden, wenn sich relevante Bedingungen ändern.

---

### `Ready`

Der Intent ist vollständig vorbereitet und darf ausgeführt werden.

```text
Planned
    ↓
Ready
```

Voraussetzungen können sein:

- benötigte Inputs verfügbar
- Rechte vorhanden
- Policies erfüllt
- Ressourcen grundsätzlich verfügbar
- notwendige Dependencies aufgelöst

`Ready` bedeutet nicht, dass sofort CPU-Zeit zugeteilt werden muss.

---

### `Executing`

Der Intent wird aktiv ausgeführt.

```text
Ready
    ↓
Executing
```

Die konkrete Ausführung kann aus mehreren:

- Prozessen
- Threads
- Capabilities
- Geräten
- Compute Nodes
- Sub-Intents

bestehen.

Der Intent selbst bleibt die übergeordnete logische Einheit.

---

### `Paused`

Die Ausführung wurde kontrolliert angehalten.

```text
Executing
    ↓
Paused
```

Ein pausierter Intent behält seinen wiederaufnehmbaren Zustand.

Mögliche Gründe:

- Nutzer pausiert die Aufgabe
- Ressourcen werden benötigt
- Energieeinsparung
- Systemwartung
- TaskCapsule wird übertragen

Fortsetzung:

```text
Paused
    ↓
Ready
```

oder direkt:

```text
Paused
    ↓
Executing
```

abhängig vom Wiederaufnahmeverfahren.

---

### `Blocked`

Der Intent kann momentan nicht weiter ausgeführt werden.

```text
Executing
    ↓
Blocked
```

Mögliche Ursachen:

- Input fehlt
- Gerät nicht verfügbar
- Netzwerk erforderlich
- Nutzerentscheidung erforderlich
- Child-Intent noch nicht abgeschlossen
- Ressource temporär gesperrt

Ein blockierter Intent ist nicht fehlgeschlagen.

Sobald die Blockade entfällt:

```text
Blocked
    ↓
Ready
```

---

### `Completed`

Das Ziel des Intents wurde erfolgreich erreicht.

```text
Executing
    ↓
Completed
```

Dabei müssen mindestens:

- Ergebnis referenziert
- finaler Status gespeichert
- relevante Causality-Daten abgeschlossen
- Evidence-Informationen erzeugt
- Information-Flow-Metadaten fortgeführt

werden, sofern die jeweiligen Subsysteme verwendet werden.

`Completed` ist ein terminaler Zustand.

---

### `Failed`

Der Intent konnte nicht erfolgreich erfüllt werden.

```text
Executing
    ↓
Failed
```

Der Fehler muss strukturiert beschreibbar sein.

Beispiel:

```text
failure {
    domain: media.audio
    code: decoder_unavailable
    recoverable: true
}
```

Ein Fehler soll mindestens unterscheiden zwischen:

```text
recoverable
non_recoverable
policy_violation
invalid_input
resource_failure
dependency_failure
internal_failure
```

Ein `Failed` Intent darf über einen neuen Recovery-Versuch fortgeführt werden, sofern der Fehler als wiederherstellbar definiert ist.

Beispiel:

```text
Failed
   ↓ retry
Ready
```

Der fehlgeschlagene Zustand muss dabei historisch nachvollziehbar bleiben.

---

### `Cancelled`

Der Intent wurde bewusst beendet, bevor sein Ziel vollständig erreicht wurde.

```text
Created
Validated
Resolved
Planned
Ready
Executing
Paused
Blocked
    ↓
Cancelled
```

Eine Cancellation muss kontrolliert erfolgen.

NovaOS soll dabei:

- laufende Sub-Operationen stoppen
- sichere Abbruchpunkte verwenden
- temporäre Ressourcen freigeben
- notwendige Compensation Operations ausführen
- konsistente Ergebnisse erhalten

`Cancelled` ist ein terminaler Zustand.

## Terminale Zustände

Reguläre terminale Zustände sind:

```text
Completed
Cancelled
```

`Failed` kann terminal sein oder einen expliziten Recovery-/Retry-Pfad besitzen.

Ein terminaler Intent darf nicht stillschweigend wieder in einen aktiven Zustand versetzt werden.

Eine erneute Ausführung muss nachvollziehbar sein.

## Zulässige Hauptübergänge

```text
Created
    → Validated

Validated
    → Resolved

Resolved
    → Planned

Planned
    → Ready

Ready
    → Executing

Executing
    → Completed
    → Failed
    → Paused
    → Blocked
    → Cancelled

Paused
    → Ready
    → Executing
    → Cancelled

Blocked
    → Ready
    → Cancelled

Failed
    → Ready
    → Cancelled
```

Nicht definierte Übergänge sind standardmäßig unzulässig.

## Transition Record

Jeder relevante Zustandsübergang muss nachvollziehbar gespeichert werden.

Beispiel:

```text
IntentTransition {
    intent_id
    previous_state
    new_state
    timestamp
    reason
    actor
}
```

Beispiel:

```text
transition {
    intent_id: intent:01JQX7M8A4
    previous_state: Executing
    new_state: Paused
    reason: user_request
    actor: user:42
}
```

Transitions dürfen für Debugging, Causality und Recovery verwendet werden.

## Zustandsänderungen

Eine Zustandsänderung muss atomar sichtbar werden.

Andere Systemkomponenten dürfen keinen undefinierten Zwischenzustand beobachten.

Beispiel:

```text
Executing
```

darf nicht gleichzeitig als:

```text
Executing + Completed
```

sichtbar sein.

## Parent- und Child-Intents

Ein Parent-Intent kann mehrere Child-Intents besitzen.

Beispiel:

```text
Intent: Podcast veröffentlichen

    ├── Audio bereinigen
    ├── Transkript erzeugen
    ├── Kapitel erkennen
    └── Export erzeugen
```

Der Parent-Lifecycle hängt von seiner definierten Completion Policy ab.

Beispiele:

```text
ALL_REQUIRED
ANY_REQUIRED
QUORUM
CUSTOM
```

Bei:

```text
ALL_REQUIRED
```

darf der Parent erst `Completed` erreichen, wenn alle erforderlichen Child-Intents erfolgreich abgeschlossen wurden.

Ein optionaler Child-Intent darf fehlschlagen, ohne zwangsläufig den Parent-Intent fehlschlagen zu lassen.

## Fehlerpropagation

Fehler eines Child-Intents werden nicht automatisch unverändert auf den Parent übertragen.

Die Parent-Policy entscheidet über die Auswirkung.

Mögliche Reaktionen:

```text
ignore
retry_child
replace_child
replan
pause_parent
fail_parent
request_user_action
```

Dadurch kann NovaOS alternative Capabilities verwenden, bevor ein kompletter Task fehlschlägt.

## Replanning

Ein Intent darf neu geplant werden, wenn sich die Ausführungsbedingungen ändern.

Beispiel:

```text
Executing
    ↓ capability unavailable
Resolved
    ↓
Planned
    ↓
Ready
    ↓
Executing
```

Ein Replanning darf das ursprüngliche Ziel, die Policies und zwingende Constraints nicht unbemerkt verändern.

Vorherige Pläne müssen bei Bedarf nachvollziehbar bleiben.

## Retry

Wiederholungsversuche müssen explizit erkennbar sein.

Beispiel:

```text
attempt: 1
attempt: 2
attempt: 3
```

Retries dürfen nicht zu unbegrenzten Schleifen führen.

Eine Retry Policy kann beispielsweise definieren:

```text
retry {
    max_attempts: 3
    backoff: exponential
}
```

Die genaue Retry-Policy wird durch die zuständigen Execution- und Policy-Komponenten bestimmt.

## Idempotenz

Intent-Typen sollen deklarieren können, ob eine erneute Ausführung sicher wiederholbar ist.

Beispiel:

```text
idempotency:
    safe
```

oder:

```text
idempotency:
    unsafe
```

Bei nicht idempotenten Operationen muss NovaOS verhindern, dass Recovery oder Retry unbeabsichtigt externe Aktionen dupliziert.

Beispiele:

- Zahlung
- Nachricht versenden
- Hardwareaktion
- Veröffentlichung
- Löschvorgang

## Pause und Resume

Ein Intent muss unabhängig von einem konkreten Prozess pausierbar sein, sofern seine Ausführungsart dies unterstützt.

Die Wiederaufnahme darf über:

- gespeicherten Intent-Zustand
- MicroCheckpoint
- TaskCapsule
- rekonstruierbaren Execution Graph

erfolgen.

Ein Resume muss prüfen, ob:

- Inputs noch gültig sind
- Policies weiterhin erfüllt sind
- Capabilities weiterhin kompatibel sind
- Ressourcen weiterhin existieren

## Crash Recovery

Nach einem Systemabsturz muss NovaOS den zuletzt persistent bestätigten Lifecycle-Zustand rekonstruieren können.

Beispiel:

```text
vor Crash:
Executing

nach Neustart:
Recovering
    ↓
Ready
    ↓
Executing
```

`Recovering` kann als interner transienter Zustand implementiert werden und muss nicht Bestandteil des öffentlichen Intent-State-Modells sein.

NovaOS darf einen Intent nach einem Crash nicht automatisch als `Completed` markieren, wenn kein bestätigter Completion Record existiert.

## Cancellation

Cancellation ist eine semantische Operation.

Sie bedeutet nicht lediglich:

```text
kill process
```

NovaOS muss unterscheiden zwischen:

```text
request_cancel
cancel_in_progress
cancelled
```

Die konkrete Implementierung darf interne Zwischenzustände verwenden.

Extern bleibt der Intent so lange aktiv, bis die Cancellation konsistent abgeschlossen wurde.

## Lifecycle Persistence

Mindestens folgende Informationen müssen persistent rekonstruierbar sein:

```text
intent_id
current_state
previous_state
transition_history
execution_attempt
parent_intent
child_intents
result_reference
failure_reference
```

Nicht jede interne Scheduler-Information muss dauerhaft gespeichert werden.

## Beobachtbarkeit

Der Lifecycle muss systemweit beobachtbar sein.

Beispiele:

```text
Intent status
Task Manager
NovaShell
TaskCapsule
Debugger
System Logging
```

Beispiel:

```text
intent:01JQX7M8A4

state: Executing
progress: 62%
attempt: 1
```

`progress` ist optional und gehört nicht zwingend zum Lifecycle-Kern.

## Ereignisse

Zustandsänderungen sollen standardisierte Events erzeugen können.

Beispiele:

```text
Intent.Created
Intent.Validated
Intent.Resolved
Intent.Ready
Intent.Started
Intent.Paused
Intent.Resumed
Intent.Blocked
Intent.Completed
Intent.Failed
Intent.Cancelled
```

Andere Subsysteme dürfen diese Events beobachten, ohne den Intent direkt verändern zu dürfen.

## Sicherheitsmodell

Lifecycle-Operationen müssen autorisiert sein.

Nicht jeder Prozess oder jede Capability darf einen Intent:

- starten
- pausieren
- abbrechen
- wiederholen
- neu planen
- als abgeschlossen markieren

Berechtigungen werden durch die zuständigen NovaOS-Sicherheitsmechanismen geprüft.

Eine Capability darf ihren eigenen Intent nicht eigenmächtig als erfolgreich abgeschlossen markieren, wenn die Completion-Kriterien nicht erfüllt sind.

## Beispiel

```text
Intent:
    "Audioaufnahme transkribieren"

Created
    ↓
Validated
    ↓
Resolved
    ↓
Planned
    ↓
Ready
    ↓
Executing
    ↓
Blocked
    │
    │ Speech Engine wartet auf Ressource
    ↓
Ready
    ↓
Executing
    ↓
Completed
```

Ein komplexerer Fall:

```text
Intent:
    "Podcast veröffentlichen"

Created
    ↓
Validated
    ↓
Resolved
    ↓
Planned
    ↓
Executing

    ├── Audio Cleanup       → Completed
    ├── Transcription       → Completed
    ├── Chapter Detection   → Failed
    │                           ↓
    │                       Replanned
    │                           ↓
    │                       Completed
    │
    └── Export              → Completed

    ↓
Completed
```

## Normative Anforderungen

1. Jeder Intent MUSS einen eindeutig bestimmbaren Lifecycle-Zustand besitzen.
2. Zustandsübergänge MÜSSEN atomar sichtbar sein.
3. Nicht definierte Zustandsübergänge MÜSSEN standardmäßig abgelehnt werden.
4. Terminale Zustände DÜRFEN nicht stillschweigend reaktiviert werden.
5. Fehler, Cancellation und Retry MÜSSEN voneinander unterscheidbar sein.
6. Der Lifecycle MUSS unabhängig von einzelnen Prozessen bestehen können.
7. Relevante Zustandsübergänge MÜSSEN persistent nachvollziehbar sein.
8. Child-Fehler DÜRFEN nicht automatisch ungeprüft auf Parent-Intents übertragen werden.
9. Replanning DARF zwingende Constraints und Policies nicht unbemerkt verändern.
10. Nach einem Crash DARF ein Intent nur dann als `Completed` gelten, wenn ein bestätigter Completion-Zustand vorliegt.
11. Nicht idempotente Operationen MÜSSEN bei Retry und Recovery besonders behandelt werden.
12. Lifecycle-Operationen MÜSSEN autorisiert werden.

## Abgrenzung

Diese NPSPEC definiert den Lifecycle eines Intents.

Nicht Bestandteil sind:

- genaue Intent-Schemas
- Semantic-Type-System
- Capability Resolution
- konkrete Planungsalgorithmen
- Execution IR
- Execution Contracts
- MicroCheckpoint-Format
- TaskCapsule-Format
- konkrete Retry-Algorithmen

Diese Bereiche werden separat spezifiziert.

## Zugehörige NPSPECs

- `NPSPEC-INTENT-0001 – Intent Object Model`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`
- `NPSPEC-INTENT-0004 – Intent Resolution`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`
- `NPSPEC-INTENT-0006 – Intent Composition`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`
- `NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model`
- `NPSPEC-CAPSULE-0001 – TaskCapsule Container Format`