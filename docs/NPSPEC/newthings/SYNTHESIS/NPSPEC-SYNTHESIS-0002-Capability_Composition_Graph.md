# NPSPEC-SYNTHESIS-0002 – Capability Composition Graph

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie mehrere Capabilities zu einem ausführbaren semantischen Graph verbunden werden.

Ziel ist, komplexe Aufgaben aus kleinen, kompatiblen Fähigkeiten zusammenzusetzen.

## Grundprinzip

```text
Input
    ↓
Capability A
    ↓
Intermediate Result
    ↓
Capability B
    ↓
Output
```

Ein Composition Graph beschreibt damit den technischen Lösungsweg für einen Intent.

## Graphmodell

Der Graph besteht aus:

```text
Nodes
Edges
```

Nodes repräsentieren Capabilities oder definierte Transformationsschritte.

Edges beschreiben Daten- oder Abhängigkeitsbeziehungen.

## Nodes

Ein Node referenziert mindestens:

```text
capability
inputs
outputs
requirements
```

Beispiel:

```text
Node A:
    VideoAudioExtractor

Node B:
    SpeechRecognizer
```

## Edges

Edges verbinden kompatible Outputs mit Inputs.

Beispiel:

```text
VideoAudioExtractor
    produces:
        Media.Audio

SpeechRecognizer
    accepts:
        Media.Audio
```

Daraus entsteht:

```text
VideoAudioExtractor
    ↓ Media.Audio
SpeechRecognizer
```

## Semantische Kompatibilität

Eine Verbindung ist nur zulässig, wenn die beteiligten Semantic Types:

```text
EXACT
COMPATIBLE
CONVERTIBLE
```

sind.

Bei `CONVERTIBLE` darf ein zusätzlicher Transformations-Node eingefügt werden.

## Verzweigungen

Ein Output darf an mehrere Capabilities weitergegeben werden.

Beispiel:

```text
Media.Audio
    ├── SpeechRecognition
    └── WaveformAnalysis
```

Damit können unabhängige Verarbeitungsschritte parallel erfolgen.

## Zusammenführungen

Mehrere Inputs dürfen in einer Capability zusammengeführt werden.

Beispiel:

```text
Transcript ─┐
            ├─ ChapterGenerator
Audio ──────┘
```

Alle erforderlichen Inputs müssen verfügbar sein, bevor der Node ausgeführt werden kann.

## Abhängigkeiten

Nicht jede Abhängigkeit ist ein Datenfluss.

Zusätzliche Beziehungen dürfen beispielsweise sein:

```text
DEPENDS_ON
MUST_RUN_BEFORE
REQUIRES_RESULT
```

Der Graph muss solche Abhängigkeiten explizit darstellen können.

## Zyklen

Composition Graphs sollen grundsätzlich azyklisch sein.

Iterative oder rückgekoppelte Verarbeitung ist nur zulässig, wenn sie explizit als kontrollierte Schleife modelliert wird.

Beispiel:

```text
IterationNode {
    condition
    max_iterations
}
```

Unbeabsichtigte Zyklen müssen abgelehnt werden.

## Constraints und Policies

Jeder Node muss mit den geltenden:

- Intent Constraints
- Policies
- Execution Contracts

vereinbar sein.

Ein semantisch gültiger Graph ist nicht automatisch zulässig.

## Seiteneffekte

Capabilities mit Seiteneffekten müssen im Graph erkennbar bleiben.

Beispiel:

```text
Pure Compute
    ↓
File Write
    ↓
Remote Publish
```

Dadurch können:

- Undo
- Security
- Causality
- Information Flow

die Auswirkungen des Graphen nachvollziehen.

## Parallelität

Unabhängige Nodes dürfen parallel ausgeführt werden.

Beispiel:

```text
        ┌── Node B
Node A ─┤
        └── Node C
```

Die konkrete Scheduling-Entscheidung gehört nicht zum Composition Graph.

## Ein- und Ausgänge

Der Graph muss seine externen Inputs und finalen Outputs eindeutig definieren.

Beispiel:

```text
Graph Input:
    Media.Video

Graph Output:
    Document.Transcript
```

Interne Zwischenergebnisse dürfen verborgen bleiben, solange ihre Semantik erhalten bleibt.

## Graphidentität

Ein erzeugter Composition Graph muss eindeutig referenzierbar sein.

Beispiel:

```text
composition:01K...
```

Änderungen am Graph müssen versioniert oder als neuer Graph dargestellt werden.

## Beispiel

Intent:

```text
Input:
    Media.Video

Output:
    Document.Transcript
```

Composition Graph:

```text
Media.Video
    ↓
VideoAudioExtractor
    ↓
Media.Audio
    ↓
NoiseReduction
    ↓
Media.Audio.Clean
    ↓
SpeechRecognizer
    ↓
Document.Transcript
```

## Normative Anforderungen

1. Capability Composition MUSS als gerichteter Graph darstellbar sein.
2. Nodes MÜSSEN eindeutig referenzierbare Capabilities oder Transformationsschritte darstellen.
3. Edges MÜSSEN semantisch kompatible Daten- oder Abhängigkeitsbeziehungen beschreiben.
4. Verzweigungen und Zusammenführungen MÜSSEN unterstützt werden.
5. Unbeabsichtigte Zyklen MÜSSEN erkannt und abgelehnt werden.
6. Constraints und Policies MÜSSEN auf Graph- und Node-Ebene prüfbar sein.
7. Seiteneffekte MÜSSEN im Graph erkennbar bleiben.
8. Composition Graphs MÜSSEN versionierbar und eindeutig referenzierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Capability Composition Graphs
- Nodes und Edges
- semantische Verbindungen
- Verzweigungen und Zusammenführungen
- grundlegende Graphgültigkeit

Nicht Bestandteil sind:

- Synthesis Planning
- Optimierung und Auswahl
- Constraint-Aware Composition im Detail
- Pipeline Verification
- Isolation
- Scheduling

## Zugehörige NPSPECs

- `NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor`
- `NPSPEC-SYNTHESIS-0003 – Synthesis Planning`
- `NPSPEC-SYNTHESIS-0004 – Constraint-Aware Composition`
- `NPSPEC-SYNTHESIS-0005 – Synthesized Pipeline Verification`
- `NPSPEC-SYNTHESIS-0006 – Generated Composition Isolation`
- `NPSPEC-SYNTHESIS-0007 – Composition Cache & Reuse`
- `NPSPEC-INTENT-0004 – Intent Resolution`
- `NPSPEC-INTENT-0006 – Intent Composition`