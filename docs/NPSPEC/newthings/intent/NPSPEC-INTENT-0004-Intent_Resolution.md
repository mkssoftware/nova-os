# NPSPEC-INTENT-0004 – Intent Resolution

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Auflösung eines `Nova.Intent` in grundsätzlich ausführbare Fähigkeiten und mögliche Ausführungspfade.

`Intent Resolution` beantwortet die Frage:

```text
Welche Capabilities können dieses Ziel unter den gegebenen Bedingungen erfüllen?
```

Die Auflösung bestimmt noch nicht zwingend den finalen Execution Graph.

Sie ermittelt zunächst gültige Kandidaten und Lösungswege.

## Grundprinzip

```text
Intent
    ↓
Schema Validation
    ↓
Semantic Analysis
    ↓
Intent Resolution
    ↓
Candidate Capabilities
    ↓
Candidate Execution Paths
    ↓
Planning / Intent Compiler
```

Die Auflösung basiert auf:

- Intent-Typ
- Semantic Types
- Inputs
- gewünschtem Output
- Constraints
- Policies
- verfügbaren Capabilities
- Kompatibilität
- Systemzustand

## Resolution Input

Die Resolution erhält mindestens:

```text
ResolutionRequest {
    intent
    available_capabilities
    system_context
}
```

Logisch relevante Informationen sind:

```text
intent.type
intent.inputs
intent.expected_output
intent.constraints
intent.policies
intent.context
```

Zusätzlich können berücksichtigt werden:

- Hardware
- verfügbare Ressourcen
- installierte Capabilities
- Trust-Level
- Netzwerkstatus
- Energiezustand
- Task-Kontext
- frühere Ausführungsergebnisse

## Resolution Result

Das Ergebnis der Resolution besteht aus einem oder mehreren Kandidaten.

Beispiel:

```text
ResolutionResult {
    intent_id
    candidates[]
}
```

Ein Kandidat kann enthalten:

```text
ResolutionCandidate {
    capability
    input_mapping
    output_mapping
    required_transformations
    compatibility
    estimated_requirements
}
```

Die konkrete interne Repräsentation ist implementierungsabhängig.

## Direkte Auflösung

Der einfachste Fall ist eine Capability, die den Intent direkt erfüllen kann.

Beispiel:

```text
Intent:
    media.audio.transcribe

Capability:
    nova.speech.transcriber

Input:
    Media.Audio

Output:
    Document.Transcript
```

Ergebnis:

```text
DIRECT_MATCH
```

## Capability Matching

Eine Capability muss maschinenlesbar beschreiben, welche semantischen Aufgaben sie erfüllen kann.

Beispiel:

```text
Capability {
    provides:
        media.audio.transcribe

    accepts:
        Media.Audio

    produces:
        Document.Transcript
}
```

Die Resolution vergleicht diese Beschreibung mit dem Intent.

## Matching-Stufen

Mindestens folgende Matching-Stufen müssen unterscheidbar sein:

```text
EXACT
COMPATIBLE
CONVERTIBLE
COMPOSABLE
INCOMPATIBLE
```

### `EXACT`

Intent und Capability stimmen vollständig überein.

```text
Intent Input:
    Media.Audio

Capability Input:
    Media.Audio
```

### `COMPATIBLE`

Die Capability akzeptiert einen kompatiblen semantischen Typ.

```text
Intent Input:
    Media.Audio.Stream

Capability Input:
    Media.Audio
```

### `CONVERTIBLE`

Eine sichere Transformation kann den Input in einen akzeptierten Typ überführen.

```text
Input:
    Media.Audio.FLAC

Capability requires:
    Media.Audio.PCM
```

Möglicher Pfad:

```text
FLAC
    ↓ decode
PCM
    ↓ transcribe
Transcript
```

### `COMPOSABLE`

Mehrere Capabilities müssen kombiniert werden.

Beispiel:

```text
Intent:
    Video transkribieren
```

Möglicher Pfad:

```text
Media.Video
    ↓ ExtractAudio
Media.Audio
    ↓ SpeechRecognition
Document.Transcript
```

### `INCOMPATIBLE`

Es existiert kein zulässiger oder bekannter Pfad.

## Semantic Resolution

Resolution darf nicht ausschließlich auf Namen basieren.

Beispiel:

```text
"convert"
```

ist semantisch nicht ausreichend.

Stattdessen müssen relevante Typen und Zielbeziehungen ausgewertet werden.

Beispiel:

```text
source:
    Document.Markdown

target:
    Document.PDF
```

Damit kann NovaOS nach geeigneten Transformationspfaden suchen.

## Input Mapping

Eine Capability kann andere Parameternamen als ein Intent-Schema verwenden.

Die Resolution muss semantische Zuordnung ermöglichen.

Beispiel:

```text
Intent:
    source: Media.Audio

Capability:
    audio_input: Media.Audio
```

Resolution:

```text
source
    → audio_input
```

Mapping darf nur erfolgen, wenn die semantische Kompatibilität gegeben ist.

## Output Mapping

Analog müssen Capability-Ergebnisse auf Intent-Ergebnisse abbildbar sein.

Beispiel:

```text
Capability output:
    transcript

Intent expected_output:
    result
```

Mapping:

```text
transcript
    → result
```

## Transformation Paths

Falls ein direkter Match nicht möglich ist, darf NovaOS Transformationspfade suchen.

Beispiel:

```text
Image.Raw
    ↓ Decode
Image.Bitmap
    ↓ Resize
Image.Bitmap
    ↓ Encode
Image.JPEG
```

Der Pfad muss semantisch gültig sein.

Transformationen können Eigenschaften besitzen wie:

```text
lossless
lossy
deterministic
trusted
local
remote
```

Diese Eigenschaften müssen mit Constraints und Policies abgeglichen werden.

## Mehrstufige Resolution

Ein Intent darf in mehrere Teilprobleme aufgelöst werden.

Beispiel:

```text
Intent:
    "Podcast veröffentlichen"
```

Mögliche Resolution:

```text
Audio reinigen
    ↓
Transkript erzeugen
    ↓
Kapitel erkennen
    ↓
Metadaten erzeugen
    ↓
Export vorbereiten
```

Diese Teilprobleme können später als Child-Intents oder Execution-Graph-Nodes dargestellt werden.

Die eigentliche Intent-Komposition wird separat spezifiziert.

## Candidate Set

Intent Resolution soll nicht zwingend sofort nur eine Lösung auswählen.

Beispiel:

```text
Candidate A:
    Local CPU Transcriber

Candidate B:
    Local NPU Transcriber

Candidate C:
    Remote Speech Service
```

Alle drei können semantisch denselben Intent erfüllen.

Die spätere Auswahl kann durch:

- Execution Contracts
- Ressourcenlage
- Policies
- Kosten
- Energie
- Latenz
- Genauigkeit
- Trust

erfolgen.

## Hard Constraints

Zwingende Constraints müssen bereits während der Resolution berücksichtigt werden.

Beispiel:

```text
local_only: true
```

Dann darf folgende Capability nicht als gültiger Kandidat gelten:

```text
RemoteSpeechService
```

Ebenso:

```text
network: denied
```

schließt Kandidaten aus, die zwingend Netzwerkzugriff benötigen.

## Soft Preferences

Nicht zwingende Präferenzen dürfen Kandidaten beeinflussen, aber nicht zwingend ausschließen.

Beispiel:

```text
prefer:
    low_energy
```

Mögliche Kandidaten:

```text
CPU
GPU
NPU
```

Die Resolution darf alle drei zurückgeben und die Präferenz für die spätere Planung markieren.

## Policy Filtering

Nach semantischem Matching müssen Kandidaten gegen Policies geprüft werden.

Beispiel:

```text
Capability:
    remote.ai.service

Intent Policy:
    remote_execution: denied
```

Ergebnis:

```text
REJECTED_BY_POLICY
```

Ein semantisch passender Kandidat ist nicht automatisch zulässig.

## Permission Filtering

Capability-Nutzung kann zusätzliche Berechtigungen voraussetzen.

Beispiel:

```text
requires:
    microphone.read
```

Wenn diese Berechtigung nicht vorhanden ist:

```text
UNAVAILABLE
```

oder:

```text
REQUIRES_AUTHORIZATION
```

Die Resolution darf nicht eigenständig neue Rechte vergeben.

## Capability Availability

Eine registrierte Capability ist nicht automatisch aktuell verfügbar.

Status kann sein:

```text
available
busy
offline
degraded
unavailable
```

Resolution muss zwischen:

```text
semantically_suitable
```

und:

```text
currently_available
```

unterscheiden können.

Damit kann ein Kandidat für Replanning erhalten bleiben, obwohl er momentan nicht nutzbar ist.

## Dynamic Environment

Resolution darf vom aktuellen Systemzustand abhängen.

Beispiel:

```text
System A:
    NPU verfügbar

System B:
    keine NPU
```

Der gleiche Intent kann unterschiedliche Candidate Sets erzeugen.

Das Intent-Ziel selbst bleibt dabei unverändert.

## Re-Resolution

Ein Intent darf erneut aufgelöst werden, wenn relevante Bedingungen sich ändern.

Beispiele:

- Capability fällt aus
- Gerät wird entfernt
- Netzwerk wird verfügbar
- Policy ändert sich
- Ressource wird freigegeben
- besser geeignete Capability erscheint

Beispiel:

```text
Candidate A
    ↓ failure

Re-Resolution
    ↓

Candidate B
```

Re-Resolution darf keine zwingenden Eigenschaften des ursprünglichen Intents stillschweigend verändern.

## Resolution Cache

Resolution-Ergebnisse dürfen zwischengespeichert werden.

Ein Cache-Eintrag muss invalidierbar sein, wenn sich relevante Bedingungen ändern.

Mögliche Invalidierungsgründe:

```text
capability_changed
schema_changed
policy_changed
resource_changed
compatibility_changed
```

Ein gecachter Resolution-Pfad darf nicht verwendet werden, wenn seine Voraussetzungen nicht mehr gültig sind.

## Ambiguous Resolution

Mehrere gleichwertige Interpretationen können möglich sein.

Beispiel:

```text
Intent:
    "Öffne Bericht"
```

Mögliche Bedeutung:

```text
anzeigen
bearbeiten
analysieren
```

Wenn die semantische Absicht nicht eindeutig genug ist, muss NovaOS:

```text
request_clarification
```

oder einen sicheren Standardpfad verwenden, sofern das Intent-Schema dies erlaubt.

NovaOS darf eine sicherheitsrelevante Mehrdeutigkeit nicht stillschweigend auflösen.

## Resolution Failure

Resolution kann fehlschlagen.

Mögliche Gründe:

```text
no_capability
no_valid_path
type_mismatch
policy_conflict
constraint_conflict
permission_missing
dependency_missing
ambiguous_intent
unsupported_schema
```

Beispiel:

```text
ResolutionFailure {
    intent_id: intent:01JQX7M8A4
    reason: no_valid_path
}
```

Ein Resolution Failure ist von einem Execution Failure zu unterscheiden.

Die eigentliche Ausführung hat zu diesem Zeitpunkt noch nicht begonnen.

## Resolution Explainability

NovaOS muss auf Anfrage erklären können, warum ein Kandidat:

- akzeptiert
- bevorzugt
- verworfen
- nicht verfügbar

wurde.

Beispiel:

```text
Candidate:
    RemoteTranscriber

Rejected because:
    policy.remote_execution = denied
```

oder:

```text
Candidate:
    LocalNPUTranscriber

Accepted because:
    semantic_match = exact
    local_execution = true
    required_hardware = available
```

Diese Informationen können von `Nova.Evidence` verwendet werden.

## Deterministische Resolution

Wenn ein Intent deterministische Planung verlangt, muss Resolution reproduzierbar arbeiten können.

Unter identischen:

```text
Intent
Schemas
Capabilities
Policies
Systembedingungen
```

soll dieselbe Kandidatenmenge entstehen.

Nicht-deterministische Optimierungsinformationen müssen davon getrennt behandelt werden.

## Trust

Capabilities dürfen Trust-Informationen besitzen.

Beispiel:

```text
trust:
    system
    verified
    third_party
    untrusted
```

Ein Intent kann Mindestanforderungen definieren:

```text
required_trust: verified
```

Unzureichend vertrauenswürdige Kandidaten müssen verworfen werden.

## Resolution und Compatibility Graph

`Nova.CompatibilityGraph` kann bei der Suche nach gültigen Pfaden verwendet werden.

Beispiel:

```text
API v2
    ↓ Adapter
API v4
    ↓ Adapter
API v6
```

Intent Resolution darf solche Pfade verwenden, sofern:

- Kompatibilität bestätigt ist
- Policies erfüllt sind
- Trust ausreichend ist
- Kosten akzeptabel sind

## Resolution und Nova.Synthesis

Intent Resolution beantwortet primär:

```text
Welche Fähigkeiten oder Pfade können den Intent erfüllen?
```

`Nova.Synthesis` kann anschließend aus mehreren Capabilities einen konkreten ausführbaren Capability Graph erzeugen.

Grenze:

```text
Intent Resolution
    ↓
mögliche Lösungspfade

Nova.Synthesis
    ↓
konkrete zusammengesetzte Lösung
```

## Resolution und Intent Compiler

Der Intent Compiler übernimmt die gültige semantische Auflösung und überführt sie in eine ausführbare Zwischenrepräsentation.

```text
Intent
    ↓
Resolution
    ↓
Resolved Intent
    ↓
Intent Compiler
    ↓
Execution IR
```

Resolution ist damit eine semantische Vorstufe der eigentlichen Compilation.

## Beispiel: Direkter Match

Intent:

```text
Intent {
    type: media.audio.transcribe

    input:
        Media.Audio

    output:
        Document.Transcript
}
```

Capability:

```text
Capability {
    id: nova.speech.local

    provides:
        media.audio.transcribe

    accepts:
        Media.Audio

    produces:
        Document.Transcript
}
```

Resolution:

```text
match:
    EXACT

candidate:
    nova.speech.local
```

## Beispiel: Zusammengesetzter Pfad

Intent:

```text
Input:
    Media.Video

Output:
    Document.Transcript
```

Verfügbare Capabilities:

```text
VideoAudioExtractor:
    Media.Video
        → Media.Audio

SpeechRecognizer:
    Media.Audio
        → Document.Transcript
```

Resolution:

```text
Media.Video
    ↓ VideoAudioExtractor
Media.Audio
    ↓ SpeechRecognizer
Document.Transcript
```

Ergebnis:

```text
COMPOSABLE
```

## Beispiel: Policy Conflict

Intent:

```text
type:
    media.audio.transcribe

policies:
    network: denied
```

Kandidaten:

```text
LocalTranscriber
RemoteTranscriber
```

Resolution:

```text
LocalTranscriber:
    ACCEPTED

RemoteTranscriber:
    REJECTED_BY_POLICY
```

## Beispiel: Re-Resolution

```text
Intent
    ↓
NPU Transcriber
    ↓
NPU unavailable
    ↓
Re-Resolution
    ↓
CPU Transcriber
    ↓
Execution continues
```

Der Nutzerintent bleibt identisch.

Nur der technische Erfüllungspfad ändert sich.

## Normative Anforderungen

1. Intent Resolution MUSS auf semantischer Bedeutung und nicht ausschließlich auf Namen basieren.
2. Nur schema- und typkompatible Capabilities DÜRFEN als gültige Kandidaten gelten.
3. Zwingende Constraints und Policies MÜSSEN während der Resolution berücksichtigt werden.
4. Eine semantisch geeignete Capability DARF nicht automatisch als autorisiert gelten.
5. Resolution MUSS direkte, kompatible, konvertierbare und zusammensetzbare Pfade unterscheiden können.
6. Mehrere gültige Kandidaten MÜSSEN repräsentierbar sein.
7. Resolution Failure MUSS von Execution Failure unterscheidbar sein.
8. Re-Resolution DARF das ursprüngliche Intent-Ziel oder zwingende Policies nicht stillschweigend verändern.
9. Gecachte Resolution-Ergebnisse MÜSSEN bei ungültig gewordenen Voraussetzungen invalidiert werden können.
10. Sicherheitsrelevante Mehrdeutigkeiten DÜRFEN nicht stillschweigend aufgelöst werden.
11. NovaOS MUSS begründen können, warum ein Resolution-Kandidat akzeptiert oder verworfen wurde.
12. Resolution MUSS mit versionierten Intent-Schemas und Semantic Types arbeiten können.

## Abgrenzung

Diese NPSPEC definiert:

- semantische Intent-Auflösung
- Capability Matching
- Candidate Sets
- Transformation Paths
- Policy- und Constraint-Filtering
- Re-Resolution
- Resolution Failure

Nicht Bestandteil sind:

- endgültige Auswahlstrategie
- Scheduling
- konkrete Kostenmodelle
- Capability-Komposition im Detail
- Intent Compilation
- Execution IR
- Hardware Lowering
- konkrete Berechtigungsverwaltung

Diese Bereiche werden separat spezifiziert.

## Zugehörige NPSPECs

- `NPSPEC-INTENT-0001 – Intent Object Model`
- `NPSPEC-INTENT-0002 – Intent Lifecycle`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`
- `NPSPEC-INTENT-0006 – Intent Composition`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`
- `NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor`
- `NPSPEC-COMPAT-0001 – Compatibility Descriptor`
- `NPSPEC-INTENTC-0001 – Intent Compiler Architecture`
- `NPSPEC-EXECIR-0001 – Execution IR Object Model`