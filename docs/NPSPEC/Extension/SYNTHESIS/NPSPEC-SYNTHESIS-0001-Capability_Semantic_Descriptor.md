# NPSPEC-SYNTHESIS-0001 – Capability Semantic Descriptor

## Status

Angenommen

## Zweck

Diese Spezifikation definiert die semantische Beschreibung einer NovaOS-Capability.

Ziel ist, dass NovaOS versteht:

- was eine Capability leisten kann
- welche Eingaben sie benötigt
- welche Ergebnisse sie erzeugt
- welche Bedingungen für ihre Nutzung gelten

Dadurch können `Nova.Intent`, `Intent Resolution` und `Nova.Synthesis` Capabilities automatisch finden und kombinieren.

## Grundprinzip

```text
Capability
    ↓
Semantic Descriptor
    ↓
NovaOS versteht Bedeutung
    ↓
Resolution / Synthesis
```

Eine Capability wird nicht nur über Name oder API identifiziert, sondern über ihre semantischen Fähigkeiten.

## Descriptor

Die logische Grundstruktur lautet:

```text
CapabilitySemanticDescriptor {
    id
    version
    provides
    accepts
    produces
    properties
    requirements
    effects
}
```

## Identität

Jede Capability muss eindeutig identifizierbar sein.

Beispiel:

```text
capability:
    nova.media.speech.transcriber

version:
    2
```

Version und Identität müssen getrennt behandelbar sein.

## `provides`

`provides` beschreibt die semantischen Aufgaben, die eine Capability erfüllen kann.

Beispiel:

```text
provides {
    media.audio.transcribe
}
```

Eine Capability darf mehrere Fähigkeiten anbieten.

```text
provides {
    media.audio.transcribe
    media.audio.language_detect
}
```

## `accepts`

`accepts` definiert die semantischen Eingabetypen.

Beispiel:

```text
accepts {
    source:
        Media.Audio
}
```

Optionale Inputs müssen gekennzeichnet werden können.

```text
language:
    Language.Code?
```

## `produces`

`produces` beschreibt die möglichen Ergebnisse.

Beispiel:

```text
produces {
    transcript:
        Document.Transcript
}
```

Mehrere Outputs sind zulässig.

```text
produces {
    transcript:
        Document.Transcript

    subtitles:
        Media.Subtitles
}
```

## Semantic Types

Inputs und Outputs müssen mit dem NovaOS Semantic-Type-System kompatibel sein.

Beispiel:

```text
Media.Audio
    ↓
Capability
    ↓
Document.Transcript
```

Damit kann NovaOS Capability-Ketten ohne anwendungsspezifische Sonderlogik zusammensetzen.

## Requirements

Eine Capability darf Anforderungen deklarieren.

Beispiele:

```text
requirements {
    network:
        optional

    memory:
        512MiB

    hardware:
        GPU?

    trust:
        verified
}
```

Requirements beschreiben Voraussetzungen und keine garantierten Ressourcen.

## Properties

Zusätzliche Eigenschaften dürfen angegeben werden.

Beispiele:

```text
properties {
    deterministic:
        true

    streaming:
        true

    checkpointable:
        true

    local_execution:
        true
}
```

Diese Informationen können bei Resolution und Synthesis berücksichtigt werden.

## Effects

Eine Capability muss relevante Seiteneffekte beschreiben können.

Beispiele:

```text
effects {
    filesystem_write
    network_access
    device_access
}
```

Eine reine Berechnungs-Capability kann beispielsweise deklarieren:

```text
effects:
    none
```

Seiteneffekte sind für:

- Policies
- Undo
- Causality
- Information Flow

relevant.

## Constraints

Eine Capability darf eigene technische Grenzen besitzen.

Beispiel:

```text
limits {
    max_input_size:
        4GiB

    supported_languages {
        de
        en
        fr
    }
}
```

Diese Grenzen müssen bei Resolution und Synthesis berücksichtigt werden.

## Trust

Der Descriptor darf eine Trust-Klassifikation enthalten.

Beispiel:

```text
trust:
    system
```

Mögliche Klassen können sein:

```text
system
verified
third_party
untrusted
```

Die endgültige Trust-Entscheidung erfolgt durch das NovaOS-Sicherheitsmodell.

## Reversibility

Capabilities mit Seiteneffekten sollen beschreiben können, ob ihre Aktionen:

```text
REVERSIBLE
COMPENSATABLE
CONDITIONAL
IRREVERSIBLE
```

sind.

Dies ermöglicht die Integration mit systemweitem Undo.

## Checkpoint Support

Eine Capability darf ihre Checkpoint-Fähigkeit deklarieren.

Beispiel:

```text
checkpoint {
    supported:
        true

    safe_points:
        between_chunks
}
```

Die genaue Checkpoint-Semantik wird durch `Nova.MicroCheckpoint` definiert.

## Composition

Der Descriptor muss ausreichend Informationen liefern, damit Nova.Synthesis Ausgänge einer Capability mit Eingängen einer anderen verbinden kann.

Beispiel:

```text
VideoAudioExtractor

accepts:
    Media.Video

produces:
    Media.Audio
```

und:

```text
SpeechRecognizer

accepts:
    Media.Audio

produces:
    Document.Transcript
```

Daraus kann entstehen:

```text
Media.Video
    ↓
VideoAudioExtractor
    ↓
Media.Audio
    ↓
SpeechRecognizer
    ↓
Document.Transcript
```

## Versionierung

Semantic Descriptors müssen versioniert sein.

Breaking Changes an:

- Inputs
- Outputs
- Semantik
- Seiteneffekten
- Requirements

müssen als inkompatible Version erkennbar sein.

## Beispiel

```text
CapabilitySemanticDescriptor {
    id:
        nova.speech.local

    version:
        2

    provides {
        media.audio.transcribe
    }

    accepts {
        source:
            Media.Audio
    }

    produces {
        transcript:
            Document.Transcript
    }

    properties {
        deterministic:
            false

        streaming:
            true

        checkpointable:
            true

        local_execution:
            true
    }

    requirements {
        memory:
            512MiB
    }

    effects {
        none
    }

    trust:
        system
}
```

## Normative Anforderungen

1. Jede synthetisierbare Capability MUSS einen maschinenlesbaren Semantic Descriptor besitzen.
2. `provides`, `accepts` und `produces` MÜSSEN semantisch typisierbar sein.
3. Relevante Requirements und Seiteneffekte MÜSSEN beschreibbar sein.
4. Semantic Descriptors MÜSSEN versionierbar sein.
5. Capability-Namen DÜRFEN nicht allein zur semantischen Zuordnung verwendet werden.
6. Descriptors MÜSSEN ausreichend Informationen für Intent Resolution und Capability Composition bereitstellen.
7. Sicherheits-, Trust- und Policy-Prüfungen DÜRFEN nicht allein durch Selbstaussagen einer Capability als erfüllt gelten.
8. Breaking Changes MÜSSEN als solche erkennbar sein.

## Abgrenzung

Diese NPSPEC definiert:

- semantische Capability-Beschreibung
- Inputs und Outputs
- Requirements
- Eigenschaften
- Seiteneffekte
- grundlegende Composition-Informationen

Nicht Bestandteil sind:

- Capability Composition Graph
- Synthesis Planning
- konkrete Auswahlalgorithmen
- Pipeline Verification
- Isolation
- Composition Cache

## Zugehörige NPSPECs

- `NPSPEC-SYNTHESIS-0002 – Capability Composition Graph`
- `NPSPEC-SYNTHESIS-0003 – Synthesis Planning`
- `NPSPEC-SYNTHESIS-0004 – Constraint-Aware Composition`
- `NPSPEC-SYNTHESIS-0005 – Synthesized Pipeline Verification`
- `NPSPEC-SYNTHESIS-0006 – Generated Composition Isolation`
- `NPSPEC-SYNTHESIS-0007 – Composition Cache & Reuse`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`
- `NPSPEC-INTENT-0004 – Intent Resolution`