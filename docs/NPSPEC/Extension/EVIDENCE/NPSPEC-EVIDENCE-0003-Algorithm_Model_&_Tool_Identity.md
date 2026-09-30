# NPSPEC-EVIDENCE-0003 – Algorithm, Model & Tool Identity

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie Algorithmen, Modelle und Werkzeuge identifiziert werden, die an der Erzeugung eines Ergebnisses beteiligt waren.

Ziel ist, später eindeutig nachvollziehen zu können, womit ein Ergebnis erzeugt wurde.

## Grundprinzip

```text
Input
    ↓
Algorithm / Model / Tool
    ↓
Result
    +
Evidence Identity
```

## Identity Descriptor

Eine verwendete Komponente kann logisch beschrieben werden als:

```text
ExecutionIdentity {
    type
    id
    version
    build
    provider
}
```

Nicht jedes Feld muss für jede Komponente vorhanden sein.

## Identitätstypen

Mindestens folgende Typen müssen unterscheidbar sein:

```text
ALGORITHM
MODEL
TOOL
CAPABILITY
```

Beispiele:

```text
algorithm:
    fft

model:
    speech-model-v4

tool:
    nova.image.converter

capability:
    media.audio.transcribe
```

## Versionierung

Die tatsächlich verwendete Version muss referenzierbar sein.

Beispiel:

```text
tool:
    nova.statistics.engine

version:
    3.2
```

Bei Builds mit relevantem Unterschied darf zusätzlich eine Build-Identität gespeichert werden.

## Modelle

Für KI- oder statistische Modelle sollen mindestens identifizierbar sein:

```text
model_id
version
provider
variant
```

Falls relevant können zusätzlich Parameter- oder Weight-Versionen referenziert werden.

## Algorithmen

Bei austauschbaren Algorithmen muss der tatsächlich verwendete Algorithmus nachvollziehbar sein.

Beispiel:

```text
operation:
    matrix_multiply

algorithm:
    blocked_gemm
```

Dies ist besonders relevant, wenn unterschiedliche Algorithmen Genauigkeit, Determinismus oder Performance beeinflussen.

## Tool Chains

Ein Ergebnis darf mehrere beteiligte Komponenten referenzieren.

```text
Decoder
    ↓
Speech Model
    ↓
Postprocessor
    ↓
Result
```

Alle für die Ergebnisinterpretation relevanten Komponenten sollen im Evidence Bundle referenzierbar sein.

## Implementierung und Semantik

Semantische Capability und konkrete Implementierung müssen unterscheidbar bleiben.

```text
Capability:
    media.audio.transcribe

Implementation:
    nova.speech.engine@4
```

Dadurch bleibt nachvollziehbar, welche Funktion verlangt und welche konkrete Umsetzung verwendet wurde.

## Beispiel

```text
execution {
    capability:
        media.audio.transcribe

    tool:
        nova.speech.engine@4

    model:
        nova.speech.model@7

    algorithm:
        beam_search
}
```

## Normative Anforderungen

1. Evidence MUSS beteiligte Algorithmen, Modelle und Werkzeuge identifizieren können.
2. Die tatsächlich verwendete Version MUSS referenzierbar sein.
3. Semantische Capability und konkrete Implementierung MÜSSEN unterscheidbar sein.
4. Mehrere beteiligte Komponenten MÜSSEN gemeinsam referenzierbar sein.
5. Relevante Modellvarianten oder Builds SOLLEN eindeutig identifizierbar sein.
6. Kurzlebige Prozess- oder Instanzkennungen DÜRFEN nicht als alleinige Identität verwendet werden.
7. Identitätsinformationen MÜSSEN mit dem zugehörigen Evidence Bundle verknüpfbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Algorithmusidentität
- Modellidentität
- Tool- und Capability-Identität
- Versionen und Implementierungen

Nicht Bestandteil sind:

- Source Provenance
- kryptografische Integrität
- Modellbewertung
- Evidence-Abfragen

## Zugehörige NPSPECs

- `NPSPEC-EVIDENCE-0001 – Evidence Bundle`
- `NPSPEC-EVIDENCE-0002 – Source & Provenance References`
- `NPSPEC-EVIDENCE-0004 – Evidence Integrity & Signing`
- `NPSPEC-EVIDENCE-0005 – Evidence Query & Explanation`
- `NPSPEC-EVIDENCE-0006 – Evidence Retention & Privacy`