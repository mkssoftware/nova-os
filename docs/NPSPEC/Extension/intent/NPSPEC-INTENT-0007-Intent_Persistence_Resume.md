# NPSPEC-INTENT-0007 – Intent Persistence & Resume

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie `Nova.Intent` dauerhaft gespeichert und nach Pause, Absturz, Neustart oder Migration wieder aufgenommen wird.

Ziel ist, dass ein Intent nicht an die Lebensdauer eines einzelnen Prozesses gebunden ist.

## Grundprinzip

```text
Intent
    ↓
Persist
    ↓
Pause / Crash / Neustart / Migration
    ↓
Restore
    ↓
Revalidate
    ↓
Resume
```

## Persistenter Zustand

Mindestens folgende Informationen müssen rekonstruierbar sein:

```text
intent_id
schema_version
lifecycle_state
inputs
expected_output
context
constraints
policies
parent_intent
child_intents
execution_attempt
result_reference
failure_reference
```

Zusätzliche Laufzeitinformationen dürfen separat gespeichert werden.

## Persistenzpunkte

Ein Intent muss an definierten Punkten persistent gesichert werden können.

Typische Persistenzpunkte:

```text
Created
Validated
Resolved
Planned
Ready
Paused
Blocked
Completed
Failed
Cancelled
```

Während `Executing` können zusätzliche sichere Persistenzpunkte durch `Nova.MicroCheckpoint` bereitgestellt werden.

## Atomare Persistenz

Ein gespeicherter Intent-Zustand muss konsistent sein.

NovaOS darf nach einem Absturz keinen teilweise geschriebenen Lifecycle-Zustand als gültig behandeln.

Beispiel:

```text
Persisted:
    state = Ready
```

oder:

```text
Persisted:
    state = Executing
```

aber kein undefinierter Mischzustand.

## Resume

Vor einer Wiederaufnahme muss NovaOS prüfen, ob die ursprünglichen Voraussetzungen weiterhin gültig sind.

Mindestens zu prüfen:

- Intent-Schema
- Semantic Types
- Input-Referenzen
- Constraints
- Policies
- Berechtigungen
- Capability-Verfügbarkeit
- Abhängigkeiten

Beispiel:

```text
Stored Intent
    ↓
Restore
    ↓
Validation
    ↓
Resolution Check
    ↓
Ready
    ↓
Executing
```

## Re-Resolution

Ist der ursprüngliche Ausführungspfad nicht mehr verfügbar, darf NovaOS den Intent erneut auflösen.

Beispiel:

```text
Vorher:
    NPU Capability

Nach Neustart:
    NPU nicht verfügbar

Resume:
    Re-Resolution
        ↓
    CPU Capability
```

Das ursprüngliche Ziel, zwingende Constraints und Policies müssen erhalten bleiben.

## Resume nach Crash

Nach einem Absturz darf NovaOS einen Intent nur aus einem bestätigten persistenten Zustand rekonstruieren.

Ein Intent darf nicht allein deshalb als abgeschlossen gelten, weil seine Prozesse vor dem Absturz beendet wurden.

```text
kein bestätigtes Completed
        ↓
nicht Completed
```

Falls ein gültiger MicroCheckpoint vorhanden ist, darf von diesem fortgesetzt werden.

## Input-Gültigkeit

Persistierte Referenzen müssen beim Resume erneut geprüft werden.

Mögliche Fälle:

```text
VALID
CHANGED
MISSING
INACCESSIBLE
INCOMPATIBLE
```

Bei veränderten Inputs entscheidet die jeweilige Policy, ob:

```text
continue
revalidate
replan
request_user
fail
```

erforderlich ist.

## Versionierung

Persistierte Intents müssen ihre verwendeten Schema-Versionen speichern.

Beispiel:

```text
intent_schema:
    media.audio.transcribe@2
```

Ist diese Version nicht mehr direkt unterstützt, darf eine definierte Migration verwendet werden.

Migrationen dürfen die ursprüngliche Bedeutung des Intents nicht stillschweigend verändern.

## Parent- und Child-Intents

Bei zusammengesetzten Intents müssen Parent-/Child-Beziehungen persistent erhalten bleiben.

Beispiel:

```text
Parent
    ├── Child A: Completed
    ├── Child B: Paused
    └── Child C: Ready
```

Nach dem Resume dürfen bereits erfolgreich abgeschlossene Child-Intents nicht unnötig erneut ausgeführt werden.

## Idempotenz

Beim Resume muss berücksichtigt werden, ob bereits ausgeführte Operationen wiederholbar sind.

Nicht idempotente Aktionen wie:

- Nachrichten versenden
- externe Veröffentlichungen
- Zahlungen
- Geräteaktionen

dürfen nach einem Crash nicht blind erneut ausgeführt werden.

Der bestätigte Ausführungszustand muss vor einem Retry geprüft werden.

## TaskCapsule-Integration

Ein persistierter Intent darf Bestandteil einer `Nova.TaskCapsule` sein.

Dabei müssen mindestens erhalten bleiben:

- Intent-Identität
- Lifecycle-Zustand
- Abhängigkeiten
- relevante Referenzen
- Constraints
- Policies

Nach Migration auf ein anderes System erfolgt erneut Validierung und Resolution.

## Aufbewahrung

Persistierte Intent-Zustände dürfen nach Abschluss bereinigt werden.

Die Retention Policy kann abhängig sein von:

- Audit-Anforderungen
- Causality
- Evidence
- Undo
- StateTime
- Benutzerkonfiguration

Ein gelöschter Laufzeitzustand darf notwendige Nachweis- oder Recovery-Daten nicht unbeabsichtigt entfernen.

## Normative Anforderungen

1. Intents MÜSSEN unabhängig von einzelnen Prozessen persistent speicherbar sein.
2. Persistierte Zustände MÜSSEN atomar und konsistent sein.
3. Resume MUSS relevante Inputs, Policies, Constraints und Abhängigkeiten erneut prüfen.
4. Ein Intent DARF nach einem Crash nur mit bestätigtem Completion Record als `Completed` gelten.
5. Nicht mehr gültige Ausführungspläne MÜSSEN durch Re-Resolution oder Replanning ersetzt werden können.
6. Schema-Versionen MÜSSEN mit dem Intent gespeichert werden.
7. Nicht idempotente Operationen MÜSSEN beim Resume vor Doppel-Ausführung geschützt werden.
8. Parent-/Child-Zustände MÜSSEN über Persistenz und Resume hinweg erhalten bleiben.

## Abgrenzung

Diese NPSPEC definiert:

- Intent-Persistenz
- Wiederherstellung
- Resume
- Revalidierung
- grundlegende Resume-Sicherheit

Nicht Bestandteil sind:

- konkrete Speicherformate
- MicroCheckpoint-Implementierung
- TaskCapsule-Format
- StateTime-Snapshots
- konkrete Migrationsalgorithmen

## Zugehörige NPSPECs

- `NPSPEC-INTENT-0001 – Intent Object Model`
- `NPSPEC-INTENT-0002 – Intent Lifecycle`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`
- `NPSPEC-INTENT-0004 – Intent Resolution`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`
- `NPSPEC-INTENT-0006 – Intent Composition`
- `NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model`
- `NPSPEC-CAPSULE-0001 – TaskCapsule Container Format`