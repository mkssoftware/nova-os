# NPSPEC-CAPSULE-0003 – Capability & Dependency Manifest

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das Capability- und Dependency-Manifest einer `Nova.TaskCapsule`.

Das Manifest beschreibt, welche Fähigkeiten, Datenmodelle und externen Voraussetzungen benötigt werden, um eine gespeicherte Aufgabe korrekt fortzusetzen.

## Grundprinzip

```text
TaskCapsule
    ↓
Dependency Manifest
    ├── Required Capabilities
    ├── Semantic Types
    ├── Schemas
    ├── Execution Requirements
    └── External Dependencies
```

Das Manifest soll bevorzugt Anforderungen beschreiben und keine unnötige Bindung an konkrete Implementierungen erzeugen.

## Manifest

Die logische Grundstruktur lautet:

```text
DependencyManifest {
    capabilities
    semantic_types
    schemas
    execution_requirements
    external_dependencies
}
```

## Capability Requirements

Benötigte Fähigkeiten sollen primär semantisch beschrieben werden.

Bevorzugt:

```text
capabilities {
    media.audio.decode
    media.audio.transcribe
    document.write
}
```

statt ausschließlich:

```text
nova.speech.engine.v4
```

Dadurch kann NovaOS eine kompatible alternative Capability verwenden.

## Konkrete Capability-Bindung

Falls der Zustand tatsächlich von einer bestimmten Implementierung abhängt, darf eine konkrete Capability verlangt werden.

Beispiel:

```text
capability {
    semantic_requirement:
        simulation.physics.solve

    implementation:
        nova.physics.solver

    version:
        3

    binding:
        REQUIRED
}
```

Mögliche Bindungsarten:

```text
SEMANTIC
PREFERRED
REQUIRED
```

## Semantic Types

Alle für Resume relevanten Semantic Types müssen referenzierbar sein.

Beispiel:

```text
semantic_types {
    Media.Audio@2
    Document.Transcript@1
}
```

Unbekannte zwingende Typen können die Wiederaufnahme verhindern.

## Schema-Abhängigkeiten

Verwendete Schemas müssen mit Version angegeben werden können.

Beispiele:

```text
schemas {
    media.audio.transcribe@2
    Nova.TaskState@1
}
```

NovaOS darf kompatible Schema-Migrationen verwenden.

## Execution Requirements

Das Manifest darf notwendige Ausführungsbedingungen enthalten.

Beispiele:

```text
execution_requirements {
    deterministic:
        true

    minimum_memory:
        512MiB

    required_precision:
        FP32_or_better
}
```

Bestehende `Execution Contracts` können zusätzlich referenziert werden.

## Hardwareabhängigkeiten

Hardware soll nur dann zwingend gebunden werden, wenn die Aufgabe tatsächlich davon abhängt.

Beispiel:

```text
hardware {
    type:
        GPU

    required:
        false
}
```

Bevorzugt werden semantische Anforderungen:

```text
requires:
    compute.matrix.accelerated
```

NovaOS darf anschließend geeignete Hardware auswählen.

## Externe Dependencies

Eine Capsule darf externe Abhängigkeiten besitzen.

Beispiele:

```text
remote_service
device
dataset
network_resource
external_identity
```

Beispiel:

```text
external_dependency {
    type:
        device

    semantic_requirement:
        Sensor.Temperature

    required:
        true
}
```

Beim Resume müssen externe Abhängigkeiten erneut geprüft werden.

## Abhängigkeitsstatus

Abhängigkeiten können beim Laden mindestens folgende Zustände erhalten:

```text
AVAILABLE
COMPATIBLE
SUBSTITUTABLE
MISSING
INCOMPATIBLE
BLOCKED
```

`SUBSTITUTABLE` bedeutet, dass eine kompatible Alternative verwendet werden kann.

## Alternative Capabilities

Ist die ursprüngliche Capability nicht verfügbar, darf NovaOS eine Alternative einsetzen, wenn:

- die semantische Aufgabe identisch erfüllt wird
- Input- und Output-Typen kompatibel sind
- Constraints und Policies eingehalten werden
- gespeicherter Zustand übertragbar oder rekonstruierbar ist

Beispiel:

```text
Original:
    GPU SpeechRecognizer

Alternative:
    CPU SpeechRecognizer
```

## Abhängigkeitsgraph

Dependencies dürfen voneinander abhängen.

Beispiel:

```text
Capability A
    ↓ requires
Semantic Type B
    ↓ requires
Schema C
```

Das Manifest muss solche Abhängigkeiten eindeutig darstellen können.

Unbeabsichtigte oder nicht auflösbare Zyklen müssen erkannt werden.

## Optionale Dependencies

Nicht jede Abhängigkeit muss zwingend erforderlich sein.

Beispiel:

```text
dependency {
    capability:
        media.chapter.detect

    required:
        false
}
```

Fehlt diese Capability, darf NovaOS den Task gegebenenfalls mit reduzierter Funktion fortsetzen.

## Manifest Validation

Vor Resume muss geprüft werden:

```text
capabilities
versions
semantic_types
schemas
hardware
external_dependencies
policies
```

Ergebnis kann mindestens sein:

```text
READY
SUBSTITUTION_REQUIRED
MIGRATION_REQUIRED
BLOCKED
INCOMPATIBLE
```

## Beispiel

```text
DependencyManifest {
    capabilities {
        media.audio.decode {
            binding:
                SEMANTIC
        }

        media.audio.transcribe {
            binding:
                SEMANTIC
        }
    }

    semantic_types {
        Media.Audio@2
        Document.Transcript@1
    }

    schemas {
        media.audio.transcribe@2
    }

    execution_requirements {
        local_only:
            true
    }

    external_dependencies {
        object:audio:42
    }
}
```

## Normative Anforderungen

1. Eine TaskCapsule MUSS ihre für Resume notwendigen Abhängigkeiten beschreiben können.
2. Capability-Anforderungen SOLLEN bevorzugt semantisch statt implementierungsspezifisch formuliert werden.
3. Zwingende konkrete Implementierungsbindungen MÜSSEN explizit gekennzeichnet sein.
4. Schema- und Semantic-Type-Versionen MÜSSEN referenzierbar sein.
5. Fehlende Capabilities MÜSSEN durch kompatible Alternativen ersetzbar sein können, sofern die Semantik erhalten bleibt.
6. Externe Abhängigkeiten MÜSSEN beim Resume erneut validiert werden.
7. Optionale und zwingende Abhängigkeiten MÜSSEN unterscheidbar sein.
8. Das Manifest MUSS vor Resume vollständig validierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Capability Manifest
- Dependency Manifest
- semantische und konkrete Capability-Bindungen
- Schema- und Typabhängigkeiten
- externe Dependencies

Nicht Bestandteil sind:

- Ressourcen-Packaging
- Security und Identity
- konkrete Capability-Installation
- Migration und Resume
- allgemeiner Compatibility Graph

## Zugehörige NPSPECs

- `NPSPEC-CAPSULE-0001 – TaskCapsule Container Format`
- `NPSPEC-CAPSULE-0002 – Task State Capture`
- `NPSPEC-CAPSULE-0004 – Resource Packaging & References`
- `NPSPEC-CAPSULE-0005 – Capsule Security & Identity`
- `NPSPEC-CAPSULE-0006 – Capsule Migration & Resume`
- `NPSPEC-CAPSULE-0007 – Capsule Versioning & Compatibility`
- `NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor`
- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`