# NPSPEC-INTENTC-0002 – Intent Normalization

## Status

Angenommen

## Zweck

Diese Spezifikation definiert, wie unterschiedlich formulierte oder strukturierte Intents in eine einheitliche interne Form überführt werden.

Ziel ist, semantisch gleiche Absichten unabhängig von ihrer ursprünglichen Darstellung gleich weiterverarbeiten zu können.

## Grundprinzip

```text
Raw Intent
    ↓
Normalization
    ↓
Canonical Intent
```

## Normalisierung

Die Normalisierung vereinheitlicht insbesondere:

```text
goal
inputs
outputs
semantic_types
constraints
policies
parameters
```

Dabei darf die ursprüngliche Bedeutung des Intents nicht verändert werden.

## Kanonische Form

Semantisch gleiche Intents sollen möglichst dieselbe interne Darstellung erhalten.

Beispiel:

```text
"Mach aus der Aufnahme Text"

"Transkribiere die Audiodatei"
```

werden zu:

```text
goal:
    media.audio.transcribe

input:
    Media.Audio

output:
    Document.Transcript
```

## Werte und Einheiten

Unterschiedliche Darstellungen gleicher Werte sollen vereinheitlicht werden.

Beispiel:

```text
2000 ms
2 s
```

können intern auf dieselbe kanonische Repräsentation abgebildet werden.

## Semantic Types

Eingaben und Ausgaben müssen auf bekannte Semantic Types aufgelöst werden.

Mögliche Zustände:

```text
RESOLVED
AMBIGUOUS
UNKNOWN
```

Mehrdeutige oder unbekannte Typen dürfen nicht stillschweigend als eindeutig behandelt werden.

## Constraints

Constraints müssen in eine einheitliche maschinenlesbare Form gebracht werden.

Beispiel:

```text
"nur lokal ausführen"
```

wird zu:

```text
execution_location:
    local_only
```

## Defaults

Fehlende optionale Werte dürfen durch definierte Defaults ergänzt werden.

Defaults dürfen explizite Nutzeranforderungen oder übergeordnete Policies nicht überschreiben.

## Provenance

Die Verbindung zwischen ursprünglichem und normalisiertem Intent muss erhalten bleiben.

```text
Original Intent
    ↓
Normalization Record
    ↓
Canonical Intent
```

Dadurch bleibt nachvollziehbar, wie die interne Darstellung entstanden ist.

## Beispiel

```text
Input:
    "Wandle diese Aufnahme lokal in Text um."

Canonical Intent {
    goal:
        media.audio.transcribe

    input:
        Media.Audio

    output:
        Document.Transcript

    constraints {
        execution_location:
            local_only
    }
}
```

## Normative Anforderungen

1. Unterschiedliche Darstellungen semantisch gleicher Intents MÜSSEN in eine gemeinsame kanonische Form überführbar sein.
2. Normalisierung DARF die Bedeutung expliziter Nutzeranforderungen nicht verändern.
3. Semantic Types, Constraints und Parameter MÜSSEN maschinenlesbar normalisiert werden können.
4. Mehrdeutige oder unbekannte Informationen MÜSSEN als solche erkennbar bleiben.
5. Defaults DÜRFEN explizite Anforderungen nicht überschreiben.
6. Werte und Einheiten SOLLEN kanonisch darstellbar sein.
7. Die Herkunft normalisierter Informationen MUSS nachvollziehbar bleiben.

## Abgrenzung

Diese NPSPEC definiert:

- Intent Normalization
- kanonische Darstellung
- Normalisierung von Typen, Werten und Constraints
- Erhaltung der ursprünglichen Semantik

Nicht Bestandteil sind:

- vollständige Semantic Analysis
- Intent Resolution
- Capability-Auswahl
- Execution-IR-Lowering

## Zugehörige NPSPECs

- `NPSPEC-INTENTC-0001 – Intent Compiler Architecture`
- `NPSPEC-INTENTC-0003 – Intent Semantic Analysis`
- `NPSPEC-INTENTC-0004 – Intent-to-Execution Lowering`
- `NPSPEC-INTENT-0003 – Intent Schema & Semantic Types`
- `NPSPEC-INTENT-0005 – Intent Constraints & Policies`