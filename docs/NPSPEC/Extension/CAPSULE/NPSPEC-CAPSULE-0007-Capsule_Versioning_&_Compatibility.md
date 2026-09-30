# NPSPEC-CAPSULE-0007 – Capsule Versioning & Compatibility

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Versionierung und Kompatibilitätsprüfung von `Nova.TaskCapsule`.

Ziel ist, dass Capsules auch nach Änderungen an NovaOS, Capabilities, Semantic Types oder Datenstrukturen möglichst weiterhin geöffnet, migriert und fortgesetzt werden können.

## Grundprinzip

```text
TaskCapsule Version
    ↓
Compatibility Check
    ↓
COMPATIBLE
MIGRATABLE
PARTIALLY_COMPATIBLE
INCOMPATIBLE
```

Versionierung dient nicht nur der Dateiformat-Erkennung, sondern der Bewertung des gesamten für Resume notwendigen Zustands.

## Versionsebenen

Eine TaskCapsule kann mehrere unabhängige Versionen enthalten:

```text
container_version
task_state_version
manifest_version
semantic_type_versions
schema_versions
capability_versions
execution_ir_version
extension_versions
```

Diese Versionen müssen getrennt bewertet werden können.

## Container Version

Das grundlegende Capsule-Format besitzt eine eigene Version.

Beispiel:

```text
format:
    Nova.TaskCapsule

version:
    2
```

Eine Änderung der Container-Version bedeutet nicht automatisch, dass der enthaltene Task inkompatibel ist.

## Kompatibilitätsklassen

Mindestens folgende Ergebnisse müssen unterstützt werden:

```text
COMPATIBLE
MIGRATABLE
PARTIALLY_COMPATIBLE
INCOMPATIBLE
```

### `COMPATIBLE`

Die Capsule kann ohne semantische Änderung verwendet werden.

### `MIGRATABLE`

Die Capsule benötigt eine definierte Transformation auf eine neuere Struktur.

### `PARTIALLY_COMPATIBLE`

Ein Teil des Zustands ist verwendbar, während andere Bestandteile ersetzt, rekonstruiert oder neu geplant werden müssen.

### `INCOMPATIBLE`

Wesentliche Semantik kann nicht zuverlässig wiederhergestellt werden.

## Backward Compatibility

Neuere NovaOS-Versionen sollen ältere TaskCapsules möglichst lesen können.

Beispiel:

```text
NovaOS:
    Capsule Format v4

Input:
    Capsule Format v2
```

Falls ein definierter Upgrade-Pfad vorhanden ist:

```text
v2
 ↓
v3
 ↓
v4
```

darf die Capsule migriert werden.

## Forward Compatibility

Ältere Systeme dürfen unbekannte optionale Felder ignorieren, wenn deren Fehlen die Semantik nicht verändert.

Beispiel:

```text
extension:
    optional
```

Unbekannte zwingende Bestandteile dürfen nicht stillschweigend ignoriert werden.

Beispiel:

```text
extension:
    required
```

Ergebnis:

```text
INCOMPATIBLE
```

oder:

```text
MIGRATION_REQUIRED
```

## Semantic Compatibility

Versionsnummern allein bestimmen nicht die Kompatibilität.

Entscheidend ist, ob die Bedeutung erhalten bleibt.

Beispiel:

```text
Semantic Type v1
    ↓ compatible evolution
Semantic Type v2
```

kann kompatibel sein.

Dagegen kann eine Änderung derselben Struktur semantisch inkompatibel sein, wenn sich ihre Bedeutung verändert.

## Capability Compatibility

Eine TaskCapsule darf auch dann fortsetzbar sein, wenn die ursprünglich verwendete Capability nicht mehr existiert.

Beispiel:

```text
Original:
    nova.audio.transcriber.v2

Unavailable
```

Das Manifest verlangt semantisch:

```text
media.audio.transcribe
```

NovaOS darf eine kompatible neue Capability auswählen.

```text
nova.audio.transcriber.v5
```

Eine konkrete Implementierungsbindung verhindert dies nur, wenn sie ausdrücklich als zwingend markiert wurde.

## Schema Migration

Datenstrukturen innerhalb einer Capsule dürfen migriert werden.

Beispiel:

```text
TaskState@1
    ↓ migration
TaskState@2
```

Eine Migration muss deterministisch nachvollziehbar sein, sofern sie sicherheits- oder zustandsrelevante Daten verändert.

## Migration Chains

Mehrere Versionssprünge dürfen als Migrationskette ausgeführt werden.

```text
v1
 ↓ migrate
v2
 ↓ migrate
v3
 ↓ migrate
v4
```

NovaOS darf direkte Migrationen verwenden, wenn diese als äquivalent definiert sind.

```text
v1
 └────────→ v4
```

## Lossless und Lossy Migration

Migrationen müssen unterscheiden können zwischen:

```text
LOSSLESS
LOSSY
```

### `LOSSLESS`

Alle für die Task-Semantik relevanten Informationen bleiben erhalten.

### `LOSSY`

Ein Teil des Zustands kann nicht vollständig übertragen werden.

Eine verlustbehaftete Migration darf nicht stillschweigend als vollständig kompatibel gelten.

## Partial Compatibility

Eine Capsule kann teilweise kompatibel sein.

Beispiel:

```text
Intent:
    compatible

Resources:
    compatible

Execution Graph:
    incompatible

MicroCheckpoint:
    incompatible
```

NovaOS kann dann:

```text
discard execution graph
    ↓
restore logical task state
    ↓
replan
    ↓
resume
```

Damit führt technische Inkompatibilität nicht automatisch zum Verlust der gesamten Aufgabe.

## Compatibility Fallback

Beim Wiederherstellen soll NovaOS von der konkretesten zur abstrakteren Ebene zurückfallen können.

Bevorzugte Reihenfolge:

```text
MicroCheckpoint
    ↓
Execution State
    ↓
Task State
    ↓
Intent State
```

Ist eine Ebene inkompatibel, kann die nächsthöhere semantische Ebene verwendet werden.

## Compatibility Descriptor

Eine Capsule darf ihre Kompatibilitätsanforderungen explizit beschreiben.

Beispiel:

```text
compatibility {
    capsule_format:
        >= 2

    task_state:
        >= 1

    semantic_types {
        Media.Audio:
            >= 2
    }
}
```

Die Auswertung erfolgt gemeinsam mit `Nova.CompatibilityGraph`.

## Breaking Changes

Breaking Changes müssen eindeutig erkennbar sein.

Beispiele:

```text
removed mandatory field
changed semantic meaning
incompatible state representation
removed required capability semantics
changed security interpretation
```

Eine Breaking Change darf nicht lediglich durch eine unveränderte Versionskennung verborgen werden.

## Security-Kompatibilität

Ältere Sicherheitsinformationen dürfen nicht automatisch auf ein neueres Sicherheitsmodell übertragen werden.

Beispiel:

```text
Old Capsule:
    legacy permission model
```

Neues System:

```text
current authorization model
```

Die Capsule muss entsprechend neu bewertet werden.

Security-Migration darf Sicherheitsanforderungen nicht stillschweigend abschwächen.

## Information-Flow-Kompatibilität

Information-Flow-Labels müssen bei Migration erhalten oder in mindestens gleich restriktive aktuelle Labels überführt werden.

Beispiel:

```text
Old:
    confidential

New:
    restricted.private
```

Eine Migration darf nicht automatisch zu:

```text
public
```

führen.

## Version Pinning

Eine Capsule darf bestimmte Versionen zwingend festlegen.

Beispiel:

```text
require:
    simulation.model@3
```

Version Pinning soll nur verwendet werden, wenn die konkrete Version für korrekte Semantik erforderlich ist.

Zu starke Bindung reduziert die langfristige Portabilität.

## Upgrade ohne Veränderung des Originals

Eine ältere Capsule soll vor einer irreversiblen Migration erhalten bleiben können.

Beispiel:

```text
Capsule v2
    ↓ migrate
Capsule v3
```

Die ursprüngliche Version kann erhalten bleiben, bis die neue Capsule vollständig validiert wurde.

## Migration Validation

Nach einer Versionsmigration müssen mindestens geprüft werden:

```text
structure
semantic_types
dependencies
resources
security
information_flow
task_state
integrity
```

Erst danach darf die neue Version als gültige Capsule verwendet werden.

## Beispiel

Vorhandene Capsule:

```text
TaskCapsule@2

TaskState@1

SpeechCapability:
    nova.speech.old@3

Intent:
    media.audio.transcribe
```

Zielsystem:

```text
TaskCapsule@4

TaskState@3

nova.speech.old:
    unavailable

nova.speech.new:
    provides media.audio.transcribe
```

Kompatibilitätsprüfung:

```text
Container:
    MIGRATABLE

Task State:
    MIGRATABLE

Original Capability:
    unavailable

Semantic Capability:
    compatible
```

Migration:

```text
TaskCapsule@2
    ↓
TaskCapsule@4

TaskState@1
    ↓
TaskState@3

nova.speech.old
    ↓ semantic replacement
nova.speech.new
```

Ergebnis:

```text
MIGRATABLE
    ↓
VALID
    ↓
RESUME
```

## Normative Anforderungen

1. TaskCapsule-Format und relevante interne Strukturen MÜSSEN versioniert sein.
2. Kompatibilität MUSS anhand von Semantik und nicht ausschließlich anhand von Versionsnummern bewertet werden.
3. NovaOS MUSS zwischen `COMPATIBLE`, `MIGRATABLE`, `PARTIALLY_COMPATIBLE` und `INCOMPATIBLE` unterscheiden können.
4. Unbekannte zwingende Bestandteile DÜRFEN nicht stillschweigend ignoriert werden.
5. Verlustbehaftete Migrationen MÜSSEN als solche erkennbar sein.
6. Inkompatible technische Zustände SOLLEN auf eine höhere semantische Resume-Ebene zurückfallen können.
7. Sicherheits- und Information-Flow-Anforderungen DÜRFEN durch Versionsmigration nicht stillschweigend abgeschwächt werden.
8. Eine migrierte Capsule MUSS vor Resume erneut validiert werden.

## Abgrenzung

Diese NPSPEC definiert:

- Capsule-Versionierung
- Kompatibilitätsklassen
- Schema- und Formatmigration
- semantische Kompatibilität
- Partial Compatibility
- Compatibility Fallback

Nicht Bestandteil sind:

- allgemeiner Compatibility Graph
- konkrete Migrationsalgorithmen einzelner Schemas
- Containerformat im Detail
- Capability-Installation
- eigentliche Capsule-Übertragung

## Zugehörige NPSPECs

- `NPSPEC-CAPSULE-0001 – TaskCapsule Container Format`
- `NPSPEC-CAPSULE-0002 – Task State Capture`
- `NPSPEC-CAPSULE-0003 – Capability & Dependency Manifest`
- `NPSPEC-CAPSULE-0004 – Resource Packaging & References`
- `NPSPEC-CAPSULE-0005 – Capsule Security & Identity`
- `NPSPEC-CAPSULE-0006 – Capsule Migration & Resume`
- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`
- `NPSPEC-COMPAT-0003 – Compatibility Graph Model`
- `NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume`