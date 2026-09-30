# NPSPEC-CAPSULE-0004 – Resource Packaging & References

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Ressourcen innerhalb einer `Nova.TaskCapsule` eingebettet, referenziert oder extern gebunden werden.

Ziel ist, eine Aufgabe portabel fortsetzen zu können, ohne große oder bereits vorhandene Daten unnötig zu duplizieren.

## Grundprinzip

```text
TaskCapsule
    ↓
Resources
    ├── Embedded
    ├── Referenced
    ├── External
    └── Reconstructable
```

NovaOS wählt abhängig von Portabilität, Größe, Sicherheit und Verfügbarkeit die geeignete Form.

## Resource Descriptor

Eine Ressource kann logisch beschrieben werden als:

```text
ResourceDescriptor {
    id
    semantic_type
    storage_mode
    location
    integrity
    required
    mutability
}
```

## Storage Modes

Mindestens folgende Modi werden unterstützt:

```text
EMBEDDED
REFERENCED
EXTERNAL
RECONSTRUCTABLE
```

### `EMBEDDED`

Die Ressource befindet sich vollständig innerhalb der TaskCapsule.

Geeignet für:

- kleine Dateien
- Konfigurationsdaten
- nicht anderweitig verfügbare Zustände
- für Migration zwingend benötigte Daten

### `REFERENCED`

Die Capsule enthält lediglich eine stabile NovaOS-Objektreferenz.

Beispiel:

```text
resource {
    mode:
        REFERENCED

    ref:
        object:audio:42
}
```

### `EXTERNAL`

Die Ressource befindet sich außerhalb des lokalen NovaOS-Objektraums.

Beispiele:

```text
remote storage
network service
external database
device
```

Diese Ressourcen müssen beim Resume erneut aufgelöst werden.

### `RECONSTRUCTABLE`

Die Ressource muss nicht gespeichert werden, wenn sie deterministisch oder ausreichend zuverlässig neu erzeugt werden kann.

Beispiel:

```text
resource {
    mode:
        RECONSTRUCTABLE

    producer:
        node:audio-waveform
}
```

## Einbettungsentscheidung

Ressourcen sollen eingebettet werden, wenn ihre Verfügbarkeit für Resume nicht anderweitig zuverlässig gewährleistet werden kann.

Nicht automatisch eingebettet werden sollen:

- große unveränderte Dateien
- systemweit vorhandene Objekte
- Cache-Daten
- reproduzierbare Zwischenergebnisse

## Referenzstabilität

Referenzen müssen gegenüber kurzlebigen Laufzeitkennungen stabil sein.

Nicht geeignet:

```text
process_id
raw_pointer
temporary_handle
```

Bevorzugt:

```text
object_id
content_id
resource_uri
semantic_reference
```

## Content Identity

Ressourcen dürfen über ihren Inhalt identifiziert werden.

Beispiel:

```text
content_hash:
    sha256:...
```

Dadurch kann NovaOS erkennen, ob eine lokal vorhandene Ressource exakt der erwarteten Version entspricht.

## Mutable Resources

Veränderbare Ressourcen müssen ihren erwarteten Zustand eindeutig beschreiben.

Beispiel:

```text
resource {
    ref:
        object:document:81

    version:
        17
}
```

Beim Resume muss erkannt werden können, wenn die Ressource inzwischen verändert wurde.

Mögliche Reaktionen:

```text
USE_CURRENT
USE_CAPTURED
RECONCILE
REPLAN
BLOCK
```

Die konkrete Konfliktentscheidung wird durch höhere Policies bestimmt.

## Embedded Resources

Eingebettete Daten müssen eindeutig einem Resource Descriptor zugeordnet sein.

Logisch:

```text
resources/
    manifest
    payload/
        resource-001
        resource-002
```

Das physische Containerlayout wird separat spezifiziert.

## Deduplication

Identische Ressourcen sollen innerhalb einer Capsule nicht mehrfach gespeichert werden.

Beispiel:

```text
Node A ─┐
        ├── object:audio:42
Node B ─┘
```

Beide Nodes referenzieren dieselbe Ressource.

Content-basierte Deduplication darf zusätzlich verwendet werden.

## Lazy Loading

Große Ressourcen dürfen erst geladen werden, wenn sie tatsächlich benötigt werden.

```text
TaskCapsule opened
    ↓
Manifest loaded
    ↓
Task resumed
    ↓
Resource requested
    ↓
Resource loaded
```

Dadurch muss nicht der gesamte Capsule-Inhalt beim Öffnen in den Speicher geladen werden.

## Teilweises Packaging

NovaOS darf nur die für eine konkrete Migration notwendigen Ressourcen einbetten.

Beispiel:

```text
Local TaskCapsule:
    references local dataset

Migration:
    target does not contain dataset

Result:
    required subset embedded
```

Damit kann Packaging abhängig vom Zielsystem erfolgen.

## Externe Ressourcen

Externe Ressourcen müssen ausreichend beschrieben werden, um sie erneut aufzulösen.

Beispiel:

```text
external_resource {
    semantic_type:
        Dataset.Map

    location:
        remote://provider/dataset

    required:
        true
}
```

Zugangsdaten dürfen nicht ungeschützt innerhalb der Resource Reference gespeichert werden.

## Temporäre Ressourcen

Kurzlebige Ressourcen müssen beim Packaging entweder:

```text
embed
persist
reconstruct
discard
```

werden.

Eine TaskCapsule darf nicht von einer Ressource abhängig bleiben, deren Lebensdauer bereits beendet ist.

## Resource Ownership

Der Descriptor muss unterscheiden können zwischen:

```text
OWNED
BORROWED
SHARED
EXTERNAL
```

`OWNED` Ressourcen gehören logisch zur Aufgabe.

`BORROWED` und `SHARED` Ressourcen bleiben außerhalb der Capsule bestehen.

## Sicherheit

Packaging darf bestehende Zugriffsrechte nicht umgehen.

Eine eingebettete Ressource muss ihre relevanten Schutzinformationen erhalten können.

Beispiele:

```text
information_labels
owner
access_policy
confidentiality
integrity_metadata
```

Beim Resume werden die Berechtigungen erneut geprüft.

## Integrität

Für jede eingebettete oder referenzierte Ressource muss Integritätsprüfung möglich sein.

Beispiel:

```text
resource {
    id:
        resource:42

    hash:
        sha256:...
}
```

Manipulierte oder unerwartet veränderte Ressourcen müssen erkannt werden.

## Fehlende Ressourcen

Beim Resume können Ressourcen fehlen.

Mögliche Zustände:

```text
AVAILABLE
RELOCATED
RECONSTRUCTABLE
MISSING_OPTIONAL
MISSING_REQUIRED
MODIFIED
```

Eine fehlende optionale Ressource darf eine reduzierte Fortsetzung erlauben.

Eine fehlende zwingende Ressource muss zu Replanning, Wiederherstellung oder Abbruch führen.

## Beispiel

```text
resources {
    source_audio {
        semantic_type:
            Media.Audio

        mode:
            REFERENCED

        ref:
            object:audio:42

        required:
            true
    }

    transcript_partial {
        semantic_type:
            Document.Transcript

        mode:
            EMBEDDED

        required:
            true
    }

    waveform_cache {
        semantic_type:
            Media.Waveform

        mode:
            RECONSTRUCTABLE

        required:
            false
    }
}
```

Beim Transfer der Capsule muss dadurch nur der tatsächlich notwendige Zustand transportiert werden.

## Normative Anforderungen

1. TaskCapsules MÜSSEN eingebettete, referenzierte und externe Ressourcen unterscheiden können.
2. Ressourcenreferenzen MÜSSEN gegenüber kurzlebigen Prozess- und Speicherkennungen stabil sein.
3. Große unveränderte Ressourcen SOLLEN nicht unnötig dupliziert werden.
4. Veränderbare Ressourcen MÜSSEN auf Versions- oder Zustandsänderungen prüfbar sein.
5. Identische Ressourcen SOLLEN innerhalb einer Capsule dedupliziert werden.
6. Fehlende zwingende Ressourcen MÜSSEN vor erfolgreichem Resume erkannt werden.
7. Packaging DARF bestehende Sicherheits- und Information-Flow-Regeln nicht umgehen.
8. Eingebettete und referenzierte Ressourcen MÜSSEN auf Integrität prüfbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Resource Packaging
- eingebettete und referenzierte Ressourcen
- externe Ressourcen
- Deduplication
- Lazy Loading
- Resource Integrity

Nicht Bestandteil sind:

- Task State Capture
- Capability Dependency Manifest
- Capsule Security im Detail
- konkrete Komprimierungsverfahren
- Transportprotokolle
- Dateisystemlayout des Containers

## Zugehörige NPSPECs

- `NPSPEC-CAPSULE-0001 – TaskCapsule Container Format`
- `NPSPEC-CAPSULE-0002 – Task State Capture`
- `NPSPEC-CAPSULE-0003 – Capability & Dependency Manifest`
- `NPSPEC-CAPSULE-0005 – Capsule Security & Identity`
- `NPSPEC-CAPSULE-0006 – Capsule Migration & Resume`
- `NPSPEC-CAPSULE-0007 – Capsule Versioning & Compatibility`
- `NPSPEC-CHECKPOINT-0003 – Incremental State Capture`
- `NPSPEC-INFOFLOW-0008 – Persistent Data Flow Metadata`