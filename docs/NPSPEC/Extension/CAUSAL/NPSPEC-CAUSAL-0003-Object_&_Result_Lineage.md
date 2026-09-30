# NPSPEC-CAUSAL-0003 – Object & Result Lineage

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie NovaOS die Herkunft und Ableitung von Objekten und Ergebnissen nachvollzieht.

`Lineage` beantwortet insbesondere:

```text
Woher stammt dieses Objekt?
```

und:

```text
Welche Ergebnisse sind daraus entstanden?
```

## Grundprinzip

```text
Quelle
    ↓
Operation
    ↓
Zwischenergebnis
    ↓
Operation
    ↓
Ergebnis
```

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
```

## Lineage-Modell

Jedes relevante Objekt darf Beziehungen zu seinen direkten Vorgängern und Nachfolgern besitzen.

Grundlegende Beziehungen:

```text
DERIVED_FROM
PRODUCED_BY
USED_TO_PRODUCE
VERSION_OF
```

Beispiel:

```text
transcript
    DERIVED_FROM
audio.cleaned
```

Die vollständige Herkunft entsteht durch Traversierung des Causality Graph.

## Direkte und transitive Herkunft

NovaOS muss zwischen direkter und transitiver Herkunft unterscheiden.

Direkt:

```text
transcript
    ← audio.cleaned
```

Transitiv:

```text
transcript
    ← audio.cleaned
    ← audio.raw
```

Dadurch kann sowohl die unmittelbare als auch die vollständige Herkunft abgefragt werden.

## Mehrere Quellen

Ein Ergebnis darf aus mehreren Objekten entstehen.

Beispiel:

```text
Image A ─┐
         ├─ compose ─→ Image C
Image B ─┘
```

Die Lineage von `Image C` enthält beide Eingaben.

NovaOS darf die Herkunft nicht auf eine einzelne Quelle reduzieren.

## Objektversionen

Änderungen an einem Objekt sollen als neue nachvollziehbare Zustände oder Versionen darstellbar sein.

```text
Document v1
    ↓ edit
Document v2
    ↓ edit
Document v3
```

Die Beziehung zwischen Versionen kann durch:

```text
VERSION_OF
```

oder eine konkrete Transformationsbeziehung beschrieben werden.

Die ursprüngliche Herkunft bleibt erhalten.

## Result Lineage

Auch Ergebnisse von Intents und Capabilities besitzen Lineage.

Beispiel:

```text
Intent:
    document.summarize

Input:
    report.pdf

Output:
    summary.txt
```

Daraus entstehen beispielsweise:

```text
summary.txt
    PRODUCED_BY
summarization_operation

summary.txt
    DERIVED_FROM
report.pdf
```

## Unveränderte Weitergabe

Wird ein bestehendes Objekt unverändert weitergereicht, bleibt seine Identität bestehen.

```text
Intent A
    ↓
Object X
    ↓
Intent B
```

Eine reine Referenzierung erzeugt keine neue Objektversion.

Neue Lineage-Knoten entstehen erst durch tatsächliche Transformation, Ableitung oder Versionierung.

## Teilabhängigkeiten

Ein Ergebnis darf nur von einem Teil eines Objekts abhängen.

Beispiel:

```text
Video
    └── Audio Track
            ↓
       Transcript
```

Eine Teilreferenz kann beispielsweise lauten:

```text
DERIVED_FROM {
    object: video:42
    component: audio_track:1
}
```

Damit muss nicht das gesamte Quellobjekt als gleichwertige Ursache behandelt werden.

## Gelöschte Quellen

Wird ein Ursprungsobjekt gelöscht, darf dessen bestehende Lineage nicht automatisch verschwinden.

Die Referenz kann beispielsweise als:

```text
source_missing
```

markiert werden.

Die Information über die frühere Herkunft soll erhalten bleiben, sofern Retention- und Datenschutzregeln dies erlauben.

## Externe Quellen

Lineage darf externe Quellen referenzieren.

Beispiele:

```text
URL
Sensor
ExternalDevice
RemoteService
ImportedFile
UserInput
```

Externe Quellen müssen eindeutig genug beschrieben werden, um ihre Rolle später nachvollziehen zu können.

## Beispiel

```text
source:microphone.recording
    ↓
object:audio.raw
    ↓
object:audio.cleaned
    ↓
object:transcript
    ↓
object:summary
```

Abfrage:

```text
lineage(summary)
```

Ergebnis:

```text
summary
    ← transcript
    ← audio.cleaned
    ← audio.raw
    ← microphone.recording
```

## Normative Anforderungen

1. NovaOS MUSS direkte und transitive Herkunft unterscheiden können.
2. Ein Ergebnis MUSS mehrere Quellen besitzen können.
3. Objektversionen MÜSSEN ihre Herkunft beibehalten können.
4. Unveränderte Referenzierung DARF nicht automatisch neue Objektidentitäten erzeugen.
5. Teilabhängigkeiten SOLLEN ausdrückbar sein.
6. Gelöschte Quellen DÜRFEN bestehende Lineage nicht automatisch zerstören.
7. Externe Quellen MÜSSEN als Herkunft referenzierbar sein.
8. Lineage MUSS vorwärts und rückwärts traversierbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- Herkunft von Objekten
- Result Lineage
- Versionen
- Mehrfachquellen
- direkte und transitive Ableitung

Nicht Bestandteil sind:

- Event-Erfassung
- automatische Propagation
- Query-Schnittstellen
- Graph-Compaction
- Evidence-Bundles

## Zugehörige NPSPECs

- `NPSPEC-CAUSAL-0001 – Causality Graph Model`
- `NPSPEC-CAUSAL-0002 – Causal Event Records`
- `NPSPEC-CAUSAL-0004 – Causality Propagation`
- `NPSPEC-CAUSAL-0005 – Causal Query Interface`
- `NPSPEC-CAUSAL-0006 – Causal Integrity & Graph Compaction`
- `NPSPEC-EVIDENCE-0002 – Source & Provenance References`
