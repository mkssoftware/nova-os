# NPSPEC-INTENTC-0004 – Intent-to-Execution Lowering

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie ein semantisch validierter Intent in `Execution IR` überführt wird.

Ziel ist, die ursprüngliche Intent-Semantik in eine technisch planbare Ausführungsstruktur zu übersetzen.

## Grundprinzip

```text
Validated Intent
    ↓
Semantic Operations
    ↓
Execution Operations
    ↓
Execution IR
```

## Lowering

Das Lowering reduziert schrittweise die Abstraktion.

```text
Intent Goal
    ↓
Semantic Operations
    ↓
Data & Dependency Graph
    ↓
Execution IR
```

Dabei muss die ursprüngliche Bedeutung erhalten bleiben.

## Semantic Operations

Ein Intent-Ziel darf in mehrere abstrakte Operationen zerlegt werden.

Beispiel:

```text
media.video.transcribe
```

wird zu:

```text
extract_audio
transcribe_audio
produce_transcript
```

Diese Operationen sind noch nicht zwingend an konkrete Implementierungen gebunden.

## Datenfluss

Inputs, Outputs und Abhängigkeiten müssen explizit in die Ausführungsstruktur übernommen werden.

```text
Media.Video
    ↓
ExtractAudio
    ↓
Media.Audio
    ↓
Transcribe
    ↓
Document.Transcript
```

## Constraints

Relevante Intent-Anforderungen müssen erhalten bleiben.

Beispiele:

```text
local_only
deadline
deterministic
precision
resource_budget
trust
```

Sie können direkt in der IR oder über `Execution Contracts` referenziert werden.

## Keine frühe Hardwarebindung

Das Lowering soll keine unnötige Bindung an konkrete Hardware erzeugen.

Bevorzugt:

```text
operation:
    matrix.multiply
```

statt:

```text
execute_gpu_kernel_x
```

Hardware- und Backend-Auswahl erfolgt in späteren Phasen.

## Traceability

IR-Strukturen müssen auf ihren Ursprung im Intent zurückführbar sein.

```text
Intent Node
    ↓
Semantic Operation
    ↓
IR Node
```

Dadurch bleiben Debugging, Evidence und Replanning möglich.

## Ergebnis

Das Lowering kann mindestens folgende Ergebnisse liefern:

```text
SUCCESS
LOWERING_FAILED
UNRESOLVED_DEPENDENCY
UNSUPPORTED_OPERATION
```

Bei Fehlern darf keine vollständig gültige IR signalisiert werden.

## Beispiel

```text
Intent:
    media.audio.transcribe

Input:
    Media.Audio

Output:
    Document.Transcript
```

Lowering:

```text
LOAD Media.Audio
    ↓
TRANSCRIBE
    ↓
PRODUCE Document.Transcript
```

Ergebnis:

```text
Execution IR
```

## Normative Anforderungen

1. Validierte Intents MÜSSEN in `Execution IR` überführbar sein.
2. Intent-Semantik MUSS während des Lowerings erhalten bleiben.
3. Inputs, Outputs und Abhängigkeiten MÜSSEN explizit darstellbar sein.
4. Relevante Constraints MÜSSEN in die Ausführungsrepräsentation übernommen werden.
5. Konkrete Hardwarebindungen SOLLEN möglichst spät erfolgen.
6. IR-Nodes MÜSSEN auf ihre Intent-Herkunft zurückführbar sein.
7. Fehlgeschlagenes Lowering DARF keine gültige Execution IR erzeugen.

## Abgrenzung

Diese NPSPEC definiert:

- Intent-to-Execution Lowering
- semantische Operationen
- Übernahme von Datenfluss und Constraints
- Traceability

Nicht Bestandteil sind:

- Intent Normalization
- Semantic Analysis
- Execution-IR-Struktur im Detail
- Capability-Auswahl
- Hardware-Lowering

## Zugehörige NPSPECs

- `NPSPEC-INTENTC-0001 – Intent Compiler Architecture`
- `NPSPEC-INTENTC-0002 – Intent Normalization`
- `NPSPEC-INTENTC-0003 – Intent Semantic Analysis`
- `NPSPEC-EXECIR-0001 – Execution IR Object Model`
- `NPSPEC-EXECIR-0002 – Execution Graph Representation`
- `NPSPEC-EXECIR-0004 – Execution Contract Integration`
- `NPSPEC-EXECIR-0006 – Backend & Hardware Lowering`