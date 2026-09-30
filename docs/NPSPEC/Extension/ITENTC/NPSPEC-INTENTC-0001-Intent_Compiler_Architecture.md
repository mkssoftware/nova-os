# NPSPEC-INTENTC-0001 – Intent Compiler Architecture

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die Architektur des `Nova Intent Compiler`.

Der Intent Compiler übersetzt einen aufgelösten Intent in eine formale Ausführungsrepräsentation, die anschließend von `Nova.Synthesis`, `Execution IR` und den Ausführungssystemen verarbeitet werden kann.

## Grundprinzip

```text
Resolved Intent
    ↓
Intent Compiler
    ↓
Execution IR
    ↓
Nova.Synthesis
    ↓
Execution
```

Der Compiler beschreibt **was ausgeführt werden soll**, ohne sich frühzeitig an konkrete Hardware oder Implementierungen zu binden.

## Compiler-Pipeline

Die grundlegende Verarbeitung besteht aus:

```text
Intent
    ↓
Normalization
    ↓
Semantic Analysis
    ↓
Constraint Resolution
    ↓
Lowering
    ↓
Execution IR
```

Die einzelnen Phasen müssen logisch voneinander getrennt bleiben.

## Eingabe

Der Intent Compiler verarbeitet einen bereits strukturierten Intent.

Dieser kann enthalten:

```text
goal
inputs
outputs
constraints
policies
semantic_types
execution_requirements
```

Unvollständige oder widersprüchliche Intents müssen vor dem Lowering erkannt werden können.

## Normalization

Unterschiedliche Darstellungen desselben Intents werden in eine kanonische Form überführt.

Beispiel:

```text
"Transkribiere diese Aufnahme"

        ↓

goal:
    media.audio.transcribe
```

Die genaue Normalisierung wird separat spezifiziert.

## Semantic Analysis

Der Compiler prüft unter anderem:

```text
semantic_types
input requirements
output requirements
constraints
policy consistency
```

Semantische Fehler müssen vor der Erzeugung einer ausführbaren IR erkannt werden.

## Lowering

Der normalisierte Intent wird schrittweise in ausführungsnähere Strukturen überführt.

```text
Intent
    ↓
Semantic Operations
    ↓
Execution Graph
    ↓
Execution IR
```

Dabei soll die ursprüngliche Intent-Semantik nachvollziehbar bleiben.

## Keine frühe Implementierungsbindung

Der Intent Compiler soll grundsätzlich keine konkrete Capability oder Hardware fest verdrahten.

Beispiel:

```text
Intent:
    matrix.compute
```

nicht:

```text
execute specific GPU kernel
```

Die konkrete Auswahl erfolgt später durch Synthesis, Compatibility und Execution Planning.

## Execution Contracts

Intent-Anforderungen müssen in passende `Nova.ExecutionContract`-Informationen überführt oder mit ihnen verknüpft werden können.

Beispiele:

```text
deadline
determinism
precision
resource_budget
trust
data_sovereignty
```

## Compiler-Ergebnis

Die Übersetzung kann mindestens folgende Ergebnisse liefern:

```text
SUCCESS
INVALID_INTENT
SEMANTIC_ERROR
CONSTRAINT_CONFLICT
LOWERING_FAILED
```

Bei einem Fehler darf keine scheinbar gültige Execution IR erzeugt werden.

## Nachvollziehbarkeit

Die Beziehung zwischen Intent und erzeugter IR muss erhalten bleiben.

```text
Intent Node
    ↓
IR Node
```

Dadurch können Debugging, Evidence, Causality und spätere Replanung den Ursprung einer Ausführungsoperation nachvollziehen.

## Normative Anforderungen

1. Der Intent Compiler MUSS strukturierte Intents in `Execution IR` überführen können.
2. Normalization, Semantic Analysis und Lowering MÜSSEN logisch getrennte Compiler-Phasen sein.
3. Semantische Fehler und Constraint-Konflikte MÜSSEN vor erfolgreichem Lowering erkannt werden.
4. Der Compiler SOLL konkrete Hardware- und Implementierungsbindungen möglichst spät vornehmen lassen.
5. Intent-Anforderungen MÜSSEN in Execution Contracts überführbar oder mit ihnen verknüpfbar sein.
6. Die Beziehung zwischen Intent-Strukturen und erzeugter IR MUSS nachvollziehbar bleiben.
7. Bei fehlgeschlagener Übersetzung DARF keine gültige Execution IR signalisiert werden.

## Abgrenzung

Diese NPSPEC definiert:

- Architektur des Intent Compilers
- Compiler-Phasen
- Eingabe und Ausgabe
- grundlegendes Lowering

Nicht Bestandteil sind:

- Normalisierung im Detail
- semantische Analyse im Detail
- Execution-IR-Struktur
- Capability-Auswahl
- Hardware-Lowering

## Zugehörige NPSPECs

- `NPSPEC-INTENTC-0002 – Intent Normalization`
- `NPSPEC-INTENTC-0003 – Intent Semantic Analysis`
- `NPSPEC-INTENTC-0004 – Intent-to-Execution Lowering`
- `NPSPEC-EXECIR-0001 – Execution IR Object Model`
- `NPSPEC-EXECIR-0002 – Execution Graph Representation`
- `NPSPEC-EXECIR-0004 – Execution Contract Integration`
- `NPSPEC-SYNTHESIS-0003 – Synthesis Planning`