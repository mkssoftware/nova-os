# NPSPEC-CAPSULE-0001 – TaskCapsule Container Format

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das grundlegende Containerformat von `Nova.TaskCapsule`.

Eine TaskCapsule kapselt eine vollständige Arbeitsaufgabe einschließlich ihres logischen Zustands, sodass sie gespeichert, pausiert, übertragen und auf einem kompatiblen NovaOS-System fortgesetzt werden kann.

## Grundprinzip

```text
Task
    ↓
TaskCapsule
    ├── Intent
    ├── State
    ├── Data References
    ├── Capability Manifest
    ├── Execution Context
    └── Metadata
```

Eine TaskCapsule ist keine klassische Anwendung und kein vollständiges Systemabbild.

Sie enthält nur die Informationen, die zur Beschreibung und Fortsetzung einer Aufgabe erforderlich sind.

## Containerstruktur

Die logische Grundstruktur lautet:

```text
TaskCapsule {
    header
    identity
    manifest
    intents
    state
    resources
    dependencies
    checkpoints
    metadata
    integrity
}
```

Die konkrete binäre Repräsentation wird separat festgelegt.

## Header

Der Header enthält grundlegende Informationen über das Format.

Beispiel:

```text
header {
    format:
        Nova.TaskCapsule

    version:
        1
}
```

Der Header muss eine eindeutige Erkennung und Versionsprüfung ermöglichen.

## Identität

Jede TaskCapsule besitzt eine eindeutige Identität.

Beispiel:

```text
capsule:01K...
```

Die Identität bleibt bei normaler Speicherung und Übertragung erhalten.

Eine bewusste Duplizierung kann eine neue Capsule-Identität erzeugen.

## Manifest

Das Manifest beschreibt den Inhalt der Capsule.

Beispiel:

```text
manifest {
    root_intent
    child_intents
    capabilities
    resources
    checkpoints
}
```

Das Manifest ermöglicht es NovaOS, die Voraussetzungen einer Capsule zu prüfen, ohne sämtliche enthaltenen Daten vollständig laden zu müssen.

## Intents

Eine TaskCapsule muss ihren zugrunde liegenden Intent referenzieren können.

Beispiel:

```text
root_intent:
    intent:01JQX7...
```

Zusammengesetzte Aufgaben dürfen zusätzlich ihre Child-Intents enthalten.

```text
intents {
    root
    child_1
    child_2
}
```

Lifecycle-Zustände müssen erhalten bleiben.

## Task State

Der Task-Zustand beschreibt den logischen Arbeitszustand.

Beispiele:

```text
active_step
completed_steps
pending_steps
execution_graph
open_objects
task_context
```

Der Zustand soll möglichst unabhängig von konkreten Prozessen oder Prozess-IDs sein.

## Ressourcen

Daten können entweder eingebettet oder referenziert werden.

```text
resources {
    embedded
    referenced
}
```

### Eingebettete Ressourcen

Geeignet für kleinere oder für die Portabilität zwingend benötigte Daten.

### Referenzierte Ressourcen

Geeignet für große oder bereits systemweit verfügbare Daten.

Beispiel:

```text
resource {
    ref:
        object:dataset:81

    required:
        true
}
```

## Capability Manifest

Die Capsule beschreibt, welche Capabilities zur Fortsetzung benötigt werden.

Beispiel:

```text
capabilities {
    nova.media.decoder
    nova.speech.transcriber
    nova.document.writer
}
```

Dabei sollen semantische Anforderungen gespeichert werden, nicht ausschließlich konkrete Implementierungen.

Bevorzugt:

```text
requires:
    media.audio.transcribe
```

statt ausschließlich:

```text
requires:
    specific.transcriber.v4
```

Dadurch kann NovaOS kompatible alternative Capabilities einsetzen.

## Abhängigkeiten

Eine Capsule muss externe Voraussetzungen beschreiben können.

Beispiele:

```text
semantic_types
schema_versions
execution_contracts
hardware_requirements
external_services
device_requirements
```

Nicht erfüllte Abhängigkeiten müssen vor Resume erkannt werden.

## MicroCheckpoints

Eine TaskCapsule darf einen oder mehrere `Nova.MicroCheckpoint` enthalten oder referenzieren.

Beispiel:

```text
checkpoints {
    current:
        checkpoint:2048
}
```

Dadurch kann eine laufende Aufgabe möglichst nahe am vorherigen Zustand fortgesetzt werden.

Fehlt ein gültiger Checkpoint, kann die Capsule dennoch ihren logischen Task-Zustand enthalten.

## Portabilität

Das Containerformat soll hardware- und geräteunabhängig sein.

Hardwareabhängige Zustände dürfen enthalten sein, müssen aber klar gekennzeichnet werden.

Beispiel:

```text
dependency {
    type:
        hardware_state

    portable:
        false
}
```

Beim Öffnen auf einem anderen System kann Replanning erforderlich sein.

## Sicherheit

Eine TaskCapsule überträgt keine impliziten Berechtigungen.

Beim Laden müssen erneut geprüft werden:

- Identität
- Berechtigungen
- Policies
- Information-Flow-Labels
- Capability Trust
- Ressourcenrechte

Ein zuvor erlaubter Zugriff darf auf einem anderen System nicht automatisch übernommen werden.

## Integrität

Eine Capsule muss auf Beschädigung und Manipulation prüfbar sein.

Mögliche Mechanismen:

```text
hash
manifest_hash
signature
object_hashes
```

Die konkrete kryptografische Ausgestaltung wird separat spezifiziert.

## Versionierung

Das Containerformat muss versioniert sein.

Beispiel:

```text
Nova.TaskCapsule@1
```

Neue Versionen dürfen zusätzliche optionale Strukturen ergänzen.

Inkompatible Änderungen müssen eindeutig erkennbar sein.

## Erweiterbarkeit

Das Format muss Erweiterungen zulassen.

Beispiel:

```text
extensions {
    nova.simulation
    vendor.example
}
```

Unbekannte optionale Erweiterungen dürfen ignoriert werden, sofern dadurch keine für Resume notwendige Semantik verloren geht.

Unbekannte zwingende Erweiterungen müssen zur Ablehnung oder zu einem kompatiblen Migrationspfad führen.

## Beispiel

```text
TaskCapsule {
    header {
        format:
            Nova.TaskCapsule

        version:
            1
    }

    identity:
        capsule:7812

    manifest {
        root_intent:
            intent:podcast.publish

        resources {
            object:audio:42
            object:project:81
        }

        capabilities {
            media.audio.clean
            media.audio.transcribe
            document.write
        }
    }

    state {
        active_step:
            transcription

        progress:
            0.62
    }

    checkpoints {
        current:
            checkpoint:2048
    }

    integrity {
        manifest_hash:
            ...
    }
}
```

Diese Capsule kann gespeichert und später auf demselben oder einem kompatiblen NovaOS-System fortgesetzt werden.

## Normative Anforderungen

1. Jede TaskCapsule MUSS eindeutig identifizierbar und versioniert sein.
2. Eine Capsule MUSS ihren Root Intent und relevanten Task-Zustand referenzieren können.
3. Ressourcen MÜSSEN eingebettet oder eindeutig referenziert werden können.
4. Capability-Abhängigkeiten SOLLEN möglichst semantisch beschrieben werden.
5. TaskCapsules DÜRFEN keine impliziten Berechtigungen übertragen.
6. Der Container MUSS auf Integrität prüfbar sein.
7. Hardwareabhängige Bestandteile MÜSSEN als solche erkennbar sein.
8. Unbekannte zwingende Formatbestandteile DÜRFEN nicht stillschweigend ignoriert werden.

## Abgrenzung

Diese NPSPEC definiert:

- grundlegendes TaskCapsule-Containerformat
- Manifest
- Intent- und Zustandsreferenzen
- Ressourcen
- Capability-Abhängigkeiten
- Versionierung und Integrität

Nicht Bestandteil sind:

- detaillierte Zustandserfassung
- Ressourcen-Packaging
- Sicherheits- und Identitätsmodell im Detail
- Migration und Resume
- Versionsmigration

## Zugehörige NPSPECs

- `NPSPEC-CAPSULE-0002 – Task State Capture`
- `NPSPEC-CAPSULE-0003 – Capability & Dependency Manifest`
- `NPSPEC-CAPSULE-0004 – Resource Packaging & References`
- `NPSPEC-CAPSULE-0005 – Capsule Security & Identity`
- `NPSPEC-CAPSULE-0006 – Capsule Migration & Resume`
- `NPSPEC-CAPSULE-0007 – Capsule Versioning & Compatibility`
- `NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`