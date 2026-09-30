# NPSPEC-SYNTHESIS-0003 – Synthesis Planning

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie `Nova.Synthesis` aus einem aufgelösten Intent einen konkreten Capability Composition Graph plant.

Ziel ist, aus mehreren möglichen Lösungswegen eine ausführbare, gültige und optimierbare Komposition zu erzeugen.

## Grundprinzip

```text
Resolved Intent
    ↓
Candidate Capabilities
    ↓
Synthesis Planning
    ↓
Composition Graph
```

## Planungsgrundlage

Die Planung berücksichtigt mindestens:

- Intent-Ziel
- Semantic Types
- verfügbare Capabilities
- Constraints
- Policies
- Abhängigkeiten
- benötigte Inputs
- erwartete Outputs

Optional können zusätzlich berücksichtigt werden:

- Ressourcen
- Kosten
- Energie
- Latenz
- Präzision
- Trust
- Hardware

## Plan

Ein Synthesis Plan kann logisch enthalten:

```text
SynthesisPlan {
    id
    intent
    graph
    selected_capabilities
    dependencies
    assumptions
}
```

Der Plan beschreibt einen konkreten Lösungsweg für den Intent.

## Kandidaten

Mehrere gültige Composition Graphs dürfen gleichzeitig existieren.

Beispiel:

```text
Plan A:
    Local CPU

Plan B:
    Local GPU

Plan C:
    Remote Service
```

Nicht jeder gültige Plan muss ausgeführt werden.

Die Auswahl erfolgt anhand der geltenden Anforderungen und Optimierungsziele.

## Graphaufbau

Die Planung verbindet Capabilities anhand ihrer semantischen Ein- und Ausgänge.

Beispiel:

```text
Media.Video
    ↓
ExtractAudio
    ↓
Media.Audio
    ↓
SpeechRecognition
    ↓
Document.Transcript
```

Fehlende Zwischentypen dürfen durch geeignete Transformations-Capabilities ergänzt werden.

## Suchraum

Nova.Synthesis muss den möglichen Lösungsraum begrenzen können.

Mögliche Grenzen:

```text
max_graph_depth
max_candidates
max_transformations
max_cost
max_planning_time
```

Damit wird verhindert, dass die Suche nach einer optimalen Komposition unbegrenzt wächst.

## Teilplanung

Komplexe Intents dürfen in Teilprobleme zerlegt werden.

Beispiel:

```text
Intent
    ↓
Subgoal A
Subgoal B
Subgoal C
```

Für jedes Subgoal kann zunächst ein eigener Teilgraph geplant werden.

Diese Teilgraphen werden anschließend zu einem Gesamtgraph verbunden.

## Hard Constraints

Ein Plan, der zwingende Constraints oder Policies verletzt, ist ungültig.

Beispiel:

```text
Policy:
    network: denied

Plan:
    RemoteService
```

Ergebnis:

```text
INVALID
```

## Soft Preferences

Nicht zwingende Anforderungen dürfen zur Bewertung gültiger Pläne verwendet werden.

Beispiele:

```text
prefer low_latency
prefer low_energy
prefer local
prefer deterministic
```

Diese Präferenzen dürfen keine Hard Constraints überschreiben.

## Replanning

Ein bestehender Plan darf ersetzt werden, wenn:

- eine Capability ausfällt
- Ressourcen nicht verfügbar sind
- Constraints sich ändern
- eine Abhängigkeit ungültig wird
- ein besser geeigneter Plan notwendig wird

Beispiel:

```text
GPU Plan
    ↓ GPU unavailable
Replan
    ↓
CPU Plan
```

Das ursprüngliche Intent-Ziel bleibt erhalten.

## Planstabilität

NovaOS soll unnötiges ständiges Umschalten zwischen ähnlich guten Plänen vermeiden.

Ein bestehender gültiger Plan darf bevorzugt weiterverwendet werden, wenn ein Wechsel keinen relevanten Vorteil bringt.

## Determinismus

Bei deterministischer Planung muss unter identischen Bedingungen derselbe Synthesis Plan entstehen können.

Dazu gehören insbesondere:

```text
Intent
Capabilities
Policies
Constraints
Planning Rules
```

## Planvalidierung

Vor Übergabe an die Ausführung muss der Plan mindestens geprüft werden auf:

- vollständige Inputs
- erreichbare Outputs
- gültige Typverbindungen
- erfüllte Constraints
- erfüllte Policies
- gültige Abhängigkeiten
- keine unbeabsichtigten Zyklen

Die detaillierte Pipeline Verification wird separat spezifiziert.

## Beispiel

Intent:

```text
Input:
    Media.Video

Output:
    Document.Transcript

Constraint:
    local_only
```

Verfügbare Capabilities:

```text
VideoAudioExtractor
LocalSpeechRecognizer
RemoteSpeechRecognizer
```

Planung:

```text
RemoteSpeechRecognizer
    → rejected

VideoAudioExtractor
    ↓
LocalSpeechRecognizer
```

Resultierender Graph:

```text
Media.Video
    ↓
VideoAudioExtractor
    ↓
Media.Audio
    ↓
LocalSpeechRecognizer
    ↓
Document.Transcript
```

## Normative Anforderungen

1. Synthesis Planning MUSS aus gültigen Capabilities einen ausführbaren Composition Graph erzeugen können.
2. Zwingende Constraints und Policies MÜSSEN ungültige Pläne ausschließen.
3. Mehrere gültige Plan-Kandidaten MÜSSEN unterstützt werden können.
4. Der Suchraum MUSS begrenzbar sein.
5. Komplexe Intents MÜSSEN in Teilpläne zerlegbar sein.
6. Replanning MUSS bei ungültig gewordenen Voraussetzungen möglich sein.
7. Soft Preferences DÜRFEN Hard Constraints nicht überschreiben.
8. Ein Plan MUSS vor Ausführung auf grundlegende semantische Gültigkeit geprüft werden.

## Abgrenzung

Diese NPSPEC definiert:

- Planung von Capability-Kompositionen
- Plan-Kandidaten
- Suchraumbegrenzung
- Teilplanung
- Replanning

Nicht Bestandteil sind:

- Constraint-Aware Composition im Detail
- Pipeline Verification
- Isolation
- Composition Cache
- konkrete Scheduling-Algorithmen

## Zugehörige NPSPECs

- `NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor`
- `NPSPEC-SYNTHESIS-0002 – Capability Composition Graph`
- `NPSPEC-SYNTHESIS-0004 – Constraint-Aware Composition`
- `NPSPEC-SYNTHESIS-0005 – Synthesized Pipeline Verification`
- `NPSPEC-SYNTHESIS-0006 – Generated Composition Isolation`
- `NPSPEC-SYNTHESIS-0007 – Composition Cache & Reuse`
- `NPSPEC-INTENT-0004 – Intent Resolution`
- `NPSPEC-INTENTC-0001 – Intent Compiler Architecture`