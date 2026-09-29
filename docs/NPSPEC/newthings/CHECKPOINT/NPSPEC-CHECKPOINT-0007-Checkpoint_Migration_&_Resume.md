# NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie ein `MicroCheckpoint` auf ein anderes System, Gerät oder Ausführungsziel übertragen und dort wieder aufgenommen werden kann.

Ziel ist, laufende Aufgaben fortsetzen zu können, ohne an dieselbe Hardware oder denselben Prozess gebunden zu sein.

## Grundprinzip

```text
System A
    ↓
Checkpoint
    ↓
Migration
    ↓
System B
    ↓
Validation
    ↓
Resume
```

## Migrationsumfang

Ein migrierbarer Checkpoint muss mindestens beschreiben können:

- Ausführungszustand
- benötigte Daten
- Objekt-Referenzen
- Capability-Abhängigkeiten
- Execution Contracts
- Precision-Anforderungen
- relevante Policies
- Checkpoint-Version

Nicht übertragbare lokale Ressourcen müssen explizit gekennzeichnet werden.

## Migration Package

Die Migration kann logisch enthalten:

```text
CheckpointMigration {
    checkpoint_id
    execution_state
    data_state
    dependencies
    compatibility_requirements
    policies
    integrity
}
```

Große unveränderte Daten dürfen referenziert oder separat übertragen werden.

## Kompatibilitätsprüfung

Vor Resume muss das Zielsystem prüfen:

```text
architecture
capabilities
schema_versions
execution_ir_version
semantic_types
precision_support
permissions
policies
```

Ein Checkpoint darf nicht allein deshalb als kompatibel gelten, weil das Zielformat gelesen werden kann.

## Hardwareunabhängigkeit

Checkpoints sollen möglichst logischen statt rein hardwaregebundenen Zustand speichern.

Bevorzugt:

```text
execution_node: 17
logical_state: solver_iteration_120
```

statt ausschließlich:

```text
CPU register dump
```

Hardwareabhängiger Zustand darf enthalten sein, benötigt dann aber eine passende Restore- oder Transformationsstrategie.

## Capability Mapping

Ist die ursprüngliche Capability auf dem Zielsystem nicht verfügbar, darf eine kompatible Alternative verwendet werden.

Beispiel:

```text
System A:
    nova.compute.gpu_solver

System B:
    nova.compute.cpu_solver
```

Voraussetzung:

- gleiche semantische Aufgabe
- kompatibler Zustand
- gültige Contracts
- ausreichender Trust

## Execution Replanning

Ein migrierter Checkpoint darf Replanning erfordern.

Beispiel:

```text
Original:
    GPU FP32

Target:
    keine GPU

Resume:
    Replan
    ↓
CPU FP64
```

Das ursprüngliche Intent-Ziel darf dadurch nicht verändert werden.

## Nicht migrierbare Ressourcen

Bestimmte Ressourcen können nicht direkt übertragen werden.

Beispiele:

```text
device_handle
open_socket
local_process_id
hardware_queue
temporary_kernel_object
```

Solche Ressourcen müssen beim Resume:

```text
recreate
reconnect
remap
replace
```

werden.

Kann dies nicht sicher erfolgen, darf der Resume nicht fortgesetzt werden.

## Externe Verbindungen

Externe Sessions oder Services müssen nach Migration erneut validiert werden.

Beispiel:

```text
network session
    ↓
reconnect
    ↓
verify remote state
```

Eine alte Verbindung darf nicht als weiterhin gültig angenommen werden.

## Datenlokalität

Ein Checkpoint darf Daten referenzieren, die auf dem Zielsystem nicht vorhanden sind.

Vor Resume muss NovaOS bestimmen, ob diese Daten:

```text
transfer
fetch
remap
already_available
unavailable
```

sind.

Fehlende zwingende Daten blockieren den Resume.

## Sicherheit

Migration darf bestehende Sicherheitsgrenzen nicht abschwächen.

Zu prüfen sind insbesondere:

- Identität
- Berechtigungen
- Information-Flow-Labels
- Verschlüsselung
- Trust-Level
- lokale Policies

Ein Zielsystem darf keine Rechte allein aus dem Checkpoint übernehmen, wenn diese dort nicht autorisiert sind.

## Integrität

Der übertragene Checkpoint muss auf Integrität geprüft werden.

Beispiel:

```text
hash
signature
version
dependency_manifest
```

Beschädigte oder manipulierte Migration Packages dürfen nicht ausgeführt werden.

## Resume

Nach erfolgreicher Migration erfolgt:

```text
Validate
    ↓
Restore State
    ↓
Remap Dependencies
    ↓
Replan if required
    ↓
Resume
```

Der Resume beginnt am im Checkpoint definierten sicheren Fortsetzungspunkt.

## Resume Result

Mindestens folgende Ergebnisse müssen unterscheidbar sein:

```text
RESUMED
REPLAN_REQUIRED
PARTIAL
INCOMPATIBLE
BLOCKED
FAILED
```

Ein nicht vollständig migrierbarer Zustand darf nicht als erfolgreicher Resume dargestellt werden.

## TaskCapsule-Integration

MicroCheckpoints dürfen Bestandteil einer `Nova.TaskCapsule` sein.

Beispiel:

```text
TaskCapsule
    ├── Intent
    ├── Data References
    ├── Capability Manifest
    └── MicroCheckpoint
```

Damit kann eine komplette Aufgabe mitsamt laufendem Zustand übertragen werden.

## Beispiel

Ausgangssystem:

```text
Device:
    Desktop

Execution:
    GPU Solver

Checkpoint:
    node 41
    iteration 250
```

Zielsystem:

```text
Device:
    Laptop

GPU Solver:
    unavailable

CPU Solver:
    compatible
```

Migration:

```text
Checkpoint Transfer
    ↓
Capability Mapping
    ↓
Replan
    ↓
CPU Solver
    ↓
Resume at iteration 250
```

## Normative Anforderungen

1. Migrierbare Checkpoints MÜSSEN ihren notwendigen Ausführungs- und Abhängigkeitszustand enthalten oder referenzieren.
2. Das Zielsystem MUSS Kompatibilität vor Resume prüfen.
3. Nicht übertragbare Ressourcen MÜSSEN explizit erkannt und neu gebunden werden.
4. Capabilities DÜRFEN durch kompatible Alternativen ersetzt werden, wenn die semantische Wirkung erhalten bleibt.
5. Migration DARF bestehende Constraints, Policies oder Sicherheitsanforderungen nicht abschwächen.
6. Externe Verbindungen MÜSSEN nach Migration erneut validiert werden.
7. Übertragene Checkpoints MÜSSEN auf Integrität prüfbar sein.
8. Ein fehlgeschlagener oder partieller Resume MUSS eindeutig als solcher erkennbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Checkpoint-Migration
- Kompatibilitätsprüfung
- Dependency Remapping
- Resume auf anderer Hardware
- grundlegende TaskCapsule-Integration

Nicht Bestandteil sind:

- TaskCapsule-Containerformat
- Netzwerktransportprotokoll
- konkrete Capability-Adapter
- Hardware-Lowering
- allgemeine Remote Execution

## Zugehörige NPSPECs

- `NPSPEC-CHECKPOINT-0001 – MicroCheckpoint Object Model`
- `NPSPEC-CHECKPOINT-0002 – Safe Checkpoint Points`
- `NPSPEC-CHECKPOINT-0003 – Incremental State Capture`
- `NPSPEC-CHECKPOINT-0004 – Checkpoint Consistency`
- `NPSPEC-CHECKPOINT-0005 – Crash Recovery`
- `NPSPEC-CHECKPOINT-0006 – Checkpoint Resource & Retention Policy`
- `NPSPEC-CAPSULE-0001 – TaskCapsule Container Format`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`