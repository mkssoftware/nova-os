# NPSPEC-INTENT-0006 – Intent Composition

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie komplexe `Nova.Intent`-Objekte aus mehreren Teil-Intents zusammengesetzt werden.

Ein komplexer Intent kann in kleinere, semantisch klar abgegrenzte Child-Intents zerlegt werden.

Grundprinzip:

```text
Parent Intent
    ↓
Child Intent A
Child Intent B
Child Intent C
```

## Composition Model

Ein zusammengesetzter Intent besitzt Beziehungen zu seinen Child-Intents.

Beispiel:

```text
Intent:
    podcast.publish

Children:
    audio.clean
    audio.transcribe
    chapter.detect
    media.export
```

Die Child-Intents bleiben eigenständige Intent-Objekte mit eigener:

- ID
- Lifecycle-State
- Constraints
- Policies
- Inputs
- Outputs

## Abhängigkeiten

Child-Intents können voneinander abhängig sein.

Beispiel:

```text
audio.clean
    ↓
audio.transcribe
    ↓
chapter.detect
```

Parallel ausführbare Intents dürfen unabhängig voneinander geplant werden.

```text
        ┌─ transcript.create
audio ──┤
        └─ waveform.generate
```

Damit entsteht ein gerichteter Intent Graph.

Zyklische Abhängigkeiten sind nur zulässig, wenn sie durch einen explizit definierten Iterationsmechanismus unterstützt werden.

## Datenfluss

Ergebnisse eines Child-Intents dürfen als Input anderer Child-Intents verwendet werden.

Beispiel:

```text
Intent A
    output: Media.Audio.Clean

        ↓

Intent B
    input: Media.Audio.Clean
```

Die Verbindung muss über semantische Typen validiert werden.

## Completion Policy

Ein Parent-Intent definiert, wann er als erfüllt gilt.

Unterstützte Grundmodelle:

```text
ALL_REQUIRED
ANY_REQUIRED
QUORUM
CUSTOM
```

Beispiel:

```text
completion:
    ALL_REQUIRED
```

Optionale Child-Intents dürfen gekennzeichnet werden.

```text
required: false
```

Ihr Fehlschlag muss dann nicht automatisch den Parent-Intent fehlschlagen lassen.

## Constraints und Policies

Child-Intents erben relevante Constraints und Policies des Parent-Intents.

Beispiel:

```text
Parent:
    network: denied

Child:
    network: denied
```

Ein Child-Intent darf geerbte Einschränkungen nur verschärfen, nicht eigenständig abschwächen.

Zusätzliche lokale Constraints sind zulässig.

## Dynamische Composition

Die Zusammensetzung darf während Resolution oder Replanning angepasst werden.

Beispiel:

```text
Original:

Audio
    ↓
Transcriber

Neue Planung:

Audio
    ↓
NoiseReduction
    ↓
Transcriber
```

Dabei darf sich das ursprüngliche Ziel des Parent-Intents nicht unbeabsichtigt verändern.

## Fehlerbehandlung

Fehler eines Child-Intents müssen anhand der Parent-Policy behandelt werden.

Mögliche Aktionen:

```text
retry
replace
replan
skip_optional
fail_parent
```

Beispiel:

```text
chapter.detect
    ↓ Failed
    ↓ optional
skip
```

Der Parent kann trotzdem erfolgreich abgeschlossen werden.

## Composition und Nova.Synthesis

`Intent Composition` beschreibt die logische Zerlegung einer Absicht.

`Nova.Synthesis` beschreibt die technische Zusammensetzung ausführbarer Capabilities.

```text
Intent Composition
    ↓
Was sind die Teilziele?

Nova.Synthesis
    ↓
Wie werden diese Teilziele technisch ausgeführt?
```

Beide Ebenen müssen getrennt bleiben.

## Beispiel

```text
Intent:
    podcast.publish

Children:

1. audio.clean
2. audio.transcribe
3. chapter.detect
4. media.export

Dependencies:

audio.clean
    ↓
audio.transcribe
    ↓
chapter.detect

audio.clean
    ↓
media.export
```

Der Parent gilt als abgeschlossen, sobald alle als erforderlich markierten Child-Intents erfolgreich abgeschlossen sind.

## Normative Anforderungen

1. Ein Parent-Intent MUSS seine Child-Intents eindeutig referenzieren können.
2. Child-Intents MÜSSEN eigenständige Intent-Objekte bleiben.
3. Abhängigkeiten zwischen Child-Intents MÜSSEN explizit darstellbar sein.
4. Datenflüsse zwischen Child-Intents MÜSSEN semantisch typisiert sein.
5. Ein Parent-Intent MUSS eine Completion Policy besitzen können.
6. Geerbte Constraints und Policies DÜRFEN durch Child-Intents nicht eigenständig abgeschwächt werden.
7. Änderungen an der Composition DÜRFEN das ursprüngliche Parent-Ziel nicht stillschweigend verändern.
8. Fehler einzelner Child-Intents MÜSSEN gemäß der Parent-Policy behandelt werden.

## Abgrenzung

Diese NPSPEC definiert:

- Parent-/Child-Intents
- Intent Graphs
- Abhängigkeiten
- Datenflüsse
- Completion Policies

Nicht Bestandteil sind:

- Capability-Komposition
- Execution Graphs
- konkrete Scheduling-Strategien
- Intent Resolution
- Persistenz und Resume

## Zugehörige NPSPECs

- `NPSPEC-INTENT-0001 – Intent Object Model`
- `NPSPEC-INTENT-0002 – Intent Lifecycle`
- `NPSPEC-INTENT-0004 – Intent Resolution`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`
- `NPSPEC-INTENT-0007 – Intent Persistence & Resume`
- `NPSPEC-SYNTHESIS-0002 – Capability Composition Graph`