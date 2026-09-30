# NPSPEC-CAPSULE-0006 – Capsule Migration & Resume

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie eine `Nova.TaskCapsule` zwischen Systemen, Geräten oder Ausführungsumgebungen übertragen und dort fortgesetzt wird.

Ziel ist, eine Aufgabe möglichst unabhängig vom ursprünglichen Gerät weiterzuführen, ohne ihren logischen Zustand, ihre Abhängigkeiten oder ihre Sicherheitsregeln zu verlieren.

## Grundprinzip

```text
Running Task
    ↓
TaskCapsule
    ↓
Transfer
    ↓
Target System
    ↓
Validation
    ↓
Restore / Replan
    ↓
Resume
```

Migration bedeutet nicht, dass der ursprüngliche technische Laufzeitzustand unverändert kopiert werden muss.

Bevorzugt wird die Wiederherstellung aus semantischem und logischem Zustand.

## Migrationsumfang

Eine Migration kann enthalten:

```text
Intent State
Task State
MicroCheckpoints
Resources
Capability Requirements
Execution Contracts
Security Metadata
Information-Flow Labels
```

Nur für die Fortsetzung relevante Informationen sollen übertragen werden.

## Migrationsarten

Mindestens folgende Formen müssen unterstützt werden können:

```text
LOCAL_MOVE
DEVICE_TO_DEVICE
SYSTEM_TO_SYSTEM
OFFLINE_TRANSFER
REMOTE_RESUME
```

### `LOCAL_MOVE`

Die Capsule wird innerhalb desselben Systems verschoben oder in einen anderen Ausführungskontext übernommen.

### `DEVICE_TO_DEVICE`

Die Aufgabe wird auf einem anderen Gerät fortgesetzt.

### `SYSTEM_TO_SYSTEM`

Die Capsule wird auf eine andere NovaOS-Installation übertragen.

### `OFFLINE_TRANSFER`

Die Capsule wird über ein transportierbares Medium übertragen.

### `REMOTE_RESUME`

Die TaskCapsule wird auf einem entfernten kompatiblen System fortgesetzt.

## Migration Preparation

Vor der Migration muss NovaOS einen konsistenten Zustand herstellen.

Möglicher Ablauf:

```text
Migration Request
    ↓
Reach Safe State
    ↓
Capture Task State
    ↓
Create / Update MicroCheckpoint
    ↓
Package Required Resources
    ↓
Finalize Capsule
```

Eine Capsule darf nicht als migrationsbereit markiert werden, wenn ihr Zustand inkonsistent ist.

## Zielsystemprüfung

Vor Resume muss das Zielsystem mindestens prüfen:

```text
capsule_format
capsule_integrity
identity
security_context
semantic_types
capabilities
dependencies
resources
execution_requirements
policies
```

Erst danach darf die eigentliche Wiederaufnahme beginnen.

## Capability Mapping

Die auf dem Ursprungssystem verwendeten Capabilities müssen nicht identisch auf dem Zielsystem vorhanden sein.

Beispiel:

```text
Source:
    GPU Speech Recognizer

Target:
    CPU Speech Recognizer
```

Wenn beide dieselbe benötigte semantische Capability erfüllen, darf NovaOS eine neue Zuordnung vornehmen.

```text
Original Capability
    ↓
Semantic Requirement
    ↓
Target Capability
```

## Replanning

Ist der ursprüngliche Composition Graph auf dem Zielsystem nicht mehr ausführbar, darf NovaOS ihn neu planen.

Beispiel:

```text
Original Graph:
    GPU Decoder
    ↓
    GPU Transcriber

Target:
    no compatible GPU
```

NovaOS kann daraus erzeugen:

```text
CPU Decoder
    ↓
CPU Transcriber
```

Der zugrunde liegende Intent bleibt dabei erhalten.

## Hardwareabhängiger Zustand

Rohzustände konkreter Hardware sollen nicht Grundlage einer portablen TaskCapsule sein.

Beispiele problematischer Zustände:

```text
CPU registers
GPU queue handles
DMA descriptors
physical addresses
device-specific command buffers
```

Solche Zustände müssen entweder:

```text
discard
reconstruct
remap
restore through compatible backend
```

werden.

## Nicht migrierbare Ressourcen

Einige Ressourcen können nicht direkt übertragen werden.

Beispiele:

```text
device_handle
process_id
open socket
kernel handle
hardware queue
temporary file descriptor
```

Beim Resume muss für jede solche Ressource eine Strategie existieren.

Mögliche Strategien:

```text
RECREATE
RECONNECT
REMAP
REPLACE
DROP
BLOCK
```

## Externe Verbindungen

Netzwerkverbindungen oder externe Sitzungen werden nicht automatisch als dauerhaft gültig betrachtet.

Beispiel:

```text
Source:
    authenticated cloud session

Target:
    session must be revalidated
```

Eine externe Verbindung darf neu aufgebaut werden, wenn:

- Policy dies erlaubt
- Credentials verfügbar und autorisiert sind
- externe Gegenstelle kompatibel ist

## Ressourcenmigration

Ressourcen können:

```text
embedded
transferred
referenced
remapped
retrieved
reconstructed
```

werden.

NovaOS muss erkennen, welche Ressourcen auf dem Zielsystem bereits vorhanden sind.

Bereits identische Ressourcen sollen nicht unnötig übertragen werden.

## Datenlokalität

Eine TaskCapsule darf ihren Zustand migrieren, während große Daten am ursprünglichen Ort verbleiben.

Beispiel:

```text
TaskCapsule
    ↓ migrate

Dataset
    ↓ remains remote
```

Beim Resume kann NovaOS:

```text
remote_access
lazy_transfer
partial_transfer
local_replication
```

verwenden, sofern Policies und Information-Flow-Regeln dies erlauben.

## Berechtigungen

Migration überträgt keine automatisch gültigen Zugriffsrechte.

Nach dem Transfer müssen sicherheitsrelevante Berechtigungen erneut validiert werden.

Beispiele:

```text
filesystem access
network access
camera
microphone
device access
external credentials
```

Eine fehlende Autorisierung kann Resume blockieren, ohne die Capsule selbst ungültig zu machen.

## Information Flow

Persistente Information-Flow-Labels müssen erhalten bleiben.

Beispiel:

```text
Source:
    confidential

Target:
    confidential
```

Migration darf keine implizite Declassification verursachen.

## Resume Strategy

NovaOS soll zunächst versuchen, den genauesten verfügbaren Zustand wiederherzustellen.

Bevorzugte Reihenfolge:

```text
MicroCheckpoint
    ↓
Captured Task State
    ↓
Replanned Task State
    ↓
Intent Resume
```

Falls ein feingranularer Zustand nicht kompatibel ist, darf auf eine höhere semantische Ebene zurückgefallen werden.

## Resume Point

Die TaskCapsule muss eindeutig beschreiben können, an welchem logischen Punkt fortgesetzt werden soll.

Beispiel:

```text
completed:
    audio.clean

active:
    audio.transcribe

pending:
    document.export
```

NovaOS darf bereits bestätigte nicht-idempotente Schritte nicht unbeabsichtigt wiederholen.

## Resume Results

Mindestens folgende Ergebnisse müssen unterscheidbar sein:

```text
RESUMED
REPLAN_REQUIRED
PARTIAL
AUTHORIZATION_REQUIRED
INCOMPATIBLE
BLOCKED
FAILED
```

### `RESUMED`

Die Aufgabe wurde erfolgreich fortgesetzt.

### `REPLAN_REQUIRED`

Der ursprüngliche Ausführungsplan ist nicht mehr verwendbar, aber der Intent kann neu geplant werden.

### `PARTIAL`

Nur ein Teil des ursprünglichen Zustands konnte wiederhergestellt werden.

### `AUTHORIZATION_REQUIRED`

Benötigte Rechte müssen erneut bestätigt werden.

### `INCOMPATIBLE`

Das Zielsystem kann wesentliche Anforderungen nicht erfüllen.

### `BLOCKED`

Policy oder Security verhindert die Fortsetzung.

### `FAILED`

Die Wiederherstellung konnte nicht erfolgreich abgeschlossen werden.

## Unterbrechung während Migration

Eine unterbrochene Migration darf die ursprüngliche TaskCapsule nicht unbrauchbar machen.

Beispiel:

```text
Source Capsule:
    VALID

Transfer:
    interrupted

Target Capsule:
    incomplete

Source Capsule:
    remains VALID
```

Erst nach vollständiger Integritätsprüfung darf die Zielkopie als gültig betrachtet werden.

## Migration Commit

Bei einer echten Übergabe der Aufgabe darf ein kontrollierter Commit verwendet werden.

Beispiel:

```text
Source active
    ↓
Target prepared
    ↓
Target verified
    ↓
Migration commit
    ↓
Target active
    ↓
Source inactive
```

Damit wird verhindert, dass eine Aufgabe unbeabsichtigt gleichzeitig mehrfach mit externen Seiteneffekten ausgeführt wird.

## Forking

Wenn parallele Fortsetzung ausdrücklich gewünscht ist, wird keine normale Migration, sondern ein Fork erzeugt.

```text
Original Capsule
    ├── Branch A
    └── Branch B
```

Beide Branches müssen eindeutig identifizierbar sein.

## Beispiel

Ursprungssystem:

```text
Task:
    Video transcription

Execution:
    GPU accelerated

State:
    63 % complete
```

Migration:

```text
Capture Task State
    ↓
Store MicroCheckpoint
    ↓
Package Capsule
    ↓
Transfer
```

Zielsystem:

```text
GPU:
    unavailable

CPU Transcriber:
    available
```

NovaOS:

```text
Checkpoint:
    incompatible with original GPU backend

Logical Task State:
    valid

Result:
    REPLAN_REQUIRED
```

Neue Ausführung:

```text
63 % completed state
    ↓
CPU Transcriber
    ↓
Resume
```

Der Intent wird fortgesetzt, obwohl sich die konkrete Hardware geändert hat.

## Normative Anforderungen

1. Eine TaskCapsule MUSS vor Migration in einen konsistenten Zustand gebracht werden können.
2. Das Zielsystem MUSS Integrität, Abhängigkeiten, Policies und Sicherheit vor Resume validieren.
3. Migration SOLL primär semantischen und logischen Zustand statt hardwarespezifischer Rohzustände verwenden.
4. Fehlende konkrete Capabilities MÜSSEN durch kompatible semantische Alternativen ersetzbar sein können.
5. Nicht migrierbare Ressourcen MÜSSEN explizit erkannt und behandelt werden.
6. Berechtigungen und externe Sessions MÜSSEN auf dem Zielsystem erneut validiert werden.
7. Ein inkompatibler Execution Graph MUSS Replanning ermöglichen können, sofern der Intent weiterhin erfüllbar ist.
8. Eine fehlgeschlagene oder unterbrochene Migration DARF eine zuvor gültige Quell-Capsule nicht automatisch ungültig machen.

## Abgrenzung

Diese NPSPEC definiert:

- TaskCapsule-Migration
- Zielsystemvalidierung
- Capability Mapping
- Replanning
- Behandlung nicht migrierbarer Ressourcen
- Resume
- Migration Commit

Nicht Bestandteil sind:

- Containerformat
- detailliertes Task State Capture
- Transportprotokolle
- Netzwerkprotokolle
- allgemeine Geräteerkennung
- Versionskompatibilität im Detail

## Zugehörige NPSPECs

- `NPSPEC-CAPSULE-0001 – TaskCapsule Container Format`
- `NPSPEC-CAPSULE-0002 – Task State Capture`
- `NPSPEC-CAPSULE-0003 – Capability & Dependency Manifest`
- `NPSPEC-CAPSULE-0004 – Resource Packaging & References`
- `NPSPEC-CAPSULE-0005 – Capsule Security & Identity`
- `NPSPEC-CAPSULE-0007 – Capsule Versioning & Compatibility`
- `NPSPEC-CHECKPOINT-0007 – Checkpoint Migration & Resume`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`
- `NPSPEC-SYNTHESIS-0003 – Synthesis Planning`