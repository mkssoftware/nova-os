# NPSPEC-CAUSAL-0001 – Causality Graph Model

## Status

Angenommen

## Zweck

Diese Spezifikation definiert das grundlegende Kausalitätsmodell von `Nova.Causality`.

NovaOS soll nachvollziehen können:

- wodurch ein Objekt entstanden ist
- welche Operationen es verändert haben
- welche Eingaben verwendet wurden
- welche Ergebnisse daraus hervorgegangen sind
- wie Objekte und Operationen miteinander zusammenhängen

Grundprinzip:

```text
Quelle
    ↓
Operation
    ↓
Ergebnis
```

Komplexer:

```text
Input A ─┐
         ├─ Operation X ─→ Result C
Input B ─┘
```

## Graphmodell

Kausalität wird als gerichteter Graph dargestellt.

Der Graph besteht aus:

```text
Nodes
Edges
```

Nodes repräsentieren unter anderem:

- Datenobjekte
- Intents
- Capabilities
- Operationen
- Ergebnisse
- externe Quellen

Edges beschreiben kausale Beziehungen zwischen diesen Nodes.

## Node-Typen

Mindestens folgende logische Node-Typen müssen unterstützt werden:

```text
Object
Operation
Intent
Capability
Source
Result
```

Beispiel:

```text
Object:
    audio.raw

Operation:
    noise_reduction

Result:
    audio.cleaned
```

Die konkrete interne Speicherung darf weitere Node-Typen verwenden.

## Kausale Beziehungen

Edges besitzen eine definierte Bedeutung.

Grundlegende Beziehungen:

```text
USED
PRODUCED
DERIVED_FROM
TRIGGERED_BY
MODIFIED_BY
DEPENDS_ON
```

Beispiel:

```text
audio.raw
    USED_BY
noise_reduction

noise_reduction
    PRODUCED
audio.cleaned
```

oder vereinfacht:

```text
audio.cleaned
    DERIVED_FROM
audio.raw
```

## Identität

Jeder Graph-Node muss eindeutig referenzierbar sein.

Beispiel:

```text
causal:object:9827
causal:operation:4412
```

Die kausale Identität darf von der eigentlichen Objekt-ID getrennt sein, muss aber eindeutig auf das referenzierte Systemobjekt verweisen können.

## Operationen

Eine Operation stellt eine konkrete kausale Veränderung dar.

Beispiel:

```text
Operation {
    id
    type
    intent
    capability
    inputs
    outputs
    timestamp
}
```

Beispiel:

```text
Operation {
    type: media.audio.noise_reduction

    input:
        object:audio.raw

    output:
        object:audio.cleaned
}
```

Eine Operation ist von einer Capability zu unterscheiden.

Die Capability beschreibt, **was ausgeführt werden kann**.

Die Operation beschreibt, **was tatsächlich ausgeführt wurde**.

## Mehrere Ursachen

Ein Ergebnis darf mehrere Ursachen besitzen.

Beispiel:

```text
Image A ─┐
         ├─ compose ─→ Image C
Image B ─┘
```

NovaOS darf die Herkunft eines Ergebnisses deshalb nicht auf genau eine Quelle reduzieren.

## Kausalkette

Mehrere Operationen bilden eine Kausalkette.

Beispiel:

```text
audio.raw
    ↓
noise_reduction
    ↓
audio.cleaned
    ↓
speech_recognition
    ↓
transcript
    ↓
summary
```

NovaOS muss die Kette rückwärts und vorwärts traversieren können.

## Zeitliche Reihenfolge

Kausalität und Zeit sind miteinander verbunden, aber nicht identisch.

```text
A geschieht vor B
```

bedeutet nicht automatisch:

```text
A verursacht B
```

Eine kausale Beziehung muss explizit durch eine Operation oder bestätigte Abhängigkeit entstehen.

Zeitstempel dienen nur als zusätzliche Information.

## Intent-Bezug

Operationen sollen auf den Intent verweisen können, der sie ausgelöst hat.

Beispiel:

```text
Intent:
    media.audio.transcribe

        ↓ triggered

Operation:
    speech_recognition
```

Dadurch kann NovaOS nicht nur beantworten:

```text
Was hat diese Datei verändert?
```

sondern auch:

```text
Warum wurde diese Operation überhaupt ausgeführt?
```

## Capability-Bezug

Eine Operation kann auf die ausführende Capability verweisen.

Beispiel:

```text
Operation:
    speech_recognition

Capability:
    nova.speech.local
```

Damit bleibt nachvollziehbar, welche technische Fähigkeit ein Ergebnis erzeugt hat.

## Unveränderlichkeit

Bereits bestätigte historische Kausalitätsdaten dürfen nicht stillschweigend verändert werden.

Korrekturen müssen als neue Information hinzugefügt oder explizit versioniert werden.

Damit bleibt die ursprüngliche Historie nachvollziehbar.

## Graphbegrenzung

Der Kausalitätsgraph kann langfristig sehr groß werden.

NovaOS darf deshalb:

- alte Detailinformationen komprimieren
- Teilgraphen archivieren
- redundante technische Zwischenschritte zusammenfassen

Die wesentliche Provenienz eines relevanten Ergebnisses muss dabei erhalten bleiben.

Die konkrete Compaction wird separat spezifiziert.

## Beispiel

```text
object:audio.raw
        │
        ▼
operation:noise_reduction
        │
        ▼
object:audio.cleaned
        │
        ▼
operation:speech_recognition
        │
        ▼
object:transcript
```

Mit Intent-Bezug:

```text
intent:podcast.transcribe
        │
        ▼
operation:speech_recognition
        │
        ├── used ─────→ object:audio.cleaned
        │
        └── produced ─→ object:transcript
```

## Normative Anforderungen

1. Nova.Causality MUSS kausale Beziehungen als gerichteten Graph darstellen können.
2. Datenobjekte und Operationen MÜSSEN eindeutig referenzierbar sein.
3. Ein Ergebnis MUSS mehrere Ursachen besitzen können.
4. Operationen MÜSSEN von Capabilities unterscheidbar sein.
5. Kausalität DARF nicht allein aus zeitlicher Reihenfolge abgeleitet werden.
6. Kausalketten MÜSSEN vorwärts und rückwärts traversierbar sein.
7. Bestätigte historische Kausalitätsinformationen DÜRFEN nicht stillschweigend überschrieben werden.
8. Der Graph MUSS kompaktierbar sein, ohne wesentliche Provenienz zu verlieren.

## Abgrenzung

Diese NPSPEC definiert:

- das grundlegende Kausalitätsgraph-Modell
- Nodes
- Edges
- Operationen
- kausale Beziehungen

Nicht Bestandteil sind:

- genaue Event-Records
- detaillierte Lineage-Regeln
- Propagation
- Query-Sprache
- Graph-Compaction
- Evidence-Bundles

## Zugehörige NPSPECs

- `NPSPEC-CAUSAL-0002 – Causal Event Records`
- `NPSPEC-CAUSAL-0003 – Object & Result Lineage`
- `NPSPEC-CAUSAL-0004 – Causality Propagation`
- `NPSPEC-CAUSAL-0005 – Causal Query Interface`
- `NPSPEC-CAUSAL-0006 – Causal Integrity & Graph Compaction`
- `NPSPEC-EVIDENCE-0001 – Evidence Bundle`